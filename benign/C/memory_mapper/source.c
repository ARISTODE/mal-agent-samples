#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/mman.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#endif

#define TEST_DATA_SIZE 4096
#define TEST_FILE_NAME "test_mapping.dat"
#define SHARED_MEMORY_NAME "Local\\TestSharedMemory"

typedef struct {
    void *mapped_addr;
    size_t size;
    char description[100];
#ifdef _WIN32
    HANDLE file_handle;
    HANDLE mapping_handle;
#else
    int file_descriptor;
#endif
} MappingInfo;

void print_usage() {
    printf("Usage: binary.exe [OPTIONS]\n");
    printf("  OPTIONS:\n");
    printf("    --help, -h        Show this help message\n");
    printf("    --test            Run memory mapping tests\n");
    printf("    --file-map        Test file mapping\n");
    printf("    --anonymous       Test anonymous mapping\n");
    printf("    --shared          Test shared memory mapping\n");
    printf("    --performance     Test mapping performance\n");
    printf("\nMemory mapping demonstration and testing\n");
}

#ifdef _WIN32
void *create_file_mapping(const char *filename, size_t size, MappingInfo *info) {
    printf("Creating file mapping for: %s (%zu bytes)\n", filename, size);
    
    info->file_handle = CreateFileA(filename, GENERIC_READ | GENERIC_WRITE,
                                   FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
                                   CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    
    if (info->file_handle == INVALID_HANDLE_VALUE) {
        printf("Error: Cannot create file '%s': %ld\n", filename, GetLastError());
        return NULL;
    }
    
    // Set file size
    LARGE_INTEGER file_size;
    file_size.QuadPart = size;
    if (!SetFilePointerEx(info->file_handle, file_size, NULL, FILE_BEGIN) ||
        !SetEndOfFile(info->file_handle)) {
        printf("Error: Cannot set file size: %ld\n", GetLastError());
        CloseHandle(info->file_handle);
        return NULL;
    }
    
    info->mapping_handle = CreateFileMappingA(info->file_handle, NULL, PAGE_READWRITE,
                                             0, (DWORD)size, NULL);
    
    if (!info->mapping_handle) {
        printf("Error: Cannot create file mapping: %ld\n", GetLastError());
        CloseHandle(info->file_handle);
        return NULL;
    }
    
    void *mapped_addr = MapViewOfFile(info->mapping_handle, FILE_MAP_ALL_ACCESS,
                                     0, 0, size);
    
    if (!mapped_addr) {
        printf("Error: Cannot map view of file: %ld\n", GetLastError());
        CloseHandle(info->mapping_handle);
        CloseHandle(info->file_handle);
        return NULL;
    }
    
    info->mapped_addr = mapped_addr;
    info->size = size;
    snprintf(info->description, sizeof(info->description), "File mapping: %s", filename);
    
    printf("File mapped at address: %p\n", mapped_addr);
    return mapped_addr;
}

void *create_anonymous_mapping(size_t size, MappingInfo *info) {
    printf("Creating anonymous mapping (%zu bytes)\n", size);
    
    info->file_handle = INVALID_HANDLE_VALUE;
    info->mapping_handle = CreateFileMappingA(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE,
                                             0, (DWORD)size, NULL);
    
    if (!info->mapping_handle) {
        printf("Error: Cannot create anonymous mapping: %ld\n", GetLastError());
        return NULL;
    }
    
    void *mapped_addr = MapViewOfFile(info->mapping_handle, FILE_MAP_ALL_ACCESS,
                                     0, 0, size);
    
    if (!mapped_addr) {
        printf("Error: Cannot map anonymous memory: %ld\n", GetLastError());
        CloseHandle(info->mapping_handle);
        return NULL;
    }
    
    info->mapped_addr = mapped_addr;
    info->size = size;
    snprintf(info->description, sizeof(info->description), "Anonymous mapping");
    
    printf("Anonymous memory mapped at address: %p\n", mapped_addr);
    return mapped_addr;
}

void *create_shared_mapping(size_t size, MappingInfo *info) {
    printf("Creating shared memory mapping (%zu bytes)\n", size);
    
    info->file_handle = INVALID_HANDLE_VALUE;
    info->mapping_handle = CreateFileMappingA(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE,
                                             0, (DWORD)size, SHARED_MEMORY_NAME);
    
    if (!info->mapping_handle) {
        printf("Error: Cannot create shared mapping: %ld\n", GetLastError());
        return NULL;
    }
    
    void *mapped_addr = MapViewOfFile(info->mapping_handle, FILE_MAP_ALL_ACCESS,
                                     0, 0, size);
    
    if (!mapped_addr) {
        printf("Error: Cannot map shared memory: %ld\n", GetLastError());
        CloseHandle(info->mapping_handle);
        return NULL;
    }
    
    info->mapped_addr = mapped_addr;
    info->size = size;
    snprintf(info->description, sizeof(info->description), "Shared memory mapping");
    
    printf("Shared memory mapped at address: %p\n", mapped_addr);
    return mapped_addr;
}

void cleanup_mapping(MappingInfo *info) {
    printf("Cleaning up: %s\n", info->description);
    
    if (info->mapped_addr) {
        if (!UnmapViewOfFile(info->mapped_addr)) {
            printf("Warning: Failed to unmap memory: %ld\n", GetLastError());
        }
        info->mapped_addr = NULL;
    }
    
    if (info->mapping_handle) {
        CloseHandle(info->mapping_handle);
        info->mapping_handle = NULL;
    }
    
    if (info->file_handle != INVALID_HANDLE_VALUE) {
        CloseHandle(info->file_handle);
        info->file_handle = INVALID_HANDLE_VALUE;
    }
}

#else
void *create_file_mapping(const char *filename, size_t size, MappingInfo *info) {
    printf("Creating file mapping for: %s (%zu bytes)\n", filename, size);
    
    info->file_descriptor = open(filename, O_CREAT | O_RDWR, 0644);
    if (info->file_descriptor == -1) {
        printf("Error: Cannot create file '%s': %s\n", filename, strerror(errno));
        return NULL;
    }
    
    if (ftruncate(info->file_descriptor, size) == -1) {
        printf("Error: Cannot set file size: %s\n", strerror(errno));
        close(info->file_descriptor);
        return NULL;
    }
    
    void *mapped_addr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED,
                            info->file_descriptor, 0);
    
    if (mapped_addr == MAP_FAILED) {
        printf("Error: Cannot map file: %s\n", strerror(errno));
        close(info->file_descriptor);
        return NULL;
    }
    
    info->mapped_addr = mapped_addr;
    info->size = size;
    snprintf(info->description, sizeof(info->description), "File mapping: %s", filename);
    
    printf("File mapped at address: %p\n", mapped_addr);
    return mapped_addr;
}

void *create_anonymous_mapping(size_t size, MappingInfo *info) {
    printf("Creating anonymous mapping (%zu bytes)\n", size);
    
    void *mapped_addr = mmap(NULL, size, PROT_READ | PROT_WRITE,
                            MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    
    if (mapped_addr == MAP_FAILED) {
        printf("Error: Cannot create anonymous mapping: %s\n", strerror(errno));
        return NULL;
    }
    
    info->mapped_addr = mapped_addr;
    info->size = size;
    info->file_descriptor = -1;
    snprintf(info->description, sizeof(info->description), "Anonymous mapping");
    
    printf("Anonymous memory mapped at address: %p\n", mapped_addr);
    return mapped_addr;
}

void *create_shared_mapping(size_t size, MappingInfo *info) {
    printf("Creating shared memory mapping (%zu bytes)\n", size);
    
    info->file_descriptor = shm_open("/test_shared_mem", O_CREAT | O_RDWR, 0644);
    if (info->file_descriptor == -1) {
        printf("Error: Cannot create shared memory: %s\n", strerror(errno));
        return NULL;
    }
    
    if (ftruncate(info->file_descriptor, size) == -1) {
        printf("Error: Cannot set shared memory size: %s\n", strerror(errno));
        close(info->file_descriptor);
        return NULL;
    }
    
    void *mapped_addr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED,
                            info->file_descriptor, 0);
    
    if (mapped_addr == MAP_FAILED) {
        printf("Error: Cannot map shared memory: %s\n", strerror(errno));
        close(info->file_descriptor);
        return NULL;
    }
    
    info->mapped_addr = mapped_addr;
    info->size = size;
    snprintf(info->description, sizeof(info->description), "Shared memory mapping");
    
    printf("Shared memory mapped at address: %p\n", mapped_addr);
    return mapped_addr;
}

void cleanup_mapping(MappingInfo *info) {
    printf("Cleaning up: %s\n", info->description);
    
    if (info->mapped_addr && info->mapped_addr != MAP_FAILED) {
        if (munmap(info->mapped_addr, info->size) == -1) {
            printf("Warning: Failed to unmap memory: %s\n", strerror(errno));
        }
        info->mapped_addr = NULL;
    }
    
    if (info->file_descriptor != -1) {
        close(info->file_descriptor);
        info->file_descriptor = -1;
    }
}
#endif

void test_memory_operations(void *addr, size_t size) {
    printf("\n--- Testing memory operations ---\n");
    char *mem = (char*)addr;
    
    printf("Writing test pattern to mapped memory...\n");
    for (size_t i = 0; i < size && i < 1000; i++) {
        mem[i] = (char)(i % 256);
    }
    
    printf("Reading back test pattern...\n");
    int errors = 0;
    for (size_t i = 0; i < size && i < 1000; i++) {
        if (mem[i] != (char)(i % 256)) {
            errors++;
            if (errors == 1) {
                printf("Error at offset %zu: expected %d, got %d\n", 
                       i, (int)(i % 256), (int)mem[i]);
            }
        }
    }
    
    if (errors == 0) {
        printf("Memory test PASSED: All data matches\n");
    } else {
        printf("Memory test FAILED: %d errors found\n", errors);
    }
    
    printf("Writing string data to beginning of mapping...\n");
    const char *test_string = "Hello, Memory Mapping World!";
    strcpy(mem, test_string);
    
    printf("Reading back string: '%s'\n", mem);
    if (strcmp(mem, test_string) == 0) {
        printf("String test PASSED\n");
    } else {
        printf("String test FAILED\n");
    }
}

void run_basic_tests() {
    printf("Memory Mapping Test Suite\n");
    printf("========================\n\n");
    
    MappingInfo info;
    
    printf("--- Test 1: File Mapping ---\n");
    void *file_addr = create_file_mapping(TEST_FILE_NAME, TEST_DATA_SIZE, &info);
    if (file_addr) {
        test_memory_operations(file_addr, TEST_DATA_SIZE);
        cleanup_mapping(&info);
    }
    
    printf("\n--- Test 2: Anonymous Mapping ---\n");
    memset(&info, 0, sizeof(info));
    void *anon_addr = create_anonymous_mapping(TEST_DATA_SIZE, &info);
    if (anon_addr) {
        test_memory_operations(anon_addr, TEST_DATA_SIZE);
        cleanup_mapping(&info);
    }
    
    printf("\n--- Test 3: Shared Memory Mapping ---\n");
    memset(&info, 0, sizeof(info));
    void *shared_addr = create_shared_mapping(TEST_DATA_SIZE, &info);
    if (shared_addr) {
        test_memory_operations(shared_addr, TEST_DATA_SIZE);
        cleanup_mapping(&info);
    }
    
    printf("\nCleaning up test files...\n");
    remove(TEST_FILE_NAME);
    
    printf("\nAll memory mapping tests completed!\n");
}

void run_file_mapping_test() {
    printf("File Mapping Detailed Test\n");
    printf("==========================\n\n");
    
    MappingInfo info;
    void *addr = create_file_mapping(TEST_FILE_NAME, TEST_DATA_SIZE * 2, &info);
    if (!addr) return;
    
    char *mem = (char*)addr;
    
    printf("Writing structured data to file mapping...\n");
    struct {
        int magic_number;
        char version[16];
        int data_count;
        int checksum;
    } header = {0x12345678, "v1.0", 1000, 0};
    
    memcpy(mem, &header, sizeof(header));
    
    printf("Writing data array...\n");
    int *data_array = (int*)(mem + sizeof(header));
    for (int i = 0; i < header.data_count && (char*)&data_array[i] < mem + TEST_DATA_SIZE * 2; i++) {
        data_array[i] = i * i;  // Square numbers
        header.checksum += data_array[i];
    }
    
    memcpy(mem, &header, sizeof(header));
    
    printf("Verifying structured data...\n");
    struct {
        int magic_number;
        char version[16];
        int data_count;
        int checksum;
    } *read_header = (void*)mem;
    
    printf("Magic number: 0x%08x\n", read_header->magic_number);
    printf("Version: %s\n", read_header->version);
    printf("Data count: %d\n", read_header->data_count);
    printf("Checksum: %d\n", read_header->checksum);
    
    int *read_data = (int*)(mem + sizeof(header));
    printf("First 10 data values: ");
    for (int i = 0; i < 10; i++) {
        printf("%d ", read_data[i]);
    }
    printf("\n");
    
    cleanup_mapping(&info);
    remove(TEST_FILE_NAME);
}

void run_performance_test() {
    printf("Memory Mapping Performance Test\n");
    printf("===============================\n\n");
    
    const size_t test_sizes[] = {4096, 65536, 1048576, 4194304}; // 4KB, 64KB, 1MB, 4MB
    const int num_sizes = sizeof(test_sizes) / sizeof(test_sizes[0]);
    
    for (int i = 0; i < num_sizes; i++) {
        size_t size = test_sizes[i];
        printf("--- Performance test with %zu bytes (%.2f KB) ---\n", 
               size, size / 1024.0);
        
        MappingInfo info;
        
        clock_t start = clock();
        void *addr = create_anonymous_mapping(size, &info);
        clock_t map_time = clock();
        
        if (!addr) {
            printf("Mapping failed for size %zu\n", size);
            continue;
        }
        
        char *mem = (char*)addr;
        for (size_t j = 0; j < size; j += 4096) {
            mem[j] = (char)(j % 256);  // Touch each page
        }
        clock_t write_time = clock();
        
        volatile char sum = 0;
        for (size_t j = 0; j < size; j += 4096) {
            sum += mem[j];  // Read each page
        }
        clock_t read_time = clock();
        
        cleanup_mapping(&info);
        clock_t cleanup_time = clock();
        
        double map_ms = ((double)(map_time - start)) / CLOCKS_PER_SEC * 1000;
        double write_ms = ((double)(write_time - map_time)) / CLOCKS_PER_SEC * 1000;
        double read_ms = ((double)(read_time - write_time)) / CLOCKS_PER_SEC * 1000;
        double cleanup_ms = ((double)(cleanup_time - read_time)) / CLOCKS_PER_SEC * 1000;
        
        printf("  Mapping time:  %.2f ms\n", map_ms);
        printf("  Write time:    %.2f ms\n", write_ms);
        printf("  Read time:     %.2f ms\n", read_ms);
        printf("  Cleanup time:  %.2f ms\n", cleanup_ms);
        printf("  Total time:    %.2f ms\n", map_ms + write_ms + read_ms + cleanup_ms);
        printf("  Checksum: %d (prevents optimization)\n\n", (int)sum);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Memory Mapper Demo\n");
        printf("==================\n\n");
        print_usage();
        return 1;
    }
    
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            print_usage();
            return 0;
        } else if (strcmp(argv[i], "--test") == 0) {
            run_basic_tests();
            return 0;
        } else if (strcmp(argv[i], "--file-map") == 0) {
            run_file_mapping_test();
            return 0;
        } else if (strcmp(argv[i], "--anonymous") == 0) {
            MappingInfo info;
            void *addr = create_anonymous_mapping(TEST_DATA_SIZE, &info);
            if (addr) {
                test_memory_operations(addr, TEST_DATA_SIZE);
                cleanup_mapping(&info);
            }
            return 0;
        } else if (strcmp(argv[i], "--shared") == 0) {
            MappingInfo info;
            void *addr = create_shared_mapping(TEST_DATA_SIZE, &info);
            if (addr) {
                test_memory_operations(addr, TEST_DATA_SIZE);
                cleanup_mapping(&info);
            }
            return 0;
        } else if (strcmp(argv[i], "--performance") == 0) {
            run_performance_test();
            return 0;
        } else {
            printf("Unknown option: %s\n", argv[i]);
            print_usage();
            return 1;
        }
    }
    
    return 0;
}