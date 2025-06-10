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

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

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
