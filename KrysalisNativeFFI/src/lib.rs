use KrysalisNativeWrapper::main_wrapper;

#[no_mangle]
pub extern "C" fn main_ffi() -> bool {
    return main_wrapper();
}