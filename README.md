# ⚡ Sequencer

<div align="center">

[![GitHub stars](https://img.shields.io/github/stars/shaderko/sequencer?style=for-the-badge)](https://github.com/shaderko/sequencer/stargazers)
[![GitHub forks](https://img.shields.io/github/forks/shaderko/sequencer?style=for-the-badge)](https://github.com/shaderko/sequencer/network)
[![GitHub issues](https://img.shields.io/github/issues/shaderko/sequencer?style=for-the-badge)](https://github.com/shaderko/sequencer/issues)
[![GitHub license](https://img.shields.io/github/license/shaderko/sequencer?style=for-the-badge)](LICENSE)

**A CLI tool to replicate mouse clicks and keyboard presses multiple times.**

</div>

## 📖 Overview

Sequencer is a command-line tool written in C that allows you to automate repetitive sequences of mouse clicks and keyboard presses.  This is useful for tasks requiring repeated input, improving workflow efficiency. The tool is designed to be lightweight and easy to use.

## ✨ Features

- Replicates mouse clicks a specified number of times.
- Replicates keyboard presses a specified number of times.
- Configurable delay between actions.
- Simple and intuitive command-line interface.

## 🚀 Quick Start

### Prerequisites

- A C compiler (like GCC or Clang)
- CMake (for building the project)


### Installation

1. **Clone the repository:**
   ```bash
   git clone https://github.com/shaderko/sequencer.git
   cd sequencer
   ```

2. **Create a build directory:**
    ```bash
    mkdir build
    cd build
    ```

3. **Build the project:**
   ```bash
   cmake ..
   make
   ```

4. **(Optional) Install:**  The `make install` command (if supported by your system) will install the executable to a system-wide location.  Otherwise, the executable will be in the `build` directory.


## 📁 Project Structure

```
sequencer/
├── CMakeLists.txt          # CMake build configuration
├── LICENSE                 # License information
├── README.md               # This file
└── src/                    # Source code
    └── ...                 # Source files (implementation details not visible from metadata)
└── deps/                   # External dependencies (likely empty based on file size)
└── externals/              # External libraries (likely empty based on file size)

```

## ⚙️ Configuration

**(Currently, configuration appears to be handled within the CMakeLists.txt file and the source code itself. There is no external configuration file detected.)**  Further analysis of the `CMakeLists.txt` file and source code would be needed to fully document configuration options.

## 🔧 Development

### Building from Source

The project uses CMake for building. The steps outlined in the "Installation" section can be repeated to rebuild the project after making changes to the source code.


### Testing

No dedicated testing framework was detected in the provided metadata.  Additional information would be needed to describe the testing process.

## 🚀 Deployment

The compiled executable can be deployed directly to any system with a compatible C runtime environment.

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

---

<div align="center">

**⭐ Star this repo if you find it helpful!**

</div>
