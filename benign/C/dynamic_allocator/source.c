#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/resource.h>
#endif

#define MAX_ALLOCATIONS 1000
#define MIN_BLOCK_SIZE 64
#define MAX_BLOCK_SIZE 1048576 // 1MB

typedef struct {
    void *ptr;
    size_t size;
    time_t alloc_time;
    int id;
    int freed;
} AllocationRecord;

typedef struct {
    AllocationRecord records[MAX_ALLOCATIONS];
    int count;
    size_t total_allocated;
    size_t peak_memory;
    size_t current_memory;
    int allocation_id_counter;
} MemoryTracker;

static MemoryTracker tracker = {0};

void print_usage() {
    printf("Usage: binary.exe [OPTIONS]\n");
    printf("  OPTIONS:\n");
    printf("    --help, -h       Show this help message\n");
    printf("    --test           Run memory allocation tests\n");
    printf("    --stress N       Run stress test with N allocations\n");
    printf("    --pattern        Test allocation patterns\n");
    printf("    --fragmentation  Test memory fragmentation\n");
    printf("    --leak-test      Test memory leak detection\n");
    printf("\nMemory allocation testing and demonstration\n");
}

void update_memory_stats(size_t size, int is_allocation) {
    if (is_allocation) {
        tracker.current_memory += size;
        tracker.total_allocated += size;
        if (tracker.current_memory > tracker.peak_memory) {
            tracker.peak_memory = tracker.current_memory;
        }
    } else {
        if (tracker.current_memory >= size) {
            tracker.current_memory -= size;
        }
    }
}

int record_allocation(void *ptr, size_t size) {
    if (tracker.count >= MAX_ALLOCATIONS) {
        printf("Warning: Maximum allocation records reached\n");
        return -1;
    }
    
    int id = tracker.allocation_id_counter++;
    tracker.records[tracker.count].ptr = ptr;
    tracker.records[tracker.count].size = size;
    tracker.records[tracker.count].alloc_time = time(NULL);
    tracker.records[tracker.count].id = id;
    tracker.records[tracker.count].freed = 0;
    tracker.count++;
    
    update_memory_stats(size, 1);
    return id;
}

int free_allocation(void *ptr) {
    for (int i = 0; i < tracker.count; i++) {
        if (tracker.records[i].ptr == ptr && !tracker.records[i].freed) {
            tracker.records[i].freed = 1;
            update_memory_stats(tracker.records[i].size, 0);
            return tracker.records[i].id;
        }
    }
    return -1;
}

void *tracked_malloc(size_t size) {
    void *ptr = malloc(size);
    if (ptr) {
        record_allocation(ptr, size);
        printf("[ALLOC] %zu bytes at %p (ID: %d)\n", size, ptr, tracker.allocation_id_counter - 1);
    } else {
        printf("[ALLOC] Failed to allocate %zu bytes: %s\n", size, strerror(errno));
    }
    return ptr;
}

void *tracked_calloc(size_t nmemb, size_t size) {
    void *ptr = calloc(nmemb, size);
    if (ptr) {
        size_t total_size = nmemb * size;
        record_allocation(ptr, total_size);
        printf("[CALLOC] %zu bytes (%zux%zu) at %p (ID: %d)\n", 
               total_size, nmemb, size, ptr, tracker.allocation_id_counter - 1);
    } else {
        printf("[CALLOC] Failed to allocate %zu bytes\n", nmemb * size);
    }
    return ptr;
}

void *tracked_realloc(void *ptr, size_t size) {
    if (ptr) {
        int old_id = free_allocation(ptr);
        if (old_id >= 0) {
            printf("[REALLOC] Freeing old allocation ID %d\n", old_id);
        }
    }
    
    void *new_ptr = realloc(ptr, size);
    if (new_ptr) {
        record_allocation(new_ptr, size);
        printf("[REALLOC] %zu bytes at %p (ID: %d)\n", size, new_ptr, tracker.allocation_id_counter - 1);
    } else {
        printf("[REALLOC] Failed to reallocate to %zu bytes\n", size);
    }
    return new_ptr;
}

void tracked_free(void *ptr) {
    if (!ptr) return;
    
    int id = free_allocation(ptr);
    if (id >= 0) {
        printf("[FREE] Freed allocation ID %d at %p\n", id, ptr);
    } else {
        printf("[FREE] Warning: Freeing untracked pointer %p\n", ptr);
    }
    free(ptr);
}

void print_memory_stats() {
    printf("\n=== Memory Statistics ===\n");
    printf("Total allocations:  %d\n", tracker.count);
    printf("Total allocated:    %zu bytes (%.2f KB)\n", 
           tracker.total_allocated, tracker.total_allocated / 1024.0);
    printf("Peak memory usage:  %zu bytes (%.2f KB)\n", 
           tracker.peak_memory, tracker.peak_memory / 1024.0);
    printf("Current memory:     %zu bytes (%.2f KB)\n", 
           tracker.current_memory, tracker.current_memory / 1024.0);
    
    int active_allocations = 0;
    for (int i = 0; i < tracker.count; i++) {
        if (!tracker.records[i].freed) {
            active_allocations++;
        }
    }
    printf("Active allocations: %d\n", active_allocations);
    
    if (active_allocations > 0) {
        printf("\n=== Memory Leaks Detected ===\n");
        for (int i = 0; i < tracker.count; i++) {
            if (!tracker.records[i].freed) {
                char *time_str = ctime(&tracker.records[i].alloc_time);
                time_str[strlen(time_str) - 1] = '\0';
                printf("Leak ID %d: %zu bytes at %p (allocated: %s)\n",
                       tracker.records[i].id, tracker.records[i].size,
                       tracker.records[i].ptr, time_str);
            }
        }
    }
}

void run_basic_tests() {
    printf("Dynamic Memory Allocation Test Suite\n");
    printf("===================================\n\n");
    
    printf("--- Test 1: Basic malloc/free ---\n");
    void *ptr1 = tracked_malloc(1024);
    void *ptr2 = tracked_malloc(2048);
    tracked_free(ptr1);
    tracked_free(ptr2);
    
    printf("\n--- Test 2: calloc (zero-initialized) ---\n");
    int *array = (int*)tracked_calloc(10, sizeof(int));
    printf("Checking zero initialization: ");
    int all_zero = 1;
    for (int i = 0; i < 10; i++) {
        if (array[i] != 0) {
            all_zero = 0;
            break;
        }
    }
    printf("%s\n", all_zero ? "PASS" : "FAIL");
    tracked_free(array);
    
    printf("\n--- Test 3: realloc ---\n");
    char *buffer = (char*)tracked_malloc(100);
    strcpy(buffer, "Hello World");
    printf("Original data: '%s'\n", buffer);
    
    buffer = (char*)tracked_realloc(buffer, 200);
    printf("After realloc: '%s'\n", buffer);
    tracked_free(buffer);
    
    printf("\n--- Test 4: Large allocation ---\n");
    void *large_ptr = tracked_malloc(1048576); // 1MB
    if (large_ptr) {
        printf("Large allocation successful\n");
        tracked_free(large_ptr);
    }
    
    print_memory_stats();
}

void run_stress_test(int num_allocations) {
    printf("Memory Stress Test\n");
    printf("==================\n");
    printf("Performing %d random allocations...\n\n", num_allocations);
    
    void **ptrs = malloc(num_allocations * sizeof(void*));
    size_t *sizes = malloc(num_allocations * sizeof(size_t));
    
    srand((unsigned int)time(NULL));
    
    // Allocate phase
    printf("Allocation phase...\n");
    for (int i = 0; i < num_allocations; i++) {
        sizes[i] = MIN_BLOCK_SIZE + rand() % (MAX_BLOCK_SIZE - MIN_BLOCK_SIZE);
        ptrs[i] = tracked_malloc(sizes[i]);
        
        if (!ptrs[i]) {
            printf("Allocation failed at iteration %d\n", i);
            num_allocations = i;
            break;
        }
        
        if (i % 100 == 0) {
            printf("Progress: %d/%d allocations\n", i, num_allocations);
        }
    }
    
    printf("\n--- Mid-test statistics ---\n");
    print_memory_stats();
    
    // Free phase (random order)
    printf("\nDeallocation phase (random order)...\n");
    for (int i = num_allocations - 1; i >= 0; i--) {
        int swap_with = rand() % (i + 1);
        void *temp_ptr = ptrs[i];
        ptrs[i] = ptrs[swap_with];
        ptrs[swap_with] = temp_ptr;
    }
    
    for (int i = 0; i < num_allocations; i++) {
        if (ptrs[i]) {
            tracked_free(ptrs[i]);
        }
        
        if (i % 100 == 0) {
            printf("Progress: %d/%d deallocations\n", i, num_allocations);
        }
    }
    
    free(ptrs);
    free(sizes);
    
    printf("\n--- Final statistics ---\n");
    print_memory_stats();
}

void run_pattern_tests() {
    printf("Memory Allocation Pattern Tests\n");
    printf("==============================\n\n");
    
    printf("--- Pattern 1: Sequential allocation ---\n");
    void *sequential[10];
    for (int i = 0; i < 10; i++) {
        sequential[i] = tracked_malloc((i + 1) * 100);
    }
    for (int i = 0; i < 10; i++) {
        tracked_free(sequential[i]);
    }
    
    printf("\n--- Pattern 2: Reverse deallocation ---\n");
    void *reverse[5];
    for (int i = 0; i < 5; i++) {
        reverse[i] = tracked_malloc(500 * (i + 1));
    }
    for (int i = 4; i >= 0; i--) {
        tracked_free(reverse[i]);
    }
    
    printf("\n--- Pattern 3: Alternating allocation/deallocation ---\n");
    for (int i = 0; i < 10; i++) {
        void *ptr = tracked_malloc(1000);
        if (i % 2 == 1) {
            tracked_free(ptr);
        }
    }
    
    print_memory_stats();
}

void run_fragmentation_test() {
    printf("Memory Fragmentation Test\n");
    printf("========================\n\n");
    
    const int NUM_BLOCKS = 20;
    void *blocks[NUM_BLOCKS];
    
    printf("Allocating %d blocks of varying sizes...\n", NUM_BLOCKS);
    for (int i = 0; i < NUM_BLOCKS; i++) {
        size_t size = 1000 + (i * 500);
        blocks[i] = tracked_malloc(size);
    }
    
    printf("\nFreeing every other block to create fragmentation...\n");
    for (int i = 1; i < NUM_BLOCKS; i += 2) {
        tracked_free(blocks[i]);
        blocks[i] = NULL;
    }
    
    printf("\nAttempting to allocate large block in fragmented space...\n");
    void *large_block = tracked_malloc(10000);
    if (large_block) {
        printf("Large allocation successful despite fragmentation\n");
        tracked_free(large_block);
    } else {
        printf("Large allocation failed due to fragmentation\n");
    }
    
    printf("\nCleaning up remaining blocks...\n");
    for (int i = 0; i < NUM_BLOCKS; i += 2) {
        if (blocks[i]) {
            tracked_free(blocks[i]);
        }
    }
    
    print_memory_stats();
}

void run_leak_test() {
    printf("Memory Leak Detection Test\n");
    printf("==========================\n\n");
    
    printf("Intentionally creating memory leaks for testing...\n");
    
    tracked_malloc(100);  // Leak 1
    tracked_malloc(500);  // Leak 2
    tracked_malloc(1000); // Leak 3
    
    void *ptr = tracked_malloc(200);
    tracked_free(ptr); // This one is properly freed
    
    printf("Test complete. Leak detection results:\n");
    print_memory_stats();
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Dynamic Memory Allocator Demo\n");
        printf("=============================\n\n");
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
        } else if (strcmp(argv[i], "--stress") == 0) {
            if (i + 1 < argc) {
                int num_allocs = atoi(argv[i + 1]);
                if (num_allocs > 0 && num_allocs <= MAX_ALLOCATIONS) {
                    run_stress_test(num_allocs);
                    i++; // Skip the number argument
                } else {
                    printf("Error: Stress test count must be between 1 and %d\n", MAX_ALLOCATIONS);
                    return 1;
                }
            } else {
                printf("Error: --stress requires a number\n");
                return 1;
            }
            return 0;
        } else if (strcmp(argv[i], "--pattern") == 0) {
            run_pattern_tests();
            return 0;
        } else if (strcmp(argv[i], "--fragmentation") == 0) {
            run_fragmentation_test();
            return 0;
        } else if (strcmp(argv[i], "--leak-test") == 0) {
            run_leak_test();
            return 0;
        } else {
            printf("Unknown option: %s\n", argv[i]);
            print_usage();
            return 1;
        }
    }
    
    return 0;
}