#![no_std]

extern crate alloc;

use alloc::string::String;

/// Formats a startup message with the given image path.
/// This is pure logic and does not depend on UEFI.
pub fn format_image_path_message(path: &str) -> String {
    let mut s = String::from("UEFI application started. Image path: ");
    s.push_str(path);
    s
}

#[cfg(test)]
extern crate std;

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn formats_message() {
        let out = format_image_path_message("PciRoot(0x0)/Foo");
        assert_eq!(
            out,
            "UEFI application started. Image path: PciRoot(0x0)/Foo"
        );
    }
}
