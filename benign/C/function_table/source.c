#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#define MAX_FUNCTIONS 20
#define MAX_NAME_LEN 32
#define MAX_OPERATIONS 100

// Function pointer types for different operation categories
typedef int (*MathOperation)(int a, int b);
typedef void (*StringOperation)(const char* input, char* output, size_t max_len);
typedef int (*ValidationOperation)(const char* input);
typedef void (*UtilityOperation)(void);

// Generic function pointer for dynamic dispatch
typedef void* (*GenericFunction)(void* arg1, void* arg2);

// Function table entry structure
typedef struct {
    char name[MAX_NAME_LEN];
    GenericFunction function;
    char description[128];
    int call_count;
    time_t last_called;
    char category[16];
} FunctionEntry;

// Global function table
static FunctionEntry function_table[MAX_FUNCTIONS];
static int function_count = 0;

// Operation history
typedef struct {
    char operation[64];
    char arguments[128];
    char result[64];
    time_t timestamp;
} OperationHistory;

static OperationHistory history[MAX_OPERATIONS];
static int history_count = 0;

void print_usage() {
    printf("Usage: binary.exe [COMMAND] [ARGS...]\n");
    printf("Function Table and Dynamic Dispatch Demonstration\n\n");
    printf("Commands:\n");
    printf("  --help, -h         Show this help message\n");
    printf("  --test             Run comprehensive function table tests\n");
    printf("  --list             List all registered functions\n");
    printf("  --call NAME ARGS   Call a specific function by name\n");
    printf("  --math A OP B      Perform math operation (add, sub, mul, div, mod, pow)\n");
    printf("  --string OP TEXT   Perform string operation (upper, lower, reverse, length)\n");
    printf("  --validate TYPE T  Validate text (email, phone, number, alpha)\n");
    printf("  --history          Show operation history\n");
    printf("  --benchmark N      Run performance benchmark with N iterations\n");
    printf("\nFeatures:\n");
    printf("  - Function pointer tables\n");
    printf("  - Dynamic function dispatch\n");
    printf("  - Operation history tracking\n");
    printf("  - Performance benchmarking\n");
    printf("  - Multiple function categories\n");
}

// Add entry to operation history
void add_to_history(const char* operation, const char* arguments, const char* result) {
    if (history_count >= MAX_OPERATIONS) {
        // Shift history entries (FIFO)
        for (int i = 0; i < MAX_OPERATIONS - 1; i++) {
            history[i] = history[i + 1];
        }
        history_count = MAX_OPERATIONS - 1;
    }

    OperationHistory* entry = &history[history_count];
    strncpy(entry->operation, operation, sizeof(entry->operation) - 1);
    strncpy(entry->arguments, arguments, sizeof(entry->arguments) - 1);
    strncpy(entry->result, result, sizeof(entry->result) - 1);
    entry->operation[sizeof(entry->operation) - 1] = '\0';
    entry->arguments[sizeof(entry->arguments) - 1] = '\0';
    entry->result[sizeof(entry->result) - 1] = '\0';
    entry->timestamp = time(NULL);
    history_count++;
}

// Math operations
int math_add(int a, int b) { return a + b; }
int math_subtract(int a, int b) { return a - b; }
int math_multiply(int a, int b) { return a * b; }
int math_divide(int a, int b) { return b != 0 ? a / b : 0; }
int math_modulo(int a, int b) { return b != 0 ? a % b : 0; }
int math_power(int a, int b) { return (int)pow(a, b); }

// Math operation wrappers for function table
void* wrapper_add(void* arg1, void* arg2) {
    int* a = (int*)arg1;
    int* b = (int*)arg2;
    static int result;
    result = math_add(*a, *b);
    return &result;
}

void* wrapper_subtract(void* arg1, void* arg2) {
    int* a = (int*)arg1;
    int* b = (int*)arg2;
    static int result;
    result = math_subtract(*a, *b);
    return &result;
}

void* wrapper_multiply(void* arg1, void* arg2) {
    int* a = (int*)arg1;
    int* b = (int*)arg2;
    static int result;
    result = math_multiply(*a, *b);
    return &result;
}

void* wrapper_divide(void* arg1, void* arg2) {
    int* a = (int*)arg1;
    int* b = (int*)arg2;
    static int result;
    result = math_divide(*a, *b);
    return &result;
}

void* wrapper_modulo(void* arg1, void* arg2) {
    int* a = (int*)arg1;
    int* b = (int*)arg2;
    static int result;
    result = math_modulo(*a, *b);
    return &result;
}

void* wrapper_power(void* arg1, void* arg2) {
    int* a = (int*)arg1;
    int* b = (int*)arg2;
    static int result;
    result = math_power(*a, *b);
    return &result;
}

// String operations
void string_upper(const char* input, char* output, size_t max_len) {
    size_t len = strlen(input);
    if (len >= max_len) len = max_len - 1;
    for (size_t i = 0; i < len; i++) {
        output[i] = (input[i] >= 'a' && input[i] <= 'z') ? input[i] - 32 : input[i];
    }
    output[len] = '\0';
}

void string_lower(const char* input, char* output, size_t max_len) {
    size_t len = strlen(input);
    if (len >= max_len) len = max_len - 1;
    for (size_t i = 0; i < len; i++) {
        output[i] = (input[i] >= 'A' && input[i] <= 'Z') ? input[i] + 32 : input[i];
    }
    output[len] = '\0';
}

void string_reverse(const char* input, char* output, size_t max_len) {
    size_t len = strlen(input);
    if (len >= max_len) len = max_len - 1;
    for (size_t i = 0; i < len; i++) {
        output[i] = input[len - 1 - i];
    }
    output[len] = '\0';
}

int string_length(const char* input) {
    return (int)strlen(input);
}

// String operation wrappers
void* wrapper_upper(void* arg1, void* arg2) {
    char* input = (char*)arg1;
    static char result[256];
    string_upper(input, result, sizeof(result));
    (void)arg2; // Unused
    return result;
}

void* wrapper_lower(void* arg1, void* arg2) {
    char* input = (char*)arg1;
    static char result[256];
    string_lower(input, result, sizeof(result));
    (void)arg2; // Unused
    return result;
}

void* wrapper_reverse(void* arg1, void* arg2) {
    char* input = (char*)arg1;
    static char result[256];
    string_reverse(input, result, sizeof(result));
    (void)arg2; // Unused
    return result;
}

void* wrapper_strlen(void* arg1, void* arg2) {
    char* input = (char*)arg1;
    static int result;
    result = string_length(input);
    (void)arg2; // Unused
    return &result;
}

// Validation operations
int validate_email(const char* input) {
    return (strchr(input, '@') != NULL && strchr(input, '.') != NULL);
}

int validate_phone(const char* input) {
    int digit_count = 0;
    for (const char* p = input; *p; p++) {
        if (*p >= '0' && *p <= '9') digit_count++;
        else if (*p != '-' && *p != '(' && *p != ')' && *p != ' ') return 0;
    }
    return digit_count >= 10;
}

int validate_number(const char* input) {
    if (!*input) return 0;
    for (const char* p = input; *p; p++) {
        if (*p < '0' || *p > '9') {
            if (*p == '-' && p == input) continue; // Allow negative sign at start
            if (*p == '.' && strchr(p + 1, '.') == NULL) continue; // Allow one decimal point
            return 0;
        }
    }
    return 1;
}

int validate_alpha(const char* input) {
    for (const char* p = input; *p; p++) {
        if (!((*p >= 'a' && *p <= 'z') || (*p >= 'A' && *p <= 'Z') || *p == ' ')) {
            return 0;
        }
    }
    return 1;
}

// Validation wrappers
void* wrapper_validate_email(void* arg1, void* arg2) {
    char* input = (char*)arg1;
    static int result;
    result = validate_email(input);
    (void)arg2; // Unused
    return &result;
}

void* wrapper_validate_phone(void* arg1, void* arg2) {
    char* input = (char*)arg1;
    static int result;
    result = validate_phone(input);
    (void)arg2; // Unused
    return &result;
}

void* wrapper_validate_number(void* arg1, void* arg2) {
    char* input = (char*)arg1;
    static int result;
    result = validate_number(input);
    (void)arg2; // Unused
    return &result;
}

void* wrapper_validate_alpha(void* arg1, void* arg2) {
    char* input = (char*)arg1;
    static int result;
    result = validate_alpha(input);
    (void)arg2; // Unused
    return &result;
}

// Utility functions
void utility_random_seed(void) {
    srand((unsigned int)time(NULL));
    printf("Random seed initialized with current time\n");
}

void utility_print_time(void) {
    time_t now = time(NULL);
    printf("Current time: %s", ctime(&now));
}

void utility_system_info(void) {
    printf("System Information:\n");
    printf("  sizeof(int): %zu bytes\n", sizeof(int));
    printf("  sizeof(long): %zu bytes\n", sizeof(long));
    printf("  sizeof(void*): %zu bytes\n", sizeof(void*));
    printf("  Function table capacity: %d entries\n", MAX_FUNCTIONS);
    printf("  History capacity: %d operations\n", MAX_OPERATIONS);
}

// Utility wrappers
void* wrapper_random_seed(void* arg1, void* arg2) {
    utility_random_seed();
    (void)arg1; (void)arg2; // Unused
    return NULL;
}

void* wrapper_print_time(void* arg1, void* arg2) {
    utility_print_time();
    (void)arg1; (void)arg2; // Unused
    return NULL;
}

void* wrapper_system_info(void* arg1, void* arg2) {
    utility_system_info();
    (void)arg1; (void)arg2; // Unused
    return NULL;
}

// Register a function in the function table
int register_function(const char* name, GenericFunction func, const char* description, const char* category) {
    if (function_count >= MAX_FUNCTIONS) {
        printf("Function table full, cannot register '%s'\n", name);
        return 0;
    }

    FunctionEntry* entry = &function_table[function_count];
    strncpy(entry->name, name, sizeof(entry->name) - 1);
    entry->name[sizeof(entry->name) - 1] = '\0';
    entry->function = func;
    strncpy(entry->description, description, sizeof(entry->description) - 1);
    entry->description[sizeof(entry->description) - 1] = '\0';
    strncpy(entry->category, category, sizeof(entry->category) - 1);
    entry->category[sizeof(entry->category) - 1] = '\0';
    entry->call_count = 0;
    entry->last_called = 0;

    function_count++;
    printf("Registered function '%s' in category '%s'\n", name, category);
    return 1;
}

// Find function by name
FunctionEntry* find_function(const char* name) {
    for (int i = 0; i < function_count; i++) {
        if (strcmp(function_table[i].name, name) == 0) {
            return &function_table[i];
        }
    }
    return NULL;
}

// Call function by name
void* call_function(const char* name, void* arg1, void* arg2) {
    FunctionEntry* entry = find_function(name);
    if (!entry) {
        printf("Function '%s' not found\n", name);
        return NULL;
    }

    printf("Calling function '%s'...\n", name);
    entry->call_count++;
    entry->last_called = time(NULL);

    void* result = entry->function(arg1, arg2);
    return result;
}

// Initialize function table with built-in functions
void initialize_function_table() {
    printf("Initializing function table...\n");

    // Math functions
    register_function("add", wrapper_add, "Add two integers", "math");
    register_function("subtract", wrapper_subtract, "Subtract two integers", "math");
    register_function("multiply", wrapper_multiply, "Multiply two integers", "math");
    register_function("divide", wrapper_divide, "Divide two integers", "math");
    register_function("modulo", wrapper_modulo, "Modulo of two integers", "math");
    register_function("power", wrapper_power, "Power of two integers", "math");

    // String functions
    register_function("upper", wrapper_upper, "Convert string to uppercase", "string");
    register_function("lower", wrapper_lower, "Convert string to lowercase", "string");
    register_function("reverse", wrapper_reverse, "Reverse a string", "string");
    register_function("strlen", wrapper_strlen, "Get string length", "string");

    // Validation functions
    register_function("validate_email", wrapper_validate_email, "Validate email format", "validation");
    register_function("validate_phone", wrapper_validate_phone, "Validate phone number", "validation");
    register_function("validate_number", wrapper_validate_number, "Validate numeric string", "validation");
    register_function("validate_alpha", wrapper_validate_alpha, "Validate alphabetic string", "validation");

    // Utility functions
    register_function("random_seed", wrapper_random_seed, "Initialize random seed", "utility");
    register_function("print_time", wrapper_print_time, "Print current time", "utility");
    register_function("system_info", wrapper_system_info, "Show system information", "utility");

    printf("Function table initialized with %d functions\n", function_count);
}

// List all registered functions
void list_functions() {
    printf("\n=== Function Table ===\n");
    printf("Total functions: %d\n\n", function_count);

    const char* categories[] = {"math", "string", "validation", "utility"};
    const int num_categories = sizeof(categories) / sizeof(categories[0]);

    for (int c = 0; c < num_categories; c++) {
        printf("Category: %s\n", categories[c]);
        printf("===================\n");

        int category_count = 0;
        for (int i = 0; i < function_count; i++) {
            if (strcmp(function_table[i].category, categories[c]) == 0) {
                FunctionEntry* entry = &function_table[i];
                printf("%2d. %-15s - %s\n", category_count + 1, entry->name, entry->description);
                printf("    Calls: %d", entry->call_count);
                if (entry->last_called > 0) {
                    printf(", Last called: %s", ctime(&entry->last_called));
                } else {
                    printf(", Never called\n");
                }
                category_count++;
            }
        }
        if (category_count == 0) {
            printf("  (no functions)\n");
        }
        printf("\n");
    }
}

// Show operation history
void show_history() {
    printf("\n=== Operation History ===\n");
    if (history_count == 0) {
        printf("No operations recorded\n");
        return;
    }

    printf("Total operations: %d\n\n", history_count);
    for (int i = history_count - 1; i >= 0; i--) {  // Show most recent first
        OperationHistory* entry = &history[i];
        printf("%2d. %s(%s) = %s\n", history_count - i, entry->operation, entry->arguments, entry->result);
        printf("    Time: %s", ctime(&entry->timestamp));
    }
}

// Performance benchmark
void run_benchmark(int iterations) {
    printf("Function Table Performance Benchmark\n");
    printf("===================================\n");
    printf("Running %d iterations...\n\n", iterations);

    clock_t start = clock();

    // Test different types of operations
    int math_ops = iterations / 4;
    int string_ops = iterations / 4;
    int validation_ops = iterations / 4;
    int utility_ops = iterations - math_ops - string_ops - validation_ops;

    // Math operations
    for (int i = 0; i < math_ops; i++) {
        int a = i + 1;
        int b = (i % 10) + 1;
        call_function("add", &a, &b);
        call_function("multiply", &a, &b);
    }

    // String operations
    char test_string[] = "BenchmarkTest";
    for (int i = 0; i < string_ops; i++) {
        call_function("upper", test_string, NULL);
        call_function("strlen", test_string, NULL);
    }

    // Validation operations
    char test_email[] = "test@example.com";
    char test_number[] = "12345";
    for (int i = 0; i < validation_ops; i++) {
        call_function("validate_email", test_email, NULL);
        call_function("validate_number", test_number, NULL);
    }

    // Utility operations
    for (int i = 0; i < utility_ops; i++) {
        call_function("random_seed", NULL, NULL);
    }

    clock_t end = clock();
    double elapsed = ((double)(end - start)) / CLOCKS_PER_SEC;

    printf("\nBenchmark Results:\n");
    printf("==================\n");
    printf("Total operations: %d\n", iterations);
    printf("Elapsed time: %.4f seconds\n", elapsed);
    printf("Operations per second: %.0f\n", iterations / elapsed);
    printf("Average time per operation: %.6f seconds\n", elapsed / iterations);
}

// Run comprehensive tests
void run_tests() {
    printf("Function Table Test Suite\n");
    printf("=========================\n\n");

    initialize_function_table();

    printf("\n--- Test 1: Math Operations ---\n");
    int a = 15, b = 3;
    void* result;

    result = call_function("add", &a, &b);
    if (result) {
        int val = *(int*)result;
        printf("Result: %d + %d = %d\n", a, b, val);
        char args[64], res[64];
        snprintf(args, sizeof(args), "%d, %d", a, b);
        snprintf(res, sizeof(res), "%d", val);
        add_to_history("add", args, res);
    }

    result = call_function("divide", &a, &b);
    if (result) {
        int val = *(int*)result;
        printf("Result: %d / %d = %d\n", a, b, val);
        char args[64], res[64];
        snprintf(args, sizeof(args), "%d, %d", a, b);
        snprintf(res, sizeof(res), "%d", val);
        add_to_history("divide", args, res);
    }

    printf("\n--- Test 2: String Operations ---\n");
    char test_string[] = "Hello World";

    result = call_function("upper", test_string, NULL);
    if (result) {
        char* val = (char*)result;
        printf("Result: upper('%s') = '%s'\n", test_string, val);
        add_to_history("upper", test_string, val);
    }

    result = call_function("reverse", test_string, NULL);
    if (result) {
        char* val = (char*)result;
        printf("Result: reverse('%s') = '%s'\n", test_string, val);
        add_to_history("reverse", test_string, val);
    }

    printf("\n--- Test 3: Validation Operations ---\n");
    char test_email[] = "user@domain.com";
    char test_phone[] = "(555) 123-4567";

    result = call_function("validate_email", test_email, NULL);
    if (result) {
        int val = *(int*)result;
        printf("Result: validate_email('%s') = %s\n", test_email, val ? "valid" : "invalid");
        add_to_history("validate_email", test_email, val ? "valid" : "invalid");
    }

    result = call_function("validate_phone", test_phone, NULL);
    if (result) {
        int val = *(int*)result;
        printf("Result: validate_phone('%s') = %s\n", test_phone, val ? "valid" : "invalid");
        add_to_history("validate_phone", test_phone, val ? "valid" : "invalid");
    }

    printf("\n--- Test 4: Utility Operations ---\n");
    call_function("system_info", NULL, NULL);
    add_to_history("system_info", "", "displayed");

    call_function("print_time", NULL, NULL);
    add_to_history("print_time", "", "displayed");

    list_functions();
    show_history();

    printf("\nFunction table tests completed successfully!\n");
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Function Table and Dynamic Dispatch System\n");
        printf("==========================================\n\n");
        print_usage();
        return 1;
    }

    if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0) {
        print_usage();
        return 0;
    }

    if (strcmp(argv[1], "--test") == 0) {
        run_tests();
        return 0;
    }

    initialize_function_table();

    if (strcmp(argv[1], "--list") == 0) {
        list_functions();
        return 0;
    }

    if (strcmp(argv[1], "--history") == 0) {
        show_history();
        return 0;
    }

    if (strcmp(argv[1], "--benchmark") == 0) {
        int iterations = 1000;
        if (argc > 2) {
            iterations = atoi(argv[2]);
            if (iterations < 1 || iterations > 1000000) {
                printf("Error: Iterations must be between 1 and 1,000,000\n");
                return 1;
            }
        }
        run_benchmark(iterations);
        return 0;
    }

    if (strcmp(argv[1], "--call") == 0) {
        if (argc < 3) {
            printf("Error: --call requires function name\n");
            return 1;
        }
        // Simple function call (would need more complex argument parsing for real use)
        printf("Calling function '%s' with default arguments\n", argv[2]);
        call_function(argv[2], NULL, NULL);
        return 0;
    }

    if (strcmp(argv[1], "--math") == 0) {
        if (argc < 5) {
            printf("Error: --math requires A OP B format\n");
            return 1;
        }
        int a = atoi(argv[2]);
        char* op = argv[3];
        int b = atoi(argv[4]);

        char op_name[32];
        if (strcmp(op, "+") == 0 || strcmp(op, "add") == 0) strcpy(op_name, "add");
        else if (strcmp(op, "-") == 0 || strcmp(op, "sub") == 0) strcpy(op_name, "subtract");
        else if (strcmp(op, "*") == 0 || strcmp(op, "mul") == 0) strcpy(op_name, "multiply");
        else if (strcmp(op, "/") == 0 || strcmp(op, "div") == 0) strcpy(op_name, "divide");
        else if (strcmp(op, "%") == 0 || strcmp(op, "mod") == 0) strcpy(op_name, "modulo");
        else if (strcmp(op, "^") == 0 || strcmp(op, "pow") == 0) strcpy(op_name, "power");
        else {
            printf("Error: Unknown operation '%s'\n", op);
            return 1;
        }

        void* result = call_function(op_name, &a, &b);
        if (result) {
            int val = *(int*)result;
            printf("Result: %d %s %d = %d\n", a, op, b, val);
        }
        return 0;
    }

    if (strcmp(argv[1], "--string") == 0) {
        if (argc < 4) {
            printf("Error: --string requires OP TEXT format\n");
            return 1;
        }
        char* op = argv[2];
        char* text = argv[3];

        void* result = call_function(op, text, NULL);
        if (result) {
            if (strcmp(op, "strlen") == 0) {
                int val = *(int*)result;
                printf("Result: %s('%s') = %d\n", op, text, val);
            } else {
                char* val = (char*)result;
                printf("Result: %s('%s') = '%s'\n", op, text, val);
            }
        }
        return 0;
    }

    if (strcmp(argv[1], "--validate") == 0) {
        if (argc < 4) {
            printf("Error: --validate requires TYPE TEXT format\n");
            return 1;
        }
        char* type = argv[2];
        char* text = argv[3];

        char func_name[64];
        snprintf(func_name, sizeof(func_name), "validate_%s", type);

        void* result = call_function(func_name, text, NULL);
        if (result) {
            int val = *(int*)result;
            printf("Result: %s('%s') = %s\n", func_name, text, val ? "valid" : "invalid");
        }
        return 0;
    }

    printf("Unknown command: %s\n", argv[1]);
    print_usage();
    return 1;
}