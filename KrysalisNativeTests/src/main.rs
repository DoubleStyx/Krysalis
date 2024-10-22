use KrysalisNative;

fn main() {
    let result = KrysalisNative::triangle::main_ffi();

    println!("Tests passed: {:?}", result);
    assert!(result);
}
 