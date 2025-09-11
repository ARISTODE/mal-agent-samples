# 7-Zip Console Sample

## Overview
7-Zip console version (7zr.exe) - a standalone compression utility
- **Version**: 25.01
- **Architecture**: x64
- **Compiler**: MinGW-w64 GCC 14.2.0

## Executable Details
- **File**: `compiled/7zr.exe`
- **Size**: ~1.2 MB
- **Type**: Windows PE console executable
- **Static linking**: Yes

## Build Information
- **Build Date**: 2025-09-10
- **Build System**: MinGW-w64 with mingw32-make
- **Source**: Official 7-Zip 25.01 source code
- **Configuration**: Release build with optimizations (-O2)

## Key Features
- Multiple compression algorithms (LZMA, LZMA2, etc.)
- Archive creation and extraction
- Support for 7z format
- Command-line interface
- Multi-threading support

## Usage Example
```cmd
# Create archive
7zr.exe a archive.7z file1.txt file2.txt

# Extract archive
7zr.exe x archive.7z

# List contents
7zr.exe l archive.7z

# Benchmark
7zr.exe b
```

## Analysis Characteristics
- Clean, unobfuscated code
- Standard PE structure
- Regular import table
- No anti-debugging techniques
- Clear string literals
- Standard control flow