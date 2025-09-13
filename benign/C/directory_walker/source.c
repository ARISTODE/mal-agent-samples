#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <time.h>

#ifdef _WIN32
#include <windows.h>
#define PATH_SEPARATOR '\\'
#else
#define PATH_SEPARATOR '/'
#endif

#define MAX_PATH_LEN 1024
#define MAX_DEPTH 20

typedef struct {
    long total_files;
    long total_dirs;
    long total_size;
    long max_depth;
    long files_by_type[10];
    char largest_file[MAX_PATH_LEN];
    long largest_file_size;
} WalkStats;

typedef struct {
    char *extensions[10];
    int count;
} FileTypes;

void print_usage() {
    printf("Usage: binary.exe [DIRECTORY] [OPTIONS]\n");
    printf("  DIRECTORY: Path to directory to walk (default: current directory)\n");
    printf("\nOptions:\n");
    printf("  --help, -h     Show this help message\n");
    printf("  --recursive    Walk directories recursively (default)\n");
    printf("  --no-recursive Only list immediate contents\n");
    printf("  --details      Show detailed file information\n");
    printf("  --stats        Show statistics summary\n");
    printf("  --create-test  Create test directory structure\n");
    printf("\nExample: binary.exe /home/user --details --stats\n");
}

void init_file_types(FileTypes *types) {
    types->extensions[0] = ".txt";
    types->extensions[1] = ".c";
    types->extensions[2] = ".h";
    types->extensions[3] = ".exe";
    types->extensions[4] = ".dll";
    types->extensions[5] = ".log";
    types->extensions[6] = ".tmp";
    types->extensions[7] = ".dat";
    types->extensions[8] = ".cfg";
    types->extensions[9] = "other";
    types->count = 10;
}

int get_file_type_index(const char *filename, FileTypes *types) {
    const char *ext = strrchr(filename, '.');
    if (!ext) return types->count - 1; // "other"
    
    for (int i = 0; i < types->count - 1; i++) {
        if (strcmp(ext, types->extensions[i]) == 0) {
            return i;
        }
    }
    return types->count - 1; // "other"
}

void print_file_details(const char *filepath, struct stat *st, int depth) {
    for (int i = 0; i < depth; i++) {
        printf("  ");
    }
    
    printf("📄 %s", strrchr(filepath, PATH_SEPARATOR) ? strrchr(filepath, PATH_SEPARATOR) + 1 : filepath);
    printf(" (%ld bytes, modified: ", st->st_size);
    
    char *time_str = ctime(&st->st_mtime);
    time_str[strlen(time_str) - 1] = '\0'; // Remove newline
    printf("%s)\n", time_str);
}

void print_dir_name(const char *dirpath, int depth) {
    for (int i = 0; i < depth; i++) {
        printf("  ");
    }
    
    printf("📁 %s/\n", strrchr(dirpath, PATH_SEPARATOR) ? strrchr(dirpath, PATH_SEPARATOR) + 1 : dirpath);
}

int is_regular_file(struct stat *st) {
    return S_ISREG(st->st_mode);
}

int is_directory(struct stat *st) {
    return S_ISDIR(st->st_mode);
}

void build_path(char *dest, const char *dir, const char *filename) {
    if (strlen(dir) > 0 && dir[strlen(dir) - 1] == PATH_SEPARATOR) {
        snprintf(dest, MAX_PATH_LEN, "%s%s", dir, filename);
    } else {
        snprintf(dest, MAX_PATH_LEN, "%s%c%s", dir, PATH_SEPARATOR, filename);
    }
}

int walk_directory_recursive(const char *dir_path, int depth, int max_depth, 
                           int show_details, WalkStats *stats, FileTypes *types) {
    if (depth > max_depth) {
        printf("Maximum depth (%d) reached, stopping recursion\n", max_depth);
        return 1;
    }
    
    if (depth > stats->max_depth) {
        stats->max_depth = depth;
    }
    
    DIR *dir = opendir(dir_path);
    if (!dir) {
        printf("Error: Cannot open directory '%s': %s\n", dir_path, strerror(errno));
        return 0;
    }
    
    if (depth == 0 || show_details) {
        printf("\n");
        print_dir_name(dir_path, depth);
    }
    
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        
        char full_path[MAX_PATH_LEN];
        build_path(full_path, dir_path, entry->d_name);
        
        struct stat st;
        if (stat(full_path, &st) != 0) {
            if (show_details) {
                for (int i = 0; i <= depth; i++) printf("  ");
                printf("❌ %s (cannot stat: %s)\n", entry->d_name, strerror(errno));
            }
            continue;
        }
        
        if (is_regular_file(&st)) {
            stats->total_files++;
            stats->total_size += st.st_size;
            
            int type_idx = get_file_type_index(entry->d_name, types);
            stats->files_by_type[type_idx]++;
            
            if (st.st_size > stats->largest_file_size) {
                stats->largest_file_size = st.st_size;
                strncpy(stats->largest_file, full_path, MAX_PATH_LEN - 1);
                stats->largest_file[MAX_PATH_LEN - 1] = '\0';
            }
            
            if (show_details) {
                print_file_details(full_path, &st, depth + 1);
            }
        } else if (is_directory(&st)) {
            stats->total_dirs++;
            
            if (show_details) {
                print_dir_name(full_path, depth + 1);
            }
            
            walk_directory_recursive(full_path, depth + 1, max_depth, show_details, stats, types);
        }
    }
    
    closedir(dir);
    return 1;
}

int walk_directory_simple(const char *dir_path, int show_details, WalkStats *stats, FileTypes *types) {
    DIR *dir = opendir(dir_path);
    if (!dir) {
        printf("Error: Cannot open directory '%s': %s\n", dir_path, strerror(errno));
        return 0;
    }
    
    printf("\nContents of '%s':\n", dir_path);
    
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        
        char full_path[MAX_PATH_LEN];
        build_path(full_path, dir_path, entry->d_name);
        
        struct stat st;
        if (stat(full_path, &st) != 0) {
            printf("❌ %s (cannot stat: %s)\n", entry->d_name, strerror(errno));
            continue;
        }
        
        if (is_regular_file(&st)) {
            stats->total_files++;
            stats->total_size += st.st_size;
            
            int type_idx = get_file_type_index(entry->d_name, types);
            stats->files_by_type[type_idx]++;
            
            if (st.st_size > stats->largest_file_size) {
                stats->largest_file_size = st.st_size;
                strncpy(stats->largest_file, full_path, MAX_PATH_LEN - 1);
                stats->largest_file[MAX_PATH_LEN - 1] = '\0';
            }
            
            if (show_details) {
                print_file_details(full_path, &st, 0);
            } else {
                printf("📄 %s\n", entry->d_name);
            }
        } else if (is_directory(&st)) {
            stats->total_dirs++;
            printf("📁 %s/\n", entry->d_name);
        }
    }
    
    closedir(dir);
    return 1;
}

void print_statistics(const WalkStats *stats, const FileTypes *types) {
    printf("\n=== Directory Walk Statistics ===\n");
    printf("Total files:      %ld\n", stats->total_files);
    printf("Total directories: %ld\n", stats->total_dirs);
    printf("Total size:       %ld bytes (%.2f KB)\n", 
           stats->total_size, stats->total_size / 1024.0);
    printf("Maximum depth:    %ld levels\n", stats->max_depth);
    
    if (stats->largest_file_size > 0) {
        printf("Largest file:     %s (%ld bytes)\n", 
               stats->largest_file, stats->largest_file_size);
    }
    
    printf("\nFile types breakdown:\n");
    for (int i = 0; i < types->count; i++) {
        if (stats->files_by_type[i] > 0) {
            printf("  %-8s: %ld files\n", types->extensions[i], stats->files_by_type[i]);
        }
    }
}

int create_test_structure() {
    printf("Creating test directory structure...\n");
    
    const char *dirs[] = {
        "test_dir",
        "test_dir/subdir1",
        "test_dir/subdir2",
        "test_dir/subdir1/deep",
        "test_dir/subdir2/another"
    };
    
    const char *files[] = {
        "test_dir/readme.txt",
        "test_dir/config.cfg",
        "test_dir/program.exe",
        "test_dir/subdir1/source.c",
        "test_dir/subdir1/header.h",
        "test_dir/subdir1/deep/data.dat",
        "test_dir/subdir2/log.log",
        "test_dir/subdir2/temp.tmp",
        "test_dir/subdir2/another/library.dll"
    };
    
    const int num_dirs = sizeof(dirs) / sizeof(dirs[0]);
    const int num_files = sizeof(files) / sizeof(files[0]);
    
    for (int i = 0; i < num_dirs; i++) {
#ifdef _WIN32
        if (CreateDirectoryA(dirs[i], NULL) == 0 && GetLastError() != ERROR_ALREADY_EXISTS) {
            printf("Error creating directory '%s': Windows error %ld\n", dirs[i], GetLastError());
        } else {
            printf("Created directory: %s\n", dirs[i]);
        }
#else
        if (mkdir(dirs[i], 0755) != 0 && errno != EEXIST) {
            printf("Error creating directory '%s': %s\n", dirs[i], strerror(errno));
        } else {
            printf("Created directory: %s\n", dirs[i]);
        }
#endif
    }
    
    for (int i = 0; i < num_files; i++) {
        FILE *f = fopen(files[i], "w");
        if (f) {
            fprintf(f, "This is test file %s\nCreated for directory walking test.\n", files[i]);
            fclose(f);
            printf("Created file: %s\n", files[i]);
        } else {
            printf("Error creating file '%s': %s\n", files[i], strerror(errno));
        }
    }
    
    printf("\nTest structure created successfully!\n");
    printf("Try: ./binary.exe test_dir --details --stats\n");
    return 1;
}

int main(int argc, char *argv[]) {
    const char *target_dir = ".";
    int recursive = 1;
    int show_details = 0;
    int show_stats = 0;
    int create_test = 0;
    
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            print_usage();
            return 0;
        } else if (strcmp(argv[i], "--recursive") == 0) {
            recursive = 1;
        } else if (strcmp(argv[i], "--no-recursive") == 0) {
            recursive = 0;
        } else if (strcmp(argv[i], "--details") == 0) {
            show_details = 1;
        } else if (strcmp(argv[i], "--stats") == 0) {
            show_stats = 1;
        } else if (strcmp(argv[i], "--create-test") == 0) {
            create_test = 1;
        } else if (argv[i][0] != '-') {
            target_dir = argv[i];
        } else {
            printf("Unknown option: %s\n", argv[i]);
            print_usage();
            return 1;
        }
    }
    
    if (create_test) {
        return create_test_structure() ? 0 : 1;
    }
    
    printf("Directory Walker\n");
    printf("================\n");
    printf("Target: %s\n", target_dir);
    printf("Mode: %s\n", recursive ? "Recursive" : "Non-recursive");
    
    WalkStats stats = {0};
    stats.largest_file_size = 0;
    stats.largest_file[0] = '\0';
    
    FileTypes types;
    init_file_types(&types);
    
    int success;
    if (recursive) {
        success = walk_directory_recursive(target_dir, 0, MAX_DEPTH, show_details, &stats, &types);
    } else {
        success = walk_directory_simple(target_dir, show_details, &stats, &types);
    }
    
    if (!success) {
        return 1;
    }
    
    if (show_stats) {
        print_statistics(&stats, &types);
    } else {
        printf("\nSummary: %ld files, %ld directories, %ld bytes total\n", 
               stats.total_files, stats.total_dirs, stats.total_size);
    }
    
    return 0;
}