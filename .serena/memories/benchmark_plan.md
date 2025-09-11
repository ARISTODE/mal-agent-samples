# LLM Code Evaluation Benchmark Plan

## Project Overview
Creating a benchmark suite for evaluating LLM performance on code analysis tasks. The suite consists of 6 samples:
- 3 benign samples (clean, unobfuscated code)
- 3 obfuscated/encrypted samples

## Directory Structure
```
program-samples/
├── benign/
│   ├── compression_7zip/
│   │   ├── source/         # 7-Zip source code
│   │   ├── build/          # Build artifacts
│   │   └── compiled/       # Final PE executable
│   ├── database_sqlite/
│   │   ├── source/         # SQLite source code
│   │   ├── build/          # Build artifacts
│   │   └── compiled/       # Final PE executable
│   └── network_curl/
│       ├── source/         # cURL source code
│       ├── build/          # Build artifacts
│       └── compiled/       # Final PE executable
└── obfuscated/
    ├── sample_1/           # TBD: Obfuscated compression tool
    ├── sample_2/           # TBD: Encrypted database tool
    └── sample_3/           # TBD: Packed network tool

## Benign Samples Detail

### 1. Compression - 7-Zip
- **Source**: https://www.7-zip.org/a/7z2501-src.7z
- **Version**: 25.01
- **Purpose**: File compression/decompression utility
- **Key Features**: Multiple compression algorithms, archive format support
- **Build**: Visual Studio or MinGW-w64
- **Expected PE Size**: ~1-2 MB

### 2. Database - SQLite
- **Source**: https://www.sqlite.org/download.html
- **Version**: Latest stable (3.47.x)
- **Purpose**: Embedded SQL database engine
- **Key Features**: SQL query processing, file I/O, B-tree operations
- **Build**: Standard C compiler (MSVC or MinGW)
- **Expected PE Size**: ~500 KB - 1 MB

### 3. Network - cURL
- **Source**: https://github.com/curl/curl/tree/rc-8_16_0-1
- **Version**: 8.16.0 RC1
- **Purpose**: Command-line tool for transferring data with URLs
- **Key Features**: HTTP/HTTPS support, SSL/TLS, multiple protocols
- **Build**: CMake with Visual Studio or MinGW
- **Expected PE Size**: ~500 KB - 2 MB

## Build Requirements
- **Windows SDK**: Latest version for Windows PE generation
- **Compilers**: 
  - MSVC (Visual Studio 2019/2022)
  - MinGW-w64 as alternative
- **Build Tools**:
  - CMake (for cURL)
  - Make/nmake
  - Python (for some build scripts)

## Evaluation Metrics
1. **Code Complexity Analysis**: Function count, cyclomatic complexity
2. **Import Analysis**: DLL dependencies, API usage patterns
3. **String Analysis**: Embedded strings, resource strings
4. **Control Flow**: Basic block count, branch density
5. **Size Metrics**: Code section size, data section size, overall PE size

## Implementation Steps

### Phase 1: Benign Samples
1. Create directory structure
2. Download source code for each tool
3. Set up build environments
4. Compile each tool to PE format
5. Verify functionality of compiled PEs
6. Document build process and configurations

### Phase 2: Obfuscated Samples (To Be Planned)
- Consider various obfuscation techniques:
  - Code packing (UPX, custom packers)
  - String encryption
  - Control flow obfuscation
  - API obfuscation
  - Anti-debugging techniques

## Notes
- All samples should be functional Windows PE executables
- Source code should be preserved for reference
- Build configurations should be documented
- Consider both 32-bit and 64-bit builds if relevant