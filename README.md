# ZipConversor

A versatile application to convert files and folders into various archive formats. Supports ZIP, 7Z, and TAR.GZ formats with both graphical and command-line interfaces.

## Features

- **Multiple Archive Formats**: Convert to ZIP, 7Z, or TAR.GZ
- **Dual Interface Modes**: 
  - GUI mode with visual file selection and progress tracking
  - CLI mode for command-line operations
- **Directory Support**: Compress entire folders while preserving structure
- **Progress Tracking**: Real-time compression progress and file size information
- **High Compression**: Uses maximum compression levels for better results
- **Cross-Platform**: Works on Linux systems (tested on GNOME/GTK environments)

## Requirements

### System Requirements
- Linux (tested on Debian-based distributions)
- CMake 3.16 or higher
- C++17 compatible compiler (GCC recommended)
- GTK+ 3.0 environment

### Dependencies
- **GTK+ 3.0** (for GUI interface)
- **libzip** (for ZIP format support)
- **p7zip-full** (for 7Z format support)
- **tar** and **gzip** (for TAR.GZ format support)
- **pkg-config** (for build configuration)

## Installation

### 1. Install Dependencies

On Ubuntu/Debian:
```bash
sudo apt-get update
sudo apt-get install build-essential cmake pkg-config
sudo apt-get install libgtkmm-3.0-dev
sudo apt-get install libzip-dev
sudo apt-get install p7zip-full
```

### 2. Build from Source

```bash
# Clone the repository (if using git)
git clone https://github.com/yourusername/ZipConversor.git
cd ZipConversor

# Create build directory
mkdir build && cd build

# Configure and build
cmake ..
make
```

## Usage

### GUI Mode

Simply run the application:
```bash
./ZipConversor
```

1. Click "Selecionar Arquivo" or "Selecionar Pasta" to choose what to compress
2. Select the desired output format from the dropdown (ZIP, 7Z, or TAR.GZ)
3. Click "Converter" to start the compression
4. Monitor progress in the log window

### CLI Mode

Run with the --cli flag:
```bash
./ZipConversor --cli
```

Follow the prompts to:
1. Enter the path to the file or folder
2. Choose the output format:
   - 1 for ZIP
   - 2 for TAR.GZ
   - 3 for 7Z

## Features in Detail

### ZIP Format
- Uses libzip with maximum compression level
- Preserves file and directory structure
- Handles large files efficiently

### 7Z Format
- Utilizes p7zip-full for best compression ratios
- Excellent for text file compression
- Maintains original file permissions

### TAR.GZ Format
- Combines tar archiving with gzip compression
- Great for preserving Linux file attributes
- Common format for Linux systems

## Technologies and Tools Used

- C++ as the main development language
- GTK3 for graphical interface
- libzip for ZIP file manipulation
- p7zip-full for 7Z compression
- CMake as build system
- pkg-config for dependency management
- System libraries for compression (gzip, tar)

## Development Stages

1. **CLI Mode Implementation**
   - Initial development of basic compression functionality
   - Implementation of support for different formats (ZIP, 7Z, TAR.GZ)
   - Testing with different file types and sizes

2. **Graphical Interface Development**
   - GTK3 integration for a user-friendly interface
   - Implementation of file selection via GUI
   - Addition of visual feedback during compression

## Theoretical Concepts Application

- **Data Compression**: Utilization of different compression algorithms for each supported format
- **File Handling**: Binary data reading and writing in C++
- **Object-Oriented Programming**: Code structuring using classes and objects
- **Graphical Interface**: Application of events and widgets concepts for a responsive GUI
- **Memory Management**: Careful handling of resource allocation and deallocation

## Results and Challenges

### Results
- Successful implementation of a cross-platform file converter
- Support for multiple compression formats (ZIP, 7Z, TAR.GZ)
- Functional and intuitive graphical interface
- Preservation of directory structure and file attributes

### Challenges Encountered
- Difficulties with C++ syntax, especially in more advanced aspects of the language
- Memory limitations when processing very large files, as the entire file needs to be loaded into memory
- Initial integration with compression libraries
- Development environment setup with all necessary dependencies

### Common Issues

1. "Command '7z' not found":
   ```bash
   sudo apt-get install p7zip-full
   ```

2. GTK warnings:
   - Make sure you have GTK3 properly installed:
   ```bash
   sudo apt-get install libgtkmm-3.0-dev
   ```

3. Build fails with missing libraries:
   - Install all required dependencies as listed above
   - Make sure pkg-config is installed and can find the libraries
