#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <cstring>
#include <memory>

#define XOR_KEY 0xAB

char* decryptString(const unsigned char* encrypted, size_t length) {
    char* decrypted = new char[length + 1];
    for (size_t i = 0; i < length; i++) {
        decrypted[i] = encrypted[i] ^ XOR_KEY;
    }
    decrypted[length] = '\0';
    return decrypted;
}

std::string DS(const unsigned char* encrypted, size_t length) {
    std::unique_ptr<char[]> temp(decryptString(encrypted, length));
    return std::string(temp.get());
}

const unsigned char str1[] = {0x46^XOR_KEY, 0x69^XOR_KEY, 0x6C^XOR_KEY, 0x65^XOR_KEY, 0x20^XOR_KEY, 0x55^XOR_KEY, 0x74^XOR_KEY, 0x69^XOR_KEY, 0x6C^XOR_KEY, 0x69^XOR_KEY, 0x74^XOR_KEY, 0x79^XOR_KEY, 0x20^XOR_KEY, 0x76^XOR_KEY, 0x31^XOR_KEY, 0x2E^XOR_KEY, 0x30^XOR_KEY};
const unsigned char str2[] = {0x55^XOR_KEY, 0x73^XOR_KEY, 0x61^XOR_KEY, 0x67^XOR_KEY, 0x65^XOR_KEY, 0x3A^XOR_KEY, 0x20^XOR_KEY, 0x66^XOR_KEY, 0x69^XOR_KEY, 0x6C^XOR_KEY, 0x65^XOR_KEY, 0x75^XOR_KEY, 0x74^XOR_KEY, 0x69^XOR_KEY, 0x6C^XOR_KEY, 0x20^XOR_KEY, 0x5B^XOR_KEY, 0x63^XOR_KEY, 0x6F^XOR_KEY, 0x6D^XOR_KEY, 0x6D^XOR_KEY, 0x61^XOR_KEY, 0x6E^XOR_KEY, 0x64^XOR_KEY, 0x5D^XOR_KEY, 0x20^XOR_KEY, 0x5B^XOR_KEY, 0x61^XOR_KEY, 0x72^XOR_KEY, 0x67^XOR_KEY, 0x75^XOR_KEY, 0x6D^XOR_KEY, 0x65^XOR_KEY, 0x6E^XOR_KEY, 0x74^XOR_KEY, 0x73^XOR_KEY, 0x5D^XOR_KEY};
const unsigned char str3[] = {0x43^XOR_KEY, 0x6F^XOR_KEY, 0x6D^XOR_KEY, 0x6D^XOR_KEY, 0x61^XOR_KEY, 0x6E^XOR_KEY, 0x64^XOR_KEY, 0x73^XOR_KEY, 0x3A^XOR_KEY};
const unsigned char str4[] = {0x20^XOR_KEY, 0x20^XOR_KEY, 0x63^XOR_KEY, 0x6F^XOR_KEY, 0x70^XOR_KEY, 0x79^XOR_KEY, 0x20^XOR_KEY, 0x3C^XOR_KEY, 0x73^XOR_KEY, 0x6F^XOR_KEY, 0x75^XOR_KEY, 0x72^XOR_KEY, 0x63^XOR_KEY, 0x65^XOR_KEY, 0x3E^XOR_KEY, 0x20^XOR_KEY, 0x3C^XOR_KEY, 0x64^XOR_KEY, 0x65^XOR_KEY, 0x73^XOR_KEY, 0x74^XOR_KEY, 0x3E^XOR_KEY, 0x20^XOR_KEY, 0x20^XOR_KEY, 0x20^XOR_KEY, 0x20^XOR_KEY, 0x2D^XOR_KEY, 0x20^XOR_KEY, 0x43^XOR_KEY, 0x6F^XOR_KEY, 0x70^XOR_KEY, 0x79^XOR_KEY, 0x20^XOR_KEY, 0x66^XOR_KEY, 0x69^XOR_KEY, 0x6C^XOR_KEY, 0x65^XOR_KEY};
const unsigned char str5[] = {0x20^XOR_KEY, 0x20^XOR_KEY, 0x73^XOR_KEY, 0x69^XOR_KEY, 0x7A^XOR_KEY, 0x65^XOR_KEY, 0x20^XOR_KEY, 0x3C^XOR_KEY, 0x66^XOR_KEY, 0x69^XOR_KEY, 0x6C^XOR_KEY, 0x65^XOR_KEY, 0x3E^XOR_KEY, 0x20^XOR_KEY, 0x20^XOR_KEY, 0x20^XOR_KEY, 0x20^XOR_KEY, 0x20^XOR_KEY, 0x20^XOR_KEY, 0x20^XOR_KEY, 0x20^XOR_KEY, 0x20^XOR_KEY, 0x20^XOR_KEY, 0x20^XOR_KEY, 0x20^XOR_KEY, 0x2D^XOR_KEY, 0x20^XOR_KEY, 0x53^XOR_KEY, 0x68^XOR_KEY, 0x6F^XOR_KEY, 0x77^XOR_KEY, 0x20^XOR_KEY, 0x66^XOR_KEY, 0x69^XOR_KEY, 0x6C^XOR_KEY, 0x65^XOR_KEY, 0x20^XOR_KEY, 0x73^XOR_KEY, 0x69^XOR_KEY, 0x7A^XOR_KEY, 0x65^XOR_KEY};
const unsigned char str6[] = {0x20^XOR_KEY, 0x20^XOR_KEY, 0x65^XOR_KEY, 0x78^XOR_KEY, 0x69^XOR_KEY, 0x73^XOR_KEY, 0x74^XOR_KEY, 0x73^XOR_KEY, 0x20^XOR_KEY, 0x3C^XOR_KEY, 0x66^XOR_KEY, 0x69^XOR_KEY, 0x6C^XOR_KEY, 0x65^XOR_KEY, 0x3E^XOR_KEY, 0x20^XOR_KEY, 0x20^XOR_KEY, 0x20^XOR_KEY, 0x20^XOR_KEY, 0x20^XOR_KEY, 0x20^XOR_KEY, 0x20^XOR_KEY, 0x20^XOR_KEY, 0x20^XOR_KEY, 0x20^XOR_KEY, 0x20^XOR_KEY, 0x2D^XOR_KEY, 0x20^XOR_KEY, 0x43^XOR_KEY, 0x68^XOR_KEY, 0x65^XOR_KEY, 0x63^XOR_KEY, 0x6B^XOR_KEY, 0x20^XOR_KEY, 0x69^XOR_KEY, 0x66^XOR_KEY, 0x20^XOR_KEY, 0x66^XOR_KEY, 0x69^XOR_KEY, 0x6C^XOR_KEY, 0x65^XOR_KEY, 0x20^XOR_KEY, 0x65^XOR_KEY, 0x78^XOR_KEY, 0x69^XOR_KEY, 0x73^XOR_KEY, 0x74^XOR_KEY, 0x73^XOR_KEY};
const unsigned char str7[] = {0x20^XOR_KEY, 0x20^XOR_KEY, 0x69^XOR_KEY, 0x6E^XOR_KEY, 0x66^XOR_KEY, 0x6F^XOR_KEY, 0x20^XOR_KEY, 0x3C^XOR_KEY, 0x66^XOR_KEY, 0x69^XOR_KEY, 0x6C^XOR_KEY, 0x65^XOR_KEY, 0x3E^XOR_KEY, 0x20^XOR_KEY, 0x20^XOR_KEY, 0x20^XOR_KEY, 0x20^XOR_KEY, 0x20^XOR_KEY, 0x20^XOR_KEY, 0x20^XOR_KEY, 0x20^XOR_KEY, 0x20^XOR_KEY, 0x20^XOR_KEY, 0x20^XOR_KEY, 0x20^XOR_KEY, 0x20^XOR_KEY, 0x2D^XOR_KEY, 0x20^XOR_KEY, 0x53^XOR_KEY, 0x68^XOR_KEY, 0x6F^XOR_KEY, 0x77^XOR_KEY, 0x20^XOR_KEY, 0x66^XOR_KEY, 0x69^XOR_KEY, 0x6C^XOR_KEY, 0x65^XOR_KEY, 0x20^XOR_KEY, 0x69^XOR_KEY, 0x6E^XOR_KEY, 0x66^XOR_KEY, 0x6F^XOR_KEY, 0x72^XOR_KEY, 0x6D^XOR_KEY, 0x61^XOR_KEY, 0x74^XOR_KEY, 0x69^XOR_KEY, 0x6F^XOR_KEY, 0x6E^XOR_KEY};

const unsigned char errStr1[] = {0x85^XOR_KEY, 0x72^XOR_KEY, 0x72^XOR_KEY, 0x6F^XOR_KEY, 0x72^XOR_KEY, 0x3A^XOR_KEY, 0x20^XOR_KEY, 0x83^XOR_KEY, 0x61^XOR_KEY, 0x6E^XOR_KEY, 0x6E^XOR_KEY, 0x6F^XOR_KEY, 0x74^XOR_KEY, 0x20^XOR_KEY, 0x6F^XOR_KEY, 0x70^XOR_KEY, 0x65^XOR_KEY, 0x6E^XOR_KEY, 0x20^XOR_KEY, 0x73^XOR_KEY, 0x6F^XOR_KEY, 0x75^XOR_KEY, 0x72^XOR_KEY, 0x63^XOR_KEY, 0x65^XOR_KEY, 0x20^XOR_KEY, 0x66^XOR_KEY, 0x69^XOR_KEY, 0x6C^XOR_KEY, 0x65^XOR_KEY, 0x3A^XOR_KEY, 0x20^XOR_KEY};
const unsigned char errStr2[] = {0x85^XOR_KEY, 0x72^XOR_KEY, 0x72^XOR_KEY, 0x6F^XOR_KEY, 0x72^XOR_KEY, 0x3A^XOR_KEY, 0x20^XOR_KEY, 0x83^XOR_KEY, 0x61^XOR_KEY, 0x6E^XOR_KEY, 0x6E^XOR_KEY, 0x6F^XOR_KEY, 0x74^XOR_KEY, 0x20^XOR_KEY, 0x63^XOR_KEY, 0x72^XOR_KEY, 0x65^XOR_KEY, 0x61^XOR_KEY, 0x74^XOR_KEY, 0x65^XOR_KEY, 0x20^XOR_KEY, 0x64^XOR_KEY, 0x65^XOR_KEY, 0x73^XOR_KEY, 0x74^XOR_KEY, 0x69^XOR_KEY, 0x6E^XOR_KEY, 0x61^XOR_KEY, 0x74^XOR_KEY, 0x69^XOR_KEY, 0x6F^XOR_KEY, 0x6E^XOR_KEY, 0x20^XOR_KEY, 0x66^XOR_KEY, 0x69^XOR_KEY, 0x6C^XOR_KEY, 0x65^XOR_KEY, 0x3A^XOR_KEY, 0x20^XOR_KEY};

const unsigned char successStr[] = {0x86^XOR_KEY, 0x69^XOR_KEY, 0x6C^XOR_KEY, 0x65^XOR_KEY, 0x20^XOR_KEY, 0x63^XOR_KEY, 0x6F^XOR_KEY, 0x70^XOR_KEY, 0x69^XOR_KEY, 0x65^XOR_KEY, 0x64^XOR_KEY, 0x20^XOR_KEY, 0x73^XOR_KEY, 0x75^XOR_KEY, 0x63^XOR_KEY, 0x63^XOR_KEY, 0x65^XOR_KEY, 0x73^XOR_KEY, 0x73^XOR_KEY, 0x66^XOR_KEY, 0x75^XOR_KEY, 0x6C^XOR_KEY, 0x6C^XOR_KEY, 0x79^XOR_KEY, 0x3A^XOR_KEY, 0x20^XOR_KEY};
const unsigned char sizeStr[] = {0x86^XOR_KEY, 0x69^XOR_KEY, 0x6C^XOR_KEY, 0x65^XOR_KEY, 0x20^XOR_KEY, 0x73^XOR_KEY, 0x69^XOR_KEY, 0x7A^XOR_KEY, 0x65^XOR_KEY, 0x3A^XOR_KEY, 0x20^XOR_KEY};
const unsigned char bytesStr[] = {0x20^XOR_KEY, 0x62^XOR_KEY, 0x79^XOR_KEY, 0x74^XOR_KEY, 0x65^XOR_KEY, 0x73^XOR_KEY};

const unsigned char existsStr[] = {0x86^XOR_KEY, 0x69^XOR_KEY, 0x6C^XOR_KEY, 0x65^XOR_KEY, 0x20^XOR_KEY, 0x65^XOR_KEY, 0x78^XOR_KEY, 0x69^XOR_KEY, 0x73^XOR_KEY, 0x74^XOR_KEY, 0x73^XOR_KEY, 0x3A^XOR_KEY, 0x20^XOR_KEY};
const unsigned char notExistsStr[] = {0x86^XOR_KEY, 0x69^XOR_KEY, 0x6C^XOR_KEY, 0x65^XOR_KEY, 0x20^XOR_KEY, 0x64^XOR_KEY, 0x6F^XOR_KEY, 0x65^XOR_KEY, 0x73^XOR_KEY, 0x20^XOR_KEY, 0x6E^XOR_KEY, 0x6F^XOR_KEY, 0x74^XOR_KEY, 0x20^XOR_KEY, 0x65^XOR_KEY, 0x78^XOR_KEY, 0x69^XOR_KEY, 0x73^XOR_KEY, 0x74^XOR_KEY, 0x3A^XOR_KEY, 0x20^XOR_KEY};

const unsigned char infoStr[] = {0x86^XOR_KEY, 0x69^XOR_KEY, 0x6C^XOR_KEY, 0x65^XOR_KEY, 0x20^XOR_KEY, 0x89^XOR_KEY, 0x6E^XOR_KEY, 0x66^XOR_KEY, 0x6F^XOR_KEY, 0x72^XOR_KEY, 0x6D^XOR_KEY, 0x61^XOR_KEY, 0x74^XOR_KEY, 0x69^XOR_KEY, 0x6F^XOR_KEY, 0x6E^XOR_KEY, 0x3A^XOR_KEY};
const unsigned char pathStr[] = {0x20^XOR_KEY, 0x20^XOR_KEY, 0x90^XOR_KEY, 0x61^XOR_KEY, 0x74^XOR_KEY, 0x68^XOR_KEY, 0x3A^XOR_KEY, 0x20^XOR_KEY};
const unsigned char typeStr[] = {0x20^XOR_KEY, 0x20^XOR_KEY, 0x94^XOR_KEY, 0x79^XOR_KEY, 0x70^XOR_KEY, 0x65^XOR_KEY, 0x3A^XOR_KEY, 0x20^XOR_KEY};
const unsigned char regularFileStr[] = {0x92^XOR_KEY, 0x65^XOR_KEY, 0x67^XOR_KEY, 0x75^XOR_KEY, 0x6C^XOR_KEY, 0x61^XOR_KEY, 0x72^XOR_KEY, 0x20^XOR_KEY, 0x66^XOR_KEY, 0x69^XOR_KEY, 0x6C^XOR_KEY, 0x65^XOR_KEY};
const unsigned char specialFileStr[] = {0x93^XOR_KEY, 0x70^XOR_KEY, 0x65^XOR_KEY, 0x63^XOR_KEY, 0x69^XOR_KEY, 0x61^XOR_KEY, 0x6C^XOR_KEY, 0x20^XOR_KEY, 0x66^XOR_KEY, 0x69^XOR_KEY, 0x6C^XOR_KEY, 0x65^XOR_KEY};

const unsigned char copyCmd[] = {0x63^XOR_KEY, 0x6F^XOR_KEY, 0x70^XOR_KEY, 0x79^XOR_KEY};
const unsigned char sizeCmd[] = {0x73^XOR_KEY, 0x69^XOR_KEY, 0x7A^XOR_KEY, 0x65^XOR_KEY};
const unsigned char existsCmd[] = {0x65^XOR_KEY, 0x78^XOR_KEY, 0x69^XOR_KEY, 0x73^XOR_KEY, 0x74^XOR_KEY, 0x73^XOR_KEY};
const unsigned char infoCmd[] = {0x69^XOR_KEY, 0x6E^XOR_KEY, 0x66^XOR_KEY, 0x6F^XOR_KEY};

const unsigned char invalidCmd[] = {0x85^XOR_KEY, 0x72^XOR_KEY, 0x72^XOR_KEY, 0x6F^XOR_KEY, 0x72^XOR_KEY, 0x3A^XOR_KEY, 0x20^XOR_KEY, 0x89^XOR_KEY, 0x6E^XOR_KEY, 0x76^XOR_KEY, 0x61^XOR_KEY, 0x6C^XOR_KEY, 0x69^XOR_KEY, 0x64^XOR_KEY, 0x20^XOR_KEY, 0x63^XOR_KEY, 0x6F^XOR_KEY, 0x6D^XOR_KEY, 0x6D^XOR_KEY, 0x61^XOR_KEY, 0x6E^XOR_KEY, 0x64^XOR_KEY, 0x20^XOR_KEY, 0x6F^XOR_KEY, 0x72^XOR_KEY, 0x20^XOR_KEY, 0x61^XOR_KEY, 0x72^XOR_KEY, 0x67^XOR_KEY, 0x75^XOR_KEY, 0x6D^XOR_KEY, 0x65^XOR_KEY, 0x6E^XOR_KEY, 0x74^XOR_KEY, 0x73^XOR_KEY};

void printUsage() {
    std::cout << DS(str1, sizeof(str1)) << std::endl;
    std::cout << DS(str2, sizeof(str2)) << std::endl;
    std::cout << DS(str3, sizeof(str3)) << std::endl;
    std::cout << DS(str4, sizeof(str4)) << std::endl;
    std::cout << DS(str5, sizeof(str5)) << std::endl;
    std::cout << DS(str6, sizeof(str6)) << std::endl;
    std::cout << DS(str7, sizeof(str7)) << std::endl;
}

bool copyFile(const std::string& source, const std::string& dest) {
    std::ifstream src(source, std::ios::binary);
    if (!src.is_open()) {
        std::cout << DS(errStr1, sizeof(errStr1)) << source << std::endl;
        return false;
    }
    
    std::ofstream dst(dest, std::ios::binary);
    if (!dst.is_open()) {
        std::cout << DS(errStr2, sizeof(errStr2)) << dest << std::endl;
        return false;
    }
    
    dst << src.rdbuf();
    std::cout << DS(successStr, sizeof(successStr)) << source << " -> " << dest << std::endl;
    return true;
}

void showFileSize(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        std::cout << DS(errStr1, sizeof(errStr1)) << filename << std::endl;
        return;
    }
    
    auto size = file.tellg();
    std::cout << DS(sizeStr, sizeof(sizeStr)) << size << DS(bytesStr, sizeof(bytesStr)) << std::endl;
}

void checkFileExists(const std::string& filename) {
    if (std::filesystem::exists(filename)) {
        std::cout << DS(existsStr, sizeof(existsStr)) << filename << std::endl;
    } else {
        std::cout << DS(notExistsStr, sizeof(notExistsStr)) << filename << std::endl;
    }
}

void showFileInfo(const std::string& filename) {
    if (!std::filesystem::exists(filename)) {
        std::cout << DS(errStr1, sizeof(errStr1)) << filename << std::endl;
        return;
    }
    
    auto fileSize = std::filesystem::file_size(filename);
    auto lastWrite = std::filesystem::last_write_time(filename);
    
    std::cout << DS(infoStr, sizeof(infoStr)) << std::endl;
    std::cout << DS(pathStr, sizeof(pathStr)) << filename << std::endl;
    std::cout << DS(sizeStr, sizeof(sizeStr)) << fileSize << DS(bytesStr, sizeof(bytesStr)) << std::endl;
    std::cout << DS(typeStr, sizeof(typeStr)) << (std::filesystem::is_regular_file(filename) ? DS(regularFileStr, sizeof(regularFileStr)) : DS(specialFileStr, sizeof(specialFileStr))) << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printUsage();
        return 1;
    }
    
    std::string command = argv[1];
    
    if (command == DS(copyCmd, sizeof(copyCmd)) && argc == 4) {
        return copyFile(argv[2], argv[3]) ? 0 : 1;
    } else if (command == DS(sizeCmd, sizeof(sizeCmd)) && argc == 3) {
        showFileSize(argv[2]);
        return 0;
    } else if (command == DS(existsCmd, sizeof(existsCmd)) && argc == 3) {
        checkFileExists(argv[2]);
        return 0;
    } else if (command == DS(infoCmd, sizeof(infoCmd)) && argc == 3) {
        showFileInfo(argv[2]);
        return 0;
    } else {
        std::cout << DS(invalidCmd, sizeof(invalidCmd)) << std::endl;
        printUsage();
        return 1;
    }
}