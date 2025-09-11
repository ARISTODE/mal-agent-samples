# String-Encrypted File Utility Sample

## Overview
File utility with XOR string encryption obfuscation
- **Base Program**: Custom file manipulation utility
- **Architecture**: x64
- **Compiler**: MinGW-w64 GCC 14.2.0

## Executable Details
- **Clean Version**: `compiled/fileutil_clean.exe` (~3.2 MB)
- **Obfuscated Version**: `compiled/fileutil_obfuscated.exe` (~3.2 MB)
- **Type**: Windows PE console executable
- **Static linking**: Yes

## Build Information
- **Build Date**: 2025-09-10
- **Build System**: MinGW-w64 with g++
- **Source**: Custom C++ implementation
- **Configuration**: Release build with optimizations (-O2)

## Obfuscation Techniques
### 1. String Encryption (XOR Cipher)
- All string literals encrypted with XOR key 0xAB
- Runtime string decryption using `decryptString()` function
- Encrypted strings stored as byte arrays
- Dynamic memory allocation for decrypted strings

### 2. String Obfuscation Features
- **Encryption Method**: Simple XOR cipher
- **Key**: 0xAB (171 decimal)
- **Storage**: Encrypted as unsigned char arrays
- **Decryption**: Runtime decryption with cleanup

## Key Features
- File copying functionality
- File size checking
- File existence verification
- File information display
- Command-line interface

## Usage Example
```cmd
# Show help
fileutil_obfuscated.exe

# Copy file
fileutil_obfuscated.exe copy source.txt dest.txt

# Check file size
fileutil_obfuscated.exe size myfile.txt

# Check if file exists
fileutil_obfuscated.exe exists myfile.txt

# Show file information
fileutil_obfuscated.exe info myfile.txt
```

## Analysis Characteristics
### Clean Version
- Clear string literals visible in binary
- Obvious function names and messages
- Standard PE structure
- Easy static analysis

### Obfuscated Version
- **String Encryption**: All user-visible strings encrypted
- **Anti-Analysis**: Encrypted strings hinder static analysis
- **Runtime Decryption**: Strings only visible during execution
- **Memory Management**: Dynamic allocation/deallocation of decrypted strings
- **Obfuscation Overhead**: Minimal size increase, slight performance cost

## Code Structure
```
file_utility_clean.cpp      - Original, unobfuscated version
file_utility_obfuscated.cpp - String-encrypted version with:
                              * XOR_KEY definition
                              * decryptString() function
                              * DS() helper function
                              * Encrypted string arrays
                              * Runtime decryption calls
```

## Security Analysis Notes
- Demonstrates basic string obfuscation technique
- XOR encryption is easily reversible with known key
- Dynamic string decryption creates runtime artifacts
- Memory contains plaintext strings during execution
- Static analysis tools may struggle with encrypted strings