#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <time.h>

#define BUFFER_SIZE 4096
#define MAX_PATH_LEN 1024

typedef struct {
    char *source_path;
    char *dest_path;
    long file_size;
    long bytes_copied;
    time_t start_time;
    time_t end_time;
    int success;
} CopyResult;

void print_usage() {
    printf("Usage: binary.exe [SOURCE] [DESTINATION]\n");
    printf("  SOURCE:      Path to source file\n");
    printf("  DESTINATION: Path to destination file\n");
    printf("\nOptions:\n");
    printf("  --help, -h   Show this help message\n");
    printf("  --test       Run built-in tests\n");
    printf("\nExample: binary.exe input.txt output.txt\n");
}

long get_file_size(const char *filename) {
    struct stat st;
    if (stat(filename, &st) == 0) {
        return st.st_size;
    }
    return -1;
}

void print_file_info(const char *filename) {
    struct stat st;
    if (stat(filename, &st) == 0) {
        printf("File: %s\n", filename);
        printf("  Size: %ld bytes\n", st.st_size);
        printf("  Modified: %s", ctime(&st.st_mtime));
    } else {
        printf("File: %s (not found)\n", filename);
    }
}

int copy_file_simple(const char *source, const char *dest) {
    FILE *src_file = fopen(source, "rb");
    if (!src_file) {
        printf("Error: Cannot open source file '%s': %s\n", source, strerror(errno));
        return 0;
    }
    
    FILE *dest_file = fopen(dest, "wb");
    if (!dest_file) {
        printf("Error: Cannot create destination file '%s': %s\n", dest, strerror(errno));
        fclose(src_file);
        return 0;
    }
    
    char buffer[BUFFER_SIZE];
    size_t bytes_read;
    long total_bytes = 0;
    
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), src_file)) > 0) {
        size_t bytes_written = fwrite(buffer, 1, bytes_read, dest_file);
        total_bytes += bytes_written;
        
        if (bytes_written != bytes_read) {
            printf("Error: Write failed after %ld bytes\n", total_bytes);
            fclose(src_file);
            fclose(dest_file);
            return 0;
        }
    }
    
    if (ferror(src_file)) {
        printf("Error: Read failed after %ld bytes\n", total_bytes);
        fclose(src_file);
        fclose(dest_file);
        return 0;
    }
    
    fclose(src_file);
    fclose(dest_file);
    
    printf("Successfully copied %ld bytes from '%s' to '%s'\n", total_bytes, source, dest);
    return 1;
}

int copy_file_with_progress(const char *source, const char *dest, CopyResult *result) {
    result->source_path = malloc(strlen(source) + 1);
    result->dest_path = malloc(strlen(dest) + 1);
    strcpy(result->source_path, source);
    strcpy(result->dest_path, dest);
    result->bytes_copied = 0;
    result->success = 0;
    result->start_time = time(NULL);
    
    result->file_size = get_file_size(source);
    if (result->file_size < 0) {
        printf("Error: Cannot get size of source file '%s'\n", source);
        return 0;
    }
    
    FILE *src_file = fopen(source, "rb");
    if (!src_file) {
        printf("Error: Cannot open source file '%s': %s\n", source, strerror(errno));
        return 0;
    }
    
    FILE *dest_file = fopen(dest, "wb");
    if (!dest_file) {
        printf("Error: Cannot create destination file '%s': %s\n", dest, strerror(errno));
        fclose(src_file);
        return 0;
    }
    
    char buffer[BUFFER_SIZE];
    size_t bytes_read;
    long last_progress = -1;
    
    printf("Copying file...\n");
    
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), src_file)) > 0) {
        size_t bytes_written = fwrite(buffer, 1, bytes_read, dest_file);
        result->bytes_copied += bytes_written;
        
        if (bytes_written != bytes_read) {
            printf("Error: Write failed after %ld bytes\n", result->bytes_copied);
            fclose(src_file);
            fclose(dest_file);
            return 0;
        }
        
        if (result->file_size > 0) {
            long progress = (result->bytes_copied * 100) / result->file_size;
            if (progress != last_progress && progress % 10 == 0) {
                printf("Progress: %ld%% (%ld/%ld bytes)\n", 
                       progress, result->bytes_copied, result->file_size);
                last_progress = progress;
            }
        }
    }
    
    if (ferror(src_file)) {
        printf("Error: Read failed after %ld bytes\n", result->bytes_copied);
        fclose(src_file);
        fclose(dest_file);
        return 0;
    }
    
    fclose(src_file);
    fclose(dest_file);
    
    result->end_time = time(NULL);
    result->success = 1;
    
    return 1;
}

void print_copy_result(const CopyResult *result) {
    printf("\n=== Copy Operation Results ===\n");
    printf("Source:      %s\n", result->source_path);
    printf("Destination: %s\n", result->dest_path);
    printf("File size:   %ld bytes\n", result->file_size);
    printf("Copied:      %ld bytes\n", result->bytes_copied);
    printf("Duration:    %d seconds\n", (int)(result->end_time - result->start_time));
    printf("Status:      %s\n", result->success ? "SUCCESS" : "FAILED");
    
    if (result->success && result->file_size > 0) {
        double duration = difftime(result->end_time, result->start_time);
        if (duration > 0) {
            double speed = result->bytes_copied / duration;
            printf("Speed:       %.2f bytes/sec\n", speed);
        }
    }
}

void free_copy_result(CopyResult *result) {
    if (result->source_path) free(result->source_path);
    if (result->dest_path) free(result->dest_path);
}

int create_test_file(const char *filename, int size_kb) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        printf("Error: Cannot create test file '%s': %s\n", filename, strerror(errno));
        return 0;
    }
    
    char line[100];
    int lines_written = 0;
    int target_bytes = size_kb * 1024;
    int current_bytes = 0;
    
    while (current_bytes < target_bytes) {
        int line_len = snprintf(line, sizeof(line), 
                               "This is test line %d with some content to fill the file.\n", 
                               lines_written + 1);
        
        if (fputs(line, file) == EOF) {
            printf("Error writing to test file\n");
            fclose(file);
            return 0;
        }
        
        current_bytes += line_len;
        lines_written++;
    }
    
    fclose(file);
    printf("Created test file '%s' with %d lines (%d bytes)\n", 
           filename, lines_written, current_bytes);
    return 1;
}

void run_tests() {
    printf("File Copy Test Suite\n");
    printf("===================\n\n");
    
    const char *test_files[] = {"test_small.txt", "test_medium.txt", "test_large.txt"};
    const int test_sizes[] = {1, 10, 100};  // KB
    const int num_tests = sizeof(test_files) / sizeof(test_files[0]);
    
    printf("Creating test files...\n");
    for (int i = 0; i < num_tests; i++) {
        if (!create_test_file(test_files[i], test_sizes[i])) {
            return;
        }
    }
    
    printf("\nTesting file copy operations...\n");
    for (int i = 0; i < num_tests; i++) {
        char dest_name[MAX_PATH_LEN];
        snprintf(dest_name, sizeof(dest_name), "copy_%s", test_files[i]);
        
        printf("\n--- Test %d: %s (%d KB) ---\n", i + 1, test_files[i], test_sizes[i]);
        
        print_file_info(test_files[i]);
        
        CopyResult result;
        if (copy_file_with_progress(test_files[i], dest_name, &result)) {
            print_copy_result(&result);
            
            long orig_size = get_file_size(test_files[i]);
            long copy_size = get_file_size(dest_name);
            
            if (orig_size == copy_size) {
                printf("Verification: File sizes match ✓\n");
            } else {
                printf("Verification: File sizes differ! Original: %ld, Copy: %ld ✗\n", 
                       orig_size, copy_size);
            }
        }
        
        free_copy_result(&result);
    }
    
    printf("\nCleaning up test files...\n");
    for (int i = 0; i < num_tests; i++) {
        remove(test_files[i]);
        
        char dest_name[MAX_PATH_LEN];
        snprintf(dest_name, sizeof(dest_name), "copy_%s", test_files[i]);
        remove(dest_name);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("File Copy Utility\n");
        printf("=================\n\n");
        print_usage();
        return 1;
    }
    
    if (argc == 2) {
        if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0) {
            print_usage();
            return 0;
        } else if (strcmp(argv[1], "--test") == 0) {
            run_tests();
            return 0;
        } else {
            printf("Error: Missing destination argument\n");
            print_usage();
            return 1;
        }
    }
    
    if (argc == 3) {
        const char *source = argv[1];
        const char *dest = argv[2];
        
        printf("File Copy Operation\n");
        printf("==================\n");
        printf("Source:      %s\n", source);
        printf("Destination: %s\n\n", dest);
        
        print_file_info(source);
        printf("\n");
        
        CopyResult result;
        if (copy_file_with_progress(source, dest, &result)) {
            print_copy_result(&result);
            print_file_info(dest);
        } else {
            printf("Copy operation failed.\n");
        }
        
        free_copy_result(&result);
        return result.success ? 0 : 1;
    }
    
    printf("Error: Too many arguments\n");
    print_usage();
    return 1;
}