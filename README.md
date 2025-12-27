# Tangue

[![Rust CI](https://github.com/clement-antoine-xavier/tangue/actions/workflows/rust.yml/badge.svg)](https://github.com/clement-antoine-xavier/tangue/actions/workflows/rust.yml)

Tangue is an operating system kernel written in Rust, designed to run on UEFI-compatible hardware. It aims to provide a robust and efficient foundation for building secure and high-performance applications.

## Features

- Written in Rust for safety and performance
- UEFI support for modern hardware compatibility
- Modular architecture for easy extension and customization
- Comprehensive documentation and examples to get started quickly
- Active development and community support
- Cross-compilation support for multiple architectures
- Built-in tools for debugging and profiling
- Integration with popular Rust libraries and frameworks
- Regular updates to leverage the latest Rust features and improvements
- Focus on security and reliability for critical applications
- Support for various file systems and storage devices
- Advanced memory management techniques
- Networking capabilities for modern applications
- Support for multi-core processors and parallelism
- Easy-to-use build system and toolchain configuration

## Getting Started

To get started with Tangue, follow these steps:

1. Clone the repository:

   ```bash
   git clone https://github.com/clement-antoine-xavier/tangue.git
    cd tangue
    ```

2. Install the required Rust toolchain and components:

    ```bash
    rustup install stable
    rustup component add rust-src rust-std clippy rustfmt
    rustup target add aarch64-unknown-uefi i686-unknown-uefi x86_64-unknown-uefi
    ```

3. Build the kernel for all supported architectures:

    ```bash
    cargo build --target i686-unknown-uefi --target x86_64-unknown-uefi --target aarch64-unknown-uefi --release
    ```

## Testing on Host

This project targets UEFI and uses `#![no_std]` for the binary, so `cargo test` is incompatible with UEFI targets. To run unit tests on your macOS host, we provide a small `src/lib.rs` with pure logic and tests.

- Run only library tests to avoid compiling the UEFI binary under the test harness:

    ```bash
    cargo test --lib --verbose
    ```

- Explanation: the UEFI binary in `src/main.rs` cannot be built with Rust’s `libtest` (it links `std` and conflicts with the UEFI panic handler). `--lib` compiles and runs just the host-side unit tests.
