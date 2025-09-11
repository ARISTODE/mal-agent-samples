#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <cstring>

void printUsage() {
    std::cout << "File Utility v1.0\n";
    std::cout << "Usage: fileutil [command] [arguments]\n";
    std::cout << "Commands:\n";
    std::cout << "  copy <source> <dest>    - Copy file\n";
    std::cout << "  size <file>             - Show file size\n";
    std::cout << "  exists <file>           - Check if file exists\n";
    std::cout << "  info <file>             - Show file information\n";
}

bool copyFile(const std::string& source, const std::string& dest) {
    std::ifstream src(source, std::ios::binary);
    if (!src.is_open()) {
        std::cout << "Error: Cannot open source file: " << source << std::endl;
        return false;
    }
    
    std::ofstream dst(dest, std::ios::binary);
    if (!dst.is_open()) {
        std::cout << "Error: Cannot create destination file: " << dest << std::endl;
        return false;
    }
    
    dst << src.rdbuf();
    std::cout << "File copied successfully: " << source << " -> " << dest << std::endl;
    return true;
}

void showFileSize(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        std::cout << "Error: Cannot open file: " << filename << std::endl;
        return;
    }
    
    auto size = file.tellg();
    std::cout << "File size: " << size << " bytes" << std::endl;
}

void checkFileExists(const std::string& filename) {
    if (std::filesystem::exists(filename)) {
        std::cout << "File exists: " << filename << std::endl;
    } else {
        std::cout << "File does not exist: " << filename << std::endl;
    }
}

void showFileInfo(const std::string& filename) {
    if (!std::filesystem::exists(filename)) {
        std::cout << "Error: File does not exist: " << filename << std::endl;
        return;
    }
    
    auto fileSize = std::filesystem::file_size(filename);
    auto lastWrite = std::filesystem::last_write_time(filename);
    
    std::cout << "File Information:" << std::endl;
    std::cout << "  Path: " << filename << std::endl;
    std::cout << "  Size: " << fileSize << " bytes" << std::endl;
    std::cout << "  Type: " << (std::filesystem::is_regular_file(filename) ? "Regular file" : "Special file") << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printUsage();
        return 1;
    }
    
    std::string command = argv[1];
    
    if (command == "copy" && argc == 4) {
        return copyFile(argv[2], argv[3]) ? 0 : 1;
    } else if (command == "size" && argc == 3) {
        showFileSize(argv[2]);
        return 0;
    } else if (command == "exists" && argc == 3) {
        checkFileExists(argv[2]);
        return 0;
    } else if (command == "info" && argc == 3) {
        showFileInfo(argv[2]);
        return 0;
    } else {
        std::cout << "Error: Invalid command or arguments" << std::endl;
        printUsage();
        return 1;
    }
}