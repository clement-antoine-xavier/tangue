# Tangue - Containerization Application

## Description

Tangue is a containerization application for Linux that enables users to create, run, and manage isolated environments (containers). The project focuses on implementing core containerization features, including process isolation, resource management, network isolation, and filesystem abstraction.

By leveraging Linux technologies such as **namespaces** and **cgroups**, Tangue ensures that containers are fully isolated from each other and the host system. This project provides a hands-on experience with containerization concepts and system-level programming.

## Key Features

- **Process Isolation**: Containers are isolated from the host system and each other using Linux namespaces (PID, MNT, NET).
- **Resource Management**: Limit the resource usage (CPU, memory, etc.) of containers through cgroups.
- **Container Lifecycle Management**: Support for creating, starting, stopping, and deleting containers.
- **Filesystem Isolation**: Containers use isolated filesystems, which can be implemented with techniques like `overlayfs`.
- **Networking**: Containers have their own network stack, allowing them to communicate while remaining isolated from the host.
- **Logging & Inspection**: Containers can be inspected, and logs will track their activities.

## Getting Started

These instructions will guide you in setting up Tangue for development and testing.

### Prerequisites

Make sure you have the following installed on your system:

- A Linux-based OS (preferably based on Debian)
- C++ Compiler (e.g., `g++`)
- CMake (for building the project)
- Boost Libraries (for system utilities)

### Installation

1. Clone the repository:
    ```bash
    git clone https://github.com/clement-antoine-xavier/tangue.git
    cd tangue
    ```

2. Install dependencies (on Debian-based systems):
    ```bash
    sudo apt update
    sudo apt install build-essential cmake libboost-all-dev -y
    ```

3. Build the project using CMake:
    ```bash
    mkdir build
    cmake -S . -B build
    cmake --build build
    ```

### Contributing

Feel free to fork the project and contribute by creating pull requests. You can also open issues if you encounter bugs or have suggestions for features.

### License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
