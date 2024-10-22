// src/main.rs

use ash::version::{EntryV1_0, InstanceV1_0, DeviceV1_0};
use ash::{vk, Entry};
use std::ffi::{CStr, CString};
use std::os::raw::c_void;
use std::ptr;
use winit::{
    event::{Event, WindowEvent},
    event_loop::{ControlFlow, EventLoop},
    window::WindowBuilder,
};

// Constants
const WINDOW_TITLE: &str = "Vulkan Triangle";
const WINDOW_WIDTH: u32 = 800;
const WINDOW_HEIGHT: u32 = 600;

const VALIDATION_ENABLED: bool = true;
const VALIDATION_LAYER_NAME: &CStr = unsafe { CStr::from_bytes_with_nul_unchecked(b"VK_LAYER_KHRONOS_validation\0") };

// Vertex Data
#[repr(C)]
#[derive(Clone, Copy)]
struct Vertex {
    pos: [f32; 2],
    color: [f32; 3],
}

impl Vertex {
    fn get_binding_description() -> vk::VertexInputBindingDescription {
        vk::VertexInputBindingDescription {
            binding: 0,
            stride: std::mem::size_of::<Vertex>() as u32,
            input_rate: vk::VertexInputRate::VERTEX,
        }
    }

    fn get_attribute_descriptions() -> [vk::VertexInputAttributeDescription; 2] {
        [
            vk::VertexInputAttributeDescription {
                binding: 0,
                location: 0,
                format: vk::Format::R32G32_SFLOAT,
                offset: 0,
            },
            vk::VertexInputAttributeDescription {
                binding: 0,
                location: 1,
                format: vk::Format::R32G32B32_SFLOAT,
                offset: 8,
            },
        ]
    }
}

const VERTICES: [Vertex; 3] = [
    Vertex { pos: [0.0, -0.5], color: [1.0, 0.0, 0.0] },
    Vertex { pos: [0.5, 0.5], color: [0.0, 1.0, 0.0] },
    Vertex { pos: [-0.5, 0.5], color: [0.0, 0.0, 1.0] },
];

// Shader bytecode (SPIR-V)
// You need to compile these shaders to SPIR-V and include the binary here.
// For simplicity, we assume you have `vert.spv` and `frag.spv` compiled.

const VERT_SHADER_CODE: &[u8] = include_bytes!("shaders/vert.spv");
const FRAG_SHADER_CODE: &[u8] = include_bytes!("shaders/frag.spv");

// Main Application Structure
struct VulkanApp {
    // Winit
    window: winit::window::Window,
    // Vulkan
    entry: Entry,
    instance: ash::Instance,
    debug_utils_loader: Option<ash::extensions::ext::DebugUtils>,
    debug_messenger: vk::DebugUtilsMessengerEXT,
    surface_loader: ash::extensions::khr::Surface,
    surface: vk::SurfaceKHR,
    physical_device: vk::PhysicalDevice,
    device: ash::Device,
    graphics_queue: vk::Queue,
    present_queue: vk::Queue,
    swapchain_loader: ash::extensions::khr::Swapchain,
    swapchain: vk::SwapchainKHR,
    swapchain_images: Vec<vk::Image>,
    swapchain_format: vk::Format,
    swapchain_extent: vk::Extent2D,
    swapchain_imageviews: Vec<vk::ImageView>,
    render_pass: vk::RenderPass,
    pipeline_layout: vk::PipelineLayout,
    graphics_pipeline: vk::Pipeline,
    framebuffers: Vec<vk::Framebuffer>,
    command_pool: vk::CommandPool,
    command_buffers: Vec<vk::CommandBuffer>,
    vertex_buffer: vk::Buffer,
    vertex_buffer_memory: vk::DeviceMemory,
    // Synchronization
    image_available_semaphores: Vec<vk::Semaphore>,
    render_finished_semaphores: Vec<vk::Semaphore>,
    in_flight_fences: Vec<vk::Fence>,
    current_frame: usize,
}

impl VulkanApp {
    pub fn new(event_loop: &EventLoop<()>) -> VulkanApp {
        // Initialize Logger
        env_logger::init();

        // Initialize Vulkan
        let entry = unsafe { Entry::new().unwrap() };

        // Create Vulkan Instance
        let instance = Self::create_instance(&entry);

        // Setup Debug Messenger
        let (debug_utils_loader, debug_messenger) = if VALIDATION_ENABLED {
            let debug_utils_loader = ash::extensions::ext::DebugUtils::new(&entry, &instance);
            let debug_messenger = Self::setup_debug_messenger(&debug_utils_loader);
            (Some(debug_utils_loader), debug_messenger)
        } else {
            (None, vk::DebugUtilsMessengerEXT::null())
        };

        // Create Surface
        let surface = unsafe { ash_window::create_surface(&entry, &instance, event_loop, None).expect("Failed to create surface.") };
        let surface_loader = ash::extensions::khr::Surface::new(&entry, &instance);

        // Pick Physical Device
        let physical_device = Self::pick_physical_device(&instance, &surface_loader, surface);

        // Create Logical Device and Queues
        let (device, graphics_queue, present_queue) = Self::create_logical_device(&instance, physical_device, &surface_loader, surface);

        // Create Swapchain
        let swapchain_loader = ash::extensions::khr::Swapchain::new(&instance, &device);
        let (swapchain, swapchain_images, swapchain_format, swapchain_extent) =
            Self::create_swapchain(&instance, &device, physical_device, &surface_loader, surface, &swapchain_loader);

        // Create Image Views
        let swapchain_imageviews = Self::create_image_views(&device, &swapchain_images, swapchain_format);

        // Create Render Pass
        let render_pass = Self::create_render_pass(&device, swapchain_format);

        // Create Graphics Pipeline
        let (pipeline_layout, graphics_pipeline) = Self::create_graphics_pipeline(&device, render_pass, swapchain_extent);

        // Create Framebuffers
        let framebuffers = Self::create_framebuffers(&device, render_pass, &swapchain_imageviews, swapchain_extent);

        // Create Command Pool
        let command_pool = Self::create_command_pool(&device, physical_device, &surface_loader, surface);

        // Create Vertex Buffer
        let (vertex_buffer, vertex_buffer_memory) = Self::create_vertex_buffer(&instance, &device, physical_device, command_pool, graphics_queue);

        // Create Command Buffers
        let command_buffers = Self::create_command_buffers(&device, command_pool, &framebuffers, render_pass, graphics_pipeline, swapchain_extent, vertex_buffer);

        // Create Synchronization Objects
        let (image_available_semaphores, render_finished_semaphores, in_flight_fences) = Self::create_sync_objects(&device);

        VulkanApp {
            window: WindowBuilder::new()
                .with_title(WINDOW_TITLE)
                .with_inner_size(winit::dpi::LogicalSize::new(WINDOW_WIDTH, WINDOW_HEIGHT))
                .build(event_loop)
                .unwrap(),
            entry,
            instance,
            debug_utils_loader,
            debug_messenger,
            surface_loader,
            surface,
            physical_device,
            device,
            graphics_queue,
            present_queue,
            swapchain_loader,
            swapchain,
            swapchain_images,
            swapchain_format,
            swapchain_extent,
            swapchain_imageviews,
            render_pass,
            pipeline_layout,
            graphics_pipeline,
            framebuffers,
            command_pool,
            command_buffers,
            vertex_buffer,
            vertex_buffer_memory,
            image_available_semaphores,
            render_finished_semaphores,
            in_flight_fences,
            current_frame: 0,
        }
    }

    fn create_instance(entry: &ash::Entry) -> ash::Instance {
        if VALIDATION_ENABLED && !Self::check_validation_layer_support(entry) {
            panic!("Validation layers requested, but not available!");
        }

        let app_name = CString::new("Hello Triangle").unwrap();
        let engine_name = CString::new("No Engine").unwrap();

        let app_info = vk::ApplicationInfo::builder()
            .application_name(&app_name)
            .application_version(vk::make_version(1, 0, 0))
            .engine_name(&engine_name)
            .engine_version(vk::make_version(1, 0, 0))
            .api_version(vk::make_version(1, 0, 0));

        // Get required extensions from winit
        let mut extension_names = ash_window::enumerate_required_extensions(&ash::vk::KhrSurfaceFn::name())
            .expect("Failed to get required extensions")
            .to_vec();

        if VALIDATION_ENABLED {
            extension_names.push(ash::extensions::ext::DebugUtils::name().as_ptr());
        }

        // Setup Validation Layers
        let layers_names = [VALIDATION_LAYER_NAME];
        let layers_names_ptr: Vec<*const i8> = layers_names.iter().map(|layer| layer.as_ptr()).collect();

        let create_info = vk::InstanceCreateInfo::builder()
            .application_info(&app_info)
            .enabled_extension_names(&extension_names)
            .enabled_layer_names(&layers_names_ptr)
            .build();

        unsafe {
            entry.create_instance(&create_info, None).expect("Instance creation error")
        }
    }

    fn check_validation_layer_support(entry: &ash::Entry) -> bool {
        let available_layers = entry.enumerate_instance_layer_properties().expect("Failed to enumerate layers");

        available_layers.iter().any(|layer| {
            let layer_name = unsafe { CStr::from_ptr(layer.layer_name.as_ptr()) };
            layer_name == VALIDATION_LAYER_NAME
        })
    }

    fn setup_debug_messenger(debug_utils_loader: &ash::extensions::ext::DebugUtils) -> vk::DebugUtilsMessengerEXT {
        let create_info = vk::DebugUtilsMessengerCreateInfoEXT::builder()
            .message_severity(
                vk::DebugUtilsMessageSeverityFlagsEXT::VERBOSE |
                vk::DebugUtilsMessageSeverityFlagsEXT::WARNING |
                vk::DebugUtilsMessageSeverityFlagsEXT::ERROR,
            )
            .message_type(
                vk::DebugUtilsMessageTypeFlagsEXT::GENERAL |
                vk::DebugUtilsMessageTypeFlagsEXT::VALIDATION |
                vk::DebugUtilsMessageTypeFlagsEXT::PERFORMANCE,
            )
            .pfn_user_callback(Some(vulkan_debug_callback));

        unsafe {
            debug_utils_loader
                .create_debug_utils_messenger(&create_info, None)
                .expect("Debug Utils Callback")
        }
    }

    fn pick_physical_device(instance: &ash::Instance, surface_loader: &ash::extensions::khr::Surface, surface: vk::SurfaceKHR) -> vk::PhysicalDevice {
        let devices = unsafe { instance.enumerate_physical_devices().expect("Physical device error") };

        devices
            .iter()
            .find(|&&device| Self::is_device_suitable(instance, device, surface_loader, surface))
            .copied()
            .expect("No suitable physical device found")
    }

    fn is_device_suitable(instance: &ash::Instance, device: vk::PhysicalDevice, surface_loader: &ash::extensions::khr::Surface, surface: vk::SurfaceKHR) -> bool {
        let indices = Self::find_queue_families(instance, device, surface_loader, surface);
        indices.is_complete()
    }

    struct QueueFamilyIndices {
        graphics_family: Option<u32>,
        present_family: Option<u32>,
    }

    impl QueueFamilyIndices {
        fn new() -> Self {
            QueueFamilyIndices {
                graphics_family: None,
                present_family: None,
            }
        }

        fn is_complete(&self) -> bool {
            self.graphics_family.is_some() && self.present_family.is_some()
        }
    }

    fn find_queue_families(instance: &ash::Instance, device: vk::PhysicalDevice, surface_loader: &ash::extensions::khr::Surface, surface: vk::SurfaceKHR) -> QueueFamilyIndices {
        let mut indices = QueueFamilyIndices::new();

        let queue_families = unsafe { instance.get_physical_device_queue_family_properties(device) };

        for (i, queue_family) in queue_families.iter().enumerate() {
            if queue_family.queue_flags.contains(vk::QueueFlags::GRAPHICS) {
                indices.graphics_family = Some(i as u32);
            }

            let present_support = unsafe {
                surface_loader.get_physical_device_surface_support(device, i as u32, surface).expect("Failed to get physical device surface support")
            };

            if present_support {
                indices.present_family = Some(i as u32);
            }

            if indices.is_complete() {
                break;
            }
        }

        indices
    }

    fn create_logical_device(instance: &ash::Instance, physical_device: vk::PhysicalDevice, surface_loader: &ash::extensions::khr::Surface, surface: vk::SurfaceKHR) -> (ash::Device, vk::Queue, vk::Queue) {
        let indices = Self::find_queue_families(instance, physical_device, surface_loader, surface);

        let unique_queue_families = if indices.graphics_family != indices.present_family {
            vec![indices.graphics_family.unwrap(), indices.present_family.unwrap()]
        } else {
            vec![indices.graphics_family.unwrap()]
        };

        let queue_priorities = [1.0_f32];
        let queue_create_infos: Vec<vk::DeviceQueueCreateInfo> = unique_queue_families.iter().map(|&queue_family| {
            vk::DeviceQueueCreateInfo::builder()
                .queue_family_index(queue_family)
                .queue_priorities(&queue_priorities)
                .build()
        }).collect();

        let device_extensions = [ash::extensions::khr::Swapchain::name().as_ptr()];

        let layers_names = [VALIDATION_LAYER_NAME];
        let layers_names_ptr: Vec<*const i8> = layers_names.iter().map(|layer| layer.as_ptr()).collect();

        let device_features = vk::PhysicalDeviceFeatures::builder();

        let create_info = vk::DeviceCreateInfo::builder()
            .queue_create_infos(&queue_create_infos)
            .enabled_extension_names(&device_extensions)
            .enabled_layer_names(&layers_names_ptr)
            .enabled_features(&device_features);

        let device: ash::Device = unsafe {
            instance.create_device(physical_device, &create_info, None)
                .expect("Failed to create logical device")
        };

        let graphics_queue = unsafe { device.get_device_queue(indices.graphics_family.unwrap(), 0) };
        let present_queue = unsafe { device.get_device_queue(indices.present_family.unwrap(), 0) };

        (device, graphics_queue, present_queue)
    }

    fn create_swapchain(instance: &ash::Instance, device: &ash::Device, physical_device: vk::PhysicalDevice, surface_loader: &ash::extensions::khr::Surface, surface: vk::SurfaceKHR, swapchain_loader: &ash::extensions::khr::Swapchain) -> (vk::SwapchainKHR, Vec<vk::Image>, vk::Format, vk::Extent2D) {
        let swapchain_support = Self::query_swapchain_support(instance, physical_device, surface_loader, surface);

        let surface_format = swapchain_support.formats.iter()
            .find(|&format| format.format == vk::Format::B8G8R8A8_SRGB && format.color_space == vk::ColorSpaceKHR::SRGB_NONLINEAR)
            .unwrap_or(&swapchain_support.formats[0]);

        let present_mode = if swapchain_support.present_modes.contains(&vk::PresentModeKHR::MAILBOX) {
            vk::PresentModeKHR::MAILBOX
        } else {
            vk::PresentModeKHR::FIFO
        };

        let extent = Self::choose_swap_extent(&swapchain_support.capabilities, &ash_window::window_extent(&*swapchain_support.capabilities.current_extent));

        let image_count = swapchain_support.capabilities.min_image_count + 1;
        let image_count = if swapchain_support.capabilities.max_image_count > 0 && image_count > swapchain_support.capabilities.max_image_count {
            swapchain_support.capabilities.max_image_count
        } else {
            image_count
        };

        let indices = Self::find_queue_families(instance, physical_device, surface_loader, surface);

        let queue_family_indices = [indices.graphics_family.unwrap(), indices.present_family.unwrap()];

        let image_sharing_mode;
        let queue_family_index_count;
        let p_queue_family_indices;

        if indices.graphics_family != indices.present_family {
            image_sharing_mode = vk::SharingMode::CONCURRENT;
            queue_family_index_count = queue_family_indices.len() as u32;
            p_queue_family_indices = queue_family_indices.as_ptr();
        } else {
            image_sharing_mode = vk::SharingMode::EXCLUSIVE;
            queue_family_index_count = 0;
            p_queue_family_indices = ptr::null();
        }

        let swapchain_create_info = vk::SwapchainCreateInfoKHR::builder()
            .surface(surface)
            .min_image_count(image_count)
            .image_format(surface_format.format)
            .image_color_space(surface_format.color_space)
            .image_extent(extent)
            .image_array_layers(1)
            .image_usage(vk::ImageUsageFlags::COLOR_ATTACHMENT)
            .image_sharing_mode(image_sharing_mode)
            .queue_family_indices(&queue_family_indices)
            .pre_transform(swapchain_support.capabilities.current_transform)
            .composite_alpha(vk::CompositeAlphaFlagsKHR::OPAQUE)
            .present_mode(present_mode)
            .clipped(true)
            .old_swapchain(vk::SwapchainKHR::null());

        let swapchain = unsafe {
            swapchain_loader.create_swapchain(&swapchain_create_info, None)
                .expect("Failed to create swapchain")
        };

        let images = unsafe { swapchain_loader.get_swapchain_images(swapchain).expect("Failed to get swapchain images") };

        (swapchain, images, surface_format.format, extent)
    }

    struct SwapChainSupportDetails {
        capabilities: vk::SurfaceCapabilitiesKHR,
        formats: Vec<vk::SurfaceFormatKHR>,
        present_modes: Vec<vk::PresentModeKHR>,
    }

    fn query_swapchain_support(instance: &ash::Instance, device: vk::PhysicalDevice, surface_loader: &ash::extensions::khr::Surface, surface: vk::SurfaceKHR) -> SwapChainSupportDetails {
        let capabilities = unsafe {
            surface_loader.get_physical_device_surface_capabilities(device, surface)
                .expect("Failed to get physical device surface capabilities")
        };

        let formats = unsafe {
            surface_loader.get_physical_device_surface_formats(device, surface)
                .expect("Failed to get physical device surface formats")
        };

        let present_modes = unsafe {
            surface_loader.get_physical_device_surface_present_modes(device, surface)
                .expect("Failed to get physical device surface present modes")
        };

        SwapChainSupportDetails {
            capabilities,
            formats,
            present_modes,
        }
    }

    fn choose_swap_extent(capabilities: &vk::SurfaceCapabilitiesKHR, window_extent: &vk::Extent2D) -> vk::Extent2D {
        if capabilities.current_extent.width != u32::MAX {
            capabilities.current_extent
        } else {
            vk::Extent2D {
                width: WINDOW_WIDTH.clamp(capabilities.min_image_extent.width, capabilities.max_image_extent.width),
                height: WINDOW_HEIGHT.clamp(capabilities.min_image_extent.height, capabilities.max_image_extent.height),
            }
        }
    }

    fn create_image_views(device: &ash::Device, swapchain_images: &[vk::Image], format: vk::Format) -> Vec<vk::ImageView> {
        swapchain_images.iter().map(|&image| {
            let create_info = vk::ImageViewCreateInfo::builder()
                .image(image)
                .view_type(vk::ImageViewType::TYPE_2D)
                .format(format)
                .components(vk::ComponentMapping {
                    r: vk::ComponentSwizzle::IDENTITY,
                    g: vk::ComponentSwizzle::IDENTITY,
                    b: vk::ComponentSwizzle::IDENTITY,
                    a: vk::ComponentSwizzle::IDENTITY,
                })
                .subresource_range(vk::ImageSubresourceRange {
                    aspect_mask: vk::ImageAspectFlags::COLOR,
                    base_mip_level: 0,
                    level_count: 1,
                    base_array_layer: 0,
                    layer_count: 1,
                });

            unsafe {
                device.create_image_view(&create_info, None)
                    .expect("Failed to create image view")
            }
        }).collect()
    }

    fn create_render_pass(device: &ash::Device, format: vk::Format) -> vk::RenderPass {
        let color_attachment = vk::AttachmentDescription::builder()
            .format(format)
            .samples(vk::SampleCountFlags::TYPE_1)
            .load_op(vk::AttachmentLoadOp::CLEAR)
            .store_op(vk::AttachmentStoreOp::STORE)
            .stencil_load_op(vk::AttachmentLoadOp::DONT_CARE)
            .stencil_store_op(vk::AttachmentStoreOp::DONT_CARE)
            .initial_layout(vk::ImageLayout::UNDEFINED)
            .final_layout(vk::ImageLayout::PRESENT_SRC_KHR)
            .build();

        let color_attachment_ref = vk::AttachmentReference {
            attachment: 0,
            layout: vk::ImageLayout::COLOR_ATTACHMENT_OPTIMAL,
        };

        let subpass = vk::SubpassDescription::builder()
            .pipeline_bind_point(vk::PipelineBindPoint::GRAPHICS)
            .color_attachments(std::slice::from_ref(&color_attachment_ref))
            .build();

        let dependency = vk::SubpassDependency::builder()
            .src_subpass(vk::SUBPASS_EXTERNAL)
            .dst_subpass(0)
            .src_stage_mask(vk::PipelineStageFlags::COLOR_ATTACHMENT_OUTPUT)
            .src_access_mask(vk::AccessFlags::empty())
            .dst_stage_mask(vk::PipelineStageFlags::COLOR_ATTACHMENT_OUTPUT)
            .dst_access_mask(vk::AccessFlags::COLOR_ATTACHMENT_WRITE)
            .build();

        let render_pass_info = vk::RenderPassCreateInfo::builder()
            .attachments(std::slice::from_ref(&color_attachment))
            .subpasses(std::slice::from_ref(&subpass))
            .dependencies(std::slice::from_ref(&dependency))
            .build();

        unsafe {
            device.create_render_pass(&render_pass_info, None)
                .expect("Failed to create render pass")
        }
    }

    fn create_graphics_pipeline(device: &ash::Device, render_pass: vk::RenderPass, extent: vk::Extent2D) -> (vk::PipelineLayout, vk::Pipeline) {
        // Load shaders
        let vert_shader_module = Self::create_shader_module(device, VERT_SHADER_CODE);
        let frag_shader_module = Self::create_shader_module(device, FRAG_SHADER_CODE);

        let vert_shader_stage_info = vk::PipelineShaderStageCreateInfo::builder()
            .stage(vk::ShaderStageFlags::VERTEX)
            .module(vert_shader_module)
            .name(CString::new("main").unwrap().as_c_str())
            .build();

        let frag_shader_stage_info = vk::PipelineShaderStageCreateInfo::builder()
            .stage(vk::ShaderStageFlags::FRAGMENT)
            .module(frag_shader_module)
            .name(CString::new("main").unwrap().as_c_str())
            .build();

        let shader_stages = [vert_shader_stage_info, frag_shader_stage_info];

        let vertex_input_info = vk::PipelineVertexInputStateCreateInfo::builder()
            .vertex_binding_descriptions(&[Vertex::get_binding_description()])
            .vertex_attribute_descriptions(&Vertex::get_attribute_descriptions())
            .build();

        let input_assembly = vk::PipelineInputAssemblyStateCreateInfo::builder()
            .topology(vk::PrimitiveTopology::TRIANGLE_LIST)
            .primitive_restart_enable(false)
            .build();

        let viewport = vk::Viewport {
            x: 0.0,
            y: 0.0,
            width: extent.width as f32,
            height: extent.height as f32,
            min_depth: 0.0,
            max_depth: 1.0,
        };

        let scissor = vk::Rect2D {
            offset: vk::Offset2D { x: 0, y: 0 },
            extent,
        };

        let viewport_state = vk::PipelineViewportStateCreateInfo::builder()
            .viewports(std::slice::from_ref(&viewport))
            .scissors(std::slice::from_ref(&scissor))
            .build();

        let rasterizer = vk::PipelineRasterizationStateCreateInfo::builder()
            .depth_clamp_enable(false)
            .rasterizer_discard_enable(false)
            .polygon_mode(vk::PolygonMode::FILL)
            .line_width(1.0)
            .cull_mode(vk::CullModeFlags::BACK)
            .front_face(vk::FrontFace::COUNTER_CLOCKWISE)
            .depth_bias_enable(false)
            .build();

        let multisampling = vk::PipelineMultisampleStateCreateInfo::builder()
            .sample_shading_enable(false)
            .rasterization_samples(vk::SampleCountFlags::TYPE_1)
            .build();

        let color_blend_attachment = vk::PipelineColorBlendAttachmentState::builder()
            .color_write_mask(
                vk::ColorComponentFlags::R |
                vk::ColorComponentFlags::G |
                vk::ColorComponentFlags::B |
                vk::ColorComponentFlags::A
            )
            .blend_enable(false)
            .build();

        let color_blending = vk::PipelineColorBlendStateCreateInfo::builder()
            .logic_op_enable(false)
            .attachments(std::slice::from_ref(&color_blend_attachment))
            .build();

        let pipeline_layout_info = vk::PipelineLayoutCreateInfo::builder()
            .set_layouts(&[])
            .push_constant_ranges(&[])
            .build();

        let pipeline_layout = unsafe {
            device.create_pipeline_layout(&pipeline_layout_info, None)
                .expect("Failed to create pipeline layout")
        };

        let pipeline_info = vk::GraphicsPipelineCreateInfo::builder()
            .stages(&shader_stages)
            .vertex_input_state(&vertex_input_info)
            .input_assembly_state(&input_assembly)
            .viewport_state(&viewport_state)
            .rasterization_state(&rasterizer)
            .multisample_state(&multisampling)
            .color_blend_state(&color_blending)
            .layout(pipeline_layout)
            .render_pass(render_pass)
            .subpass(0)
            .build();

        let graphics_pipelines = unsafe {
            device.create_graphics_pipelines(vk::PipelineCache::null(), &[pipeline_info], None)
                .expect("Failed to create graphics pipeline")
        };

        unsafe {
            device.destroy_shader_module(vert_shader_module, None);
            device.destroy_shader_module(frag_shader_module, None);
        }

        (pipeline_layout, graphics_pipelines[0])
    }

    fn create_shader_module(device: &ash::Device, code: &[u8]) -> vk::ShaderModule {
        let shader_info = vk::ShaderModuleCreateInfo::builder()
            .code(bytemuck::cast_slice(code))
            .build();

        unsafe {
            device.create_shader_module(&shader_info, None)
                .expect("Failed to create shader module")
        }
    }

    fn create_framebuffers(device: &ash::Device, render_pass: vk::RenderPass, image_views: &[vk::ImageView], extent: vk::Extent2D) -> Vec<vk::Framebuffer> {
        image_views.iter().map(|&image_view| {
            let framebuffer_info = vk::FramebufferCreateInfo::builder()
                .render_pass(render_pass)
                .attachments(std::slice::from_ref(&image_view))
                .width(extent.width)
                .height(extent.height)
                .layers(1)
                .build();

            unsafe {
                device.create_framebuffer(&framebuffer_info, None)
                    .expect("Failed to create framebuffer")
            }
        }).collect()
    }

    fn create_command_pool(device: &ash::Device, physical_device: vk::PhysicalDevice, surface_loader: &ash::extensions::khr::Surface, surface: vk::SurfaceKHR) -> vk::CommandPool {
        let queue_family_indices = Self::find_queue_families(&device.instance().handle(), physical_device, surface_loader, surface);

        let pool_info = vk::CommandPoolCreateInfo::builder()
            .queue_family_index(queue_family_indices.graphics_family.unwrap())
            .flags(vk::CommandPoolCreateFlags::RESET_COMMAND_BUFFER)
            .build();

        unsafe {
            device.create_command_pool(&pool_info, None)
                .expect("Failed to create command pool")
        }
    }

    fn create_vertex_buffer(entry: &ash::Entry, device: &ash::Device, physical_device: vk::PhysicalDevice, command_pool: vk::CommandPool, graphics_queue: vk::Queue) -> (vk::Buffer, vk::DeviceMemory) {
        let buffer_size = (std::mem::size_of::<Vertex>() * VERTICES.len()) as vk::DeviceSize;

        let buffer_info = vk::BufferCreateInfo::builder()
            .size(buffer_size)
            .usage(vk::BufferUsageFlags::VERTEX_BUFFER)
            .sharing_mode(vk::SharingMode::EXCLUSIVE)
            .build();

        let vertex_buffer = unsafe {
            device.create_buffer(&buffer_info, None)
                .expect("Failed to create vertex buffer")
        };

        let mem_requirements = unsafe { device.get_buffer_memory_requirements(vertex_buffer) };

        let mem_type = Self::find_memory_type(physical_device, mem_requirements.memory_type_bits, vk::MemoryPropertyFlags::HOST_VISIBLE | vk::MemoryPropertyFlags::HOST_COHERENT);

        let alloc_info = vk::MemoryAllocateInfo::builder()
            .allocation_size(mem_requirements.size)
            .memory_type_index(mem_type)
            .build();

        let vertex_buffer_memory = unsafe {
            device.allocate_memory(&alloc_info, None)
                .expect("Failed to allocate vertex buffer memory")
        };

        unsafe {
            device.bind_buffer_memory(vertex_buffer, vertex_buffer_memory, 0)
                .expect("Failed to bind vertex buffer memory");
        }

        // Map and copy data
        let data_ptr = unsafe {
            device.map_memory(vertex_buffer_memory, 0, buffer_size, vk::MemoryMapFlags::empty())
                .expect("Failed to map memory") as *mut Vertex
        };

        unsafe {
            ptr::copy_nonoverlapping(VERTICES.as_ptr(), data_ptr, VERTICES.len());
            device.unmap_memory(vertex_buffer_memory);
        }

        (vertex_buffer, vertex_buffer_memory)
    }

    fn find_memory_type(physical_device: vk::PhysicalDevice, type_filter: u32, properties: vk::MemoryPropertyFlags) -> u32 {
        let mem_properties = unsafe {
            Self::get_instance().get_physical_device_memory_properties(physical_device)
        };

        for (i, memory_type) in mem_properties.memory_types.iter().enumerate() {
            if (type_filter & (1 << i)) != 0 && memory_type.property_flags.contains(properties) {
                return i as u32;
            }
        }

        panic!("Failed to find suitable memory type!")
    }

    fn get_instance() -> ash::Instance {
        // Placeholder for actual instance retrieval
        panic!("Instance retrieval not implemented")
    }

    fn create_command_buffers(device: &ash::Device, command_pool: vk::CommandPool, framebuffers: &[vk::Framebuffer], render_pass: vk::RenderPass, pipeline: vk::Pipeline, extent: vk::Extent2D, vertex_buffer: vk::Buffer) -> Vec<vk::CommandBuffer> {
        let allocate_info = vk::CommandBufferAllocateInfo::builder()
            .command_pool(command_pool)
            .level(vk::CommandBufferLevel::PRIMARY)
            .command_buffer_count(framebuffers.len() as u32)
            .build();

        let command_buffers = unsafe {
            device.allocate_command_buffers(&allocate_info)
                .expect("Failed to allocate command buffers")
        };

        for (i, &command_buffer) in command_buffers.iter().enumerate() {
            let begin_info = vk::CommandBufferBeginInfo::builder()
                .flags(vk::CommandBufferUsageFlags::SIMULTANEOUS_USE)
                .build();

            unsafe {
                device.begin_command_buffer(command_buffer, &begin_info)
                    .expect("Failed to begin recording command buffer");
            }

            let clear_values = [vk::ClearValue {
                color: vk::ClearColorValue {
                    float32: [0.0, 0.0, 0.0, 1.0],
                },
            }];

            let render_pass_info = vk::RenderPassBeginInfo::builder()
                .render_pass(render_pass)
                .framebuffer(framebuffers[i])
                .render_area(vk::Rect2D {
                    offset: vk::Offset2D { x: 0, y: 0 },
                    extent,
                })
                .clear_values(&clear_values)
                .build();

            unsafe {
                device.cmd_begin_render_pass(command_buffer, &render_pass_info, vk::SubpassContents::INLINE);
                device.cmd_bind_pipeline(command_buffer, vk::PipelineBindPoint::GRAPHICS, pipeline);
                device.cmd_bind_vertex_buffers(command_buffer, 0, &[vertex_buffer], &[0]);
                device.cmd_draw(command_buffer, VERTICES.len() as u32, 1, 0, 0);
                device.cmd_end_render_pass(command_buffer);
                device.end_command_buffer(command_buffer)
                    .expect("Failed to record command buffer");
            }
        }

        command_buffers
    }

    fn create_sync_objects(device: &ash::Device) -> (Vec<vk::Semaphore>, Vec<vk::Semaphore>, Vec<vk::Fence>) {
        let semaphore_info = vk::SemaphoreCreateInfo::builder().build();
        let fence_info = vk::FenceCreateInfo::builder()
            .flags(vk::FenceCreateFlags::SIGNALED)
            .build();

        let mut image_available_semaphores = Vec::with_capacity(2);
        let mut render_finished_semaphores = Vec::with_capacity(2);
        let mut in_flight_fences = Vec::with_capacity(2);

        for _ in 0..2 {
            let semaphore = unsafe {
                device.create_semaphore(&semaphore_info, None)
                    .expect("Failed to create semaphore")
            };
            image_available_semaphores.push(semaphore);

            let semaphore = unsafe {
                device.create_semaphore(&semaphore_info, None)
                    .expect("Failed to create semaphore")
            };
            render_finished_semaphores.push(semaphore);

            let fence = unsafe {
                device.create_fence(&fence_info, None)
                    .expect("Failed to create fence")
            };
            in_flight_fences.push(fence);
        }

        (image_available_semaphores, render_finished_semaphores, in_flight_fences)
    }

    fn create_instance() -> ash::Instance {
        // Placeholder: This function is already implemented above
        panic!("Instance creation already implemented")
    }

    fn draw_frame(&mut self) {
        // Wait for the previous frame
        unsafe {
            self.device.wait_for_fences(&[self.in_flight_fences[self.current_frame]], true, std::u64::MAX)
                .expect("Failed to wait for fence");
        }

        // Acquire next image
        let (image_index, _) = unsafe {
            match self.swapchain_loader.acquire_next_image(self.swapchain, std::u64::MAX, self.image_available_semaphores[self.current_frame], vk::Fence::null()) {
                Ok((img, suboptimal)) => (img, suboptimal),
                Err(vk::Result::ERROR_OUT_OF_DATE_KHR) => {
                    // Handle swapchain recreation
                    return;
                },
                Err(error) => panic!("Failed to acquire next image: {:?}", error),
            }
        };

        // Submit command buffer
        let wait_semaphores = [self.image_available_semaphores[self.current_frame]];
        let wait_stages = [vk::PipelineStageFlags::COLOR_ATTACHMENT_OUTPUT];
        let signal_semaphores = [self.render_finished_semaphores[self.current_frame]];

        let submit_info = vk::SubmitInfo::builder()
            .wait_semaphores(&wait_semaphores)
            .wait_dst_stage_mask(&wait_stages)
            .command_buffers(&[self.command_buffers[image_index as usize]])
            .signal_semaphores(&signal_semaphores)
            .build();

        unsafe {
            self.device.reset_fences(&[self.in_flight_fences[self.current_frame]]).expect("Failed to reset fences");

            self.device.queue_submit(self.graphics_queue, &[submit_info], self.in_flight_fences[self.current_frame])
                .expect("Failed to submit queue");
        }

        // Present the image
        let swapchains = [self.swapchain];
        let image_indices = [image_index];

        let present_info = vk::PresentInfoKHR::builder()
            .wait_semaphores(&signal_semaphores)
            .swapchains(&swapchains)
            .image_indices(&image_indices)
            .build();

        unsafe {
            match self.swapchain_loader.queue_present(self.present_queue, &present_info) {
                Ok(_) => {},
                Err(vk::Result::ERROR_OUT_OF_DATE_KHR) => {
                    // Handle swapchain recreation
                },
                Err(error) => panic!("Failed to present queue: {:?}", error),
            }
        }

        self.current_frame = (self.current_frame + 1) % 2;
    }

    // Additional functions like cleanup, recreate_swapchain, etc., would go here.
}

// Debug callback
unsafe extern "system" fn vulkan_debug_callback(
    message_severity: vk::DebugUtilsMessageSeverityFlagsEXT,
    message_types: vk::DebugUtilsMessageTypeFlagsEXT,
    p_callback_data: *const vk::DebugUtilsMessengerCallbackDataEXT,
    _p_user_data: *mut c_void,
) -> vk::Bool32 {
    let message = CStr::from_ptr((*p_callback_data).p_message);
    println!("Debug: {:?}", message);
    vk::FALSE
}

impl Drop for VulkanApp {
    fn drop(&mut self) {
        unsafe {
            self.device.device_wait_idle().expect("Failed to wait device idle");

            // Cleanup Vulkan resources here
            // Destroy synchronization objects, buffers, pipelines, etc.

            self.device.destroy_buffer(self.vertex_buffer, None);
            self.device.free_memory(self.vertex_buffer_memory, None);

            for &semaphore in self.render_finished_semaphores.iter() {
                self.device.destroy_semaphore(semaphore, None);
            }
            for &semaphore in self.image_available_semaphores.iter() {
                self.device.destroy_semaphore(semaphore, None);
            }
            for &fence in self.in_flight_fences.iter() {
                self.device.destroy_fence(fence, None);
            }

            self.device.destroy_command_pool(self.command_pool, None);

            self.device.destroy_pipeline(self.graphics_pipeline, None);
            self.device.destroy_pipeline_layout(self.pipeline_layout, None);
            self.device.destroy_render_pass(self.render_pass, None);

            for &framebuffer in self.framebuffers.iter() {
                self.device.destroy_framebuffer(framebuffer, None);
            }

            for &image_view in self.swapchain_imageviews.iter() {
                self.device.destroy_image_view(image_view, None);
            }

            self.swapchain_loader.destroy_swapchain(self.swapchain, None);
            self.device.destroy_device(None);

            if VALIDATION_ENABLED {
                if let Some(debug_utils_loader) = &self.debug_utils_loader {
                    debug_utils_loader.destroy_debug_utils_messenger(self.debug_messenger, None);
                }
            }

            self.surface_loader.destroy_surface(self.surface, None);
            self.instance.destroy_instance(None);
        }
    }
}

fn main() {
    // Create event loop
    let event_loop = EventLoop::new();

    // Initialize Vulkan App
    let mut app = VulkanApp::new(&event_loop);

    // Run event loop
    event_loop.run(move |event, _, control_flow| {
        *control_flow = ControlFlow::Poll;

        match event {
            Event::MainEventsCleared => {
                app.window.request_redraw();
            },
            Event::RedrawRequested(_) => {
                app.draw_frame();
            },
            Event::WindowEvent { event: WindowEvent::CloseRequested, .. } => {
                *control_flow = ControlFlow::Exit;
            },
            _ => {},
        }
    });
}
