use KrysalisNative;

fn main() {
    let result = KrysalisNative::triangle::main();

    // Optionally, assert something about the result (assuming init returns something)
    assert!(result.is_ok(), "Initialization failed");
}
