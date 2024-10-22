use KrysalisNative;

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_init() {
        // Call the init() function from KrysalisNative
        let result = KrysalisNative::init();

        // Optionally, assert something about the result (assuming init returns something)
        assert!(result, "Initialization failed");
    }
}
