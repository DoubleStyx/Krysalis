use KrysalisNative;

fn main() {
    let result = KrysalisNative::triangle::main();

    println!("Tests passed: {:?}", result.is_ok());
    assert!(result.is_ok());
}
 