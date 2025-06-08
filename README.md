# ZipConversor

A simple and application to convert any file into a ZIP archive. ZipConversor has both a graphical user interface and a command-line mode.

## Features

- **Dual Modes**: Choose between GUI and CLI modes
- **File Browser**: Easy file selection through a graphical file app you have
- **Linux platform**: Works on Linux (with GNOME desktop environment)

*GUI Mode:*
- File selection
- Progress log

## Requirements

### System Requirements
- Linux (tested on GNOME running Linux Debian)
- CMake
- C++ compiler (GCC recommended)

### Dependencies
- **GTK+ 3.0** 
- **libzip** or **minizip** library
- **pkg-config**

## Installation

### 1. Install Dependencies

On Ubuntu/Debian:
```bash
sudo apt-get update
sudo apt-get install build-essential cmake pkg-config
sudo apt-get install libgtkmm-3.0-dev
sudo apt-get install libzip-dev
```

### 2. Clone and Build
```bash
git clone <repository-url>
cd ZipConversor
mkdir build && cd build
cmake ..
make
```

### 3. Run the Application
```bash
# GUI Mode
./ZipConversor

# CLI Mode
./ZipConversor --cli
```

## Usage
### GUI Mode
1. **Launch the application**:

```bash
./ZipConversor
```
2. **Select a file**:
    - Click "Selecionar Arquivo" button
    - Browse through your files
    - Select the file you want to convert to ZIP

3. **Convert**:
    - Click "Converter para ZIP" button
    - Progress in the progress bar
    - Check the log window for more information

4. **Results**:
    - The ZIP file will be created in the same directory as the original file
    - The ZIP name will be the original name without extension + ".zip"


## How It Works
1. **File Selection**: Users can select any file through the GUI file picker or CLI input
2. **Memory Loading**: The entire file is loaded into memory for processing
3. **ZIP Creation**: A new ZIP archive is created using the minizip/libzip library
4. **Output**: The ZIP file is saved in the same directory as the original file

## Limitations
- **Memory Usage**: Files are loaded entirely into memory, so very large files may cause errors
- **File**: Currently only supports just file compression 
- **Platform**: Primarily tested on Linux Debian with GNOME
