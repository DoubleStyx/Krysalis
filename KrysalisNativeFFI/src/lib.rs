use KrysalisNativeWrapper;

#[no_mangle]
pub extern "C" fn main_ffi() -> bool {
    return main_wrapper();
}