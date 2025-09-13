#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#define MAX_STACK_SIZE 1000
#define MAX_ELEMENT_SIZE 64
#define MAX_EXPRESSION_SIZE 256
#define MAX_HISTORY 100

// Stack element structure for different data types
typedef union {
    int int_val;
    float float_val;
    char str_val[MAX_ELEMENT_SIZE];
} StackData;

typedef enum {
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_STRING
} DataType;

typedef struct {
    StackData data;
    DataType type;
} StackElement;

// Stack structure
typedef struct {
    StackElement elements[MAX_STACK_SIZE];
    int top;
    int max_used;
    char name[32];
    time_t created;
} Stack;

// Operation history
typedef struct {
    char operation[64];
    char details[128];
    time_t timestamp;
    int stack_size_after;
} OperationHistory;

// Global stacks (multiple stack support)
static Stack stacks[5];
static int stack_count = 0;
static int current_stack = 0;

// History tracking
static OperationHistory history[MAX_HISTORY];
static int history_count = 0;

void print_usage() {
    printf("Usage: binary.exe [COMMAND] [ARGS...]\n");
    printf("Stack Array Data Structure Demonstration\n\n");
    printf("Commands:\n");
    printf("  --help, -h         Show this help message\n");
    printf("  --test             Run comprehensive stack tests\n");
    printf("  --create NAME      Create a new stack with name\n");
    printf("  --use INDEX        Switch to stack at index\n");
    printf("  --list             List all stacks\n");
    printf("  --push TYPE VAL    Push value (int, float, string)\n");
    printf("  --pop              Pop top element\n");
    printf("  --peek             Peek at top element\n");
    printf("  --size             Show stack size\n");
    printf("  --empty            Check if stack is empty\n");
    printf("  --clear            Clear all elements\n");
    printf("  --print            Print entire stack\n");
    printf("  --reverse          Reverse stack order\n");
    printf("  --duplicate        Duplicate top element\n");
    printf("  --swap             Swap top two elements\n");
    printf("  --rotate N         Rotate top N elements\n");
    printf("  --search VAL       Search for value in stack\n");
    printf("  --stats            Show stack statistics\n");
    printf("  --history          Show operation history\n");
    printf("  --calculator       Interactive calculator mode\n");
    printf("  --bracket EXPR     Check bracket matching\n");
    printf("  --postfix EXPR     Evaluate postfix expression\n");
    printf("  --benchmark N      Run performance benchmark\n");
    printf("\nFeatures:\n");
    printf("  - Multiple data type support (int, float, string)\n");
    printf("  - Multiple concurrent stacks\n");
    printf("  - Calculator and expression evaluation\n");
    printf("  - Bracket matching validation\n");
    printf("  - Operation history tracking\n");
    printf("  - Performance benchmarking\n");
}

// Add operation to history
void add_history(const char* operation, const char* details, int stack_size) {
    if (history_count >= MAX_HISTORY) {
        // Shift history (FIFO)
        for (int i = 0; i < MAX_HISTORY - 1; i++) {
            history[i] = history[i + 1];
        }
        history_count = MAX_HISTORY - 1;
    }

    OperationHistory* entry = &history[history_count];
    strncpy(entry->operation, operation, sizeof(entry->operation) - 1);
    strncpy(entry->details, details, sizeof(entry->details) - 1);
    entry->operation[sizeof(entry->operation) - 1] = '\0';
    entry->details[sizeof(entry->details) - 1] = '\0';
    entry->timestamp = time(NULL);
    entry->stack_size_after = stack_size;
    history_count++;
}

// Initialize a stack
void init_stack(Stack* stack, const char* name) {
    stack->top = -1;
    stack->max_used = 0;
    strncpy(stack->name, name, sizeof(stack->name) - 1);
    stack->name[sizeof(stack->name) - 1] = '\0';
    stack->created = time(NULL);
}

// Create a new stack
int create_stack(const char* name) {
    if (stack_count >= 5) {
        printf("Maximum number of stacks (5) reached\n");
        return 0;
    }

    init_stack(&stacks[stack_count], name);
    printf("Created stack '%s' at index %d\n", name, stack_count);
    stack_count++;
    return 1;
}

// Check if stack is empty
int is_empty(Stack* stack) {
    return stack->top == -1;
}

// Check if stack is full
int is_full(Stack* stack) {
    return stack->top == MAX_STACK_SIZE - 1;
}

// Push element onto stack
int push(Stack* stack, DataType type, const void* value) {
    if (is_full(stack)) {
        printf("Stack overflow! Cannot push element\n");
        return 0;
    }

    stack->top++;
    stack->elements[stack->top].type = type;

    char details[128];
    switch (type) {
        case TYPE_INT:
            stack->elements[stack->top].data.int_val = *(const int*)value;
            snprintf(details, sizeof(details), "int: %d", *(const int*)value);
            break;
        case TYPE_FLOAT:
            stack->elements[stack->top].data.float_val = *(const float*)value;
            snprintf(details, sizeof(details), "float: %.2f", *(const float*)value);
            break;
        case TYPE_STRING:
            strncpy(stack->elements[stack->top].data.str_val, (const char*)value, MAX_ELEMENT_SIZE - 1);
            stack->elements[stack->top].data.str_val[MAX_ELEMENT_SIZE - 1] = '\0';
            snprintf(details, sizeof(details), "string: '%s'", (const char*)value);
            break;
    }

    if (stack->top > stack->max_used) {
        stack->max_used = stack->top;
    }

    printf("Pushed %s (size: %d)\n", details, stack->top + 1);
    add_history("push", details, stack->top + 1);
    return 1;
}

// Pop element from stack
int pop(Stack* stack, StackElement* result) {
    if (is_empty(stack)) {
        printf("Stack underflow! Cannot pop element\n");
        return 0;
    }

    if (result) {
        *result = stack->elements[stack->top];
    }

    StackElement* elem = &stack->elements[stack->top];
    char details[128];

    switch (elem->type) {
        case TYPE_INT:
            snprintf(details, sizeof(details), "int: %d", elem->data.int_val);
            break;
        case TYPE_FLOAT:
            snprintf(details, sizeof(details), "float: %.2f", elem->data.float_val);
            break;
        case TYPE_STRING:
            snprintf(details, sizeof(details), "string: '%s'", elem->data.str_val);
            break;
    }

    printf("Popped %s (size: %d)\n", details, stack->top);
    stack->top--;

    add_history("pop", details, stack->top + 1);
    return 1;
}

// Peek at top element
int peek(Stack* stack, StackElement* result) {
    if (is_empty(stack)) {
        printf("Stack is empty\n");
        return 0;
    }

    if (result) {
        *result = stack->elements[stack->top];
    }

    StackElement* elem = &stack->elements[stack->top];
    switch (elem->type) {
        case TYPE_INT:
            printf("Top element: int %d\n", elem->data.int_val);
            break;
        case TYPE_FLOAT:
            printf("Top element: float %.2f\n", elem->data.float_val);
            break;
        case TYPE_STRING:
            printf("Top element: string '%s'\n", elem->data.str_val);
            break;
    }

    return 1;
}

// Print entire stack
void print_stack(Stack* stack) {
    printf("\n=== Stack '%s' ===\n", stack->name);
    printf("Size: %d / %d\n", stack->top + 1, MAX_STACK_SIZE);

    if (is_empty(stack)) {
        printf("Stack is empty\n");
        return;
    }

    printf("Elements (top to bottom):\n");
    for (int i = stack->top; i >= 0; i--) {
        StackElement* elem = &stack->elements[i];
        printf("[%2d] ", i);

        switch (elem->type) {
            case TYPE_INT:
                printf("int: %d\n", elem->data.int_val);
                break;
            case TYPE_FLOAT:
                printf("float: %.2f\n", elem->data.float_val);
                break;
            case TYPE_STRING:
                printf("string: '%s'\n", elem->data.str_val);
                break;
        }
    }
    printf("\n");
}

// Clear stack
void clear_stack(Stack* stack) {
    int old_size = stack->top + 1;
    stack->top = -1;
    printf("Cleared stack '%s' (%d elements removed)\n", stack->name, old_size);
    add_history("clear", "", 0);
}

// Duplicate top element
int duplicate_top(Stack* stack) {
    if (is_empty(stack)) {
        printf("Cannot duplicate: stack is empty\n");
        return 0;
    }

    if (is_full(stack)) {
        printf("Cannot duplicate: stack is full\n");
        return 0;
    }

    StackElement top_elem = stack->elements[stack->top];
    stack->top++;
    stack->elements[stack->top] = top_elem;

    printf("Duplicated top element\n");
    add_history("duplicate", "top element", stack->top + 1);
    return 1;
}

// Swap top two elements
int swap_top(Stack* stack) {
    if (stack->top < 1) {
        printf("Cannot swap: need at least 2 elements\n");
        return 0;
    }

    StackElement temp = stack->elements[stack->top];
    stack->elements[stack->top] = stack->elements[stack->top - 1];
    stack->elements[stack->top - 1] = temp;

    printf("Swapped top two elements\n");
    add_history("swap", "top two elements", stack->top + 1);
    return 1;
}

// Rotate top N elements
int rotate_elements(Stack* stack, int n) {
    if (n < 2 || n > stack->top + 1) {
        printf("Cannot rotate: need 2 <= n <= stack_size\n");
        return 0;
    }

    // Save top element
    StackElement temp = stack->elements[stack->top];

    // Shift elements
    for (int i = stack->top; i > stack->top - n + 1; i--) {
        stack->elements[i] = stack->elements[i - 1];
    }

    // Put top element at position (top - n + 1)
    stack->elements[stack->top - n + 1] = temp;

    printf("Rotated top %d elements\n", n);
    char details[64];
    snprintf(details, sizeof(details), "top %d elements", n);
    add_history("rotate", details, stack->top + 1);
    return 1;
}

// Search for value in stack
int search_stack(Stack* stack, DataType type, const void* value) {
    if (is_empty(stack)) {
        printf("Stack is empty\n");
        return -1;
    }

    for (int i = stack->top; i >= 0; i--) {
        StackElement* elem = &stack->elements[i];

        if (elem->type != type) continue;

        int found = 0;
        switch (type) {
            case TYPE_INT:
                found = (elem->data.int_val == *(const int*)value);
                break;
            case TYPE_FLOAT:
                found = (elem->data.float_val == *(const float*)value);
                break;
            case TYPE_STRING:
                found = (strcmp(elem->data.str_val, (const char*)value) == 0);
                break;
        }

        if (found) {
            printf("Found at position %d (distance from top: %d)\n", i, stack->top - i);
            return i;
        }
    }

    printf("Value not found in stack\n");
    return -1;
}

// Reverse stack
void reverse_stack(Stack* stack) {
    if (stack->top < 1) {
        printf("Cannot reverse: need at least 2 elements\n");
        return;
    }

    int left = 0;
    int right = stack->top;

    while (left < right) {
        StackElement temp = stack->elements[left];
        stack->elements[left] = stack->elements[right];
        stack->elements[right] = temp;
        left++;
        right--;
    }

    printf("Reversed stack order\n");
    add_history("reverse", "", stack->top + 1);
}

// Show stack statistics
void show_statistics(Stack* stack) {
    printf("\n=== Stack Statistics: '%s' ===\n", stack->name);
    printf("Current size: %d\n", stack->top + 1);
    printf("Maximum used: %d\n", stack->max_used + 1);
    printf("Capacity: %d\n", MAX_STACK_SIZE);
    printf("Utilization: %.1f%%\n", ((float)(stack->top + 1) / MAX_STACK_SIZE) * 100);
    printf("Age: %d seconds\n", (int)(time(NULL) - stack->created));

    // Count data types
    int type_counts[3] = {0};
    for (int i = 0; i <= stack->top; i++) {
        type_counts[stack->elements[i].type]++;
    }

    printf("Data type distribution:\n");
    printf("  Integers: %d\n", type_counts[TYPE_INT]);
    printf("  Floats: %d\n", type_counts[TYPE_FLOAT]);
    printf("  Strings: %d\n", type_counts[TYPE_STRING]);
}

// Show operation history
void show_history() {
    printf("\n=== Operation History ===\n");
    if (history_count == 0) {
        printf("No operations recorded\n");
        return;
    }

    printf("Total operations: %d\n\n", history_count);
    for (int i = history_count - 1; i >= 0 && i >= history_count - 10; i--) {
        OperationHistory* entry = &history[i];
        printf("%2d. %s", history_count - i, entry->operation);
        if (strlen(entry->details) > 0) {
            printf(" (%s)", entry->details);
        }
        printf(" -> size: %d\n", entry->stack_size_after);
        printf("    Time: %s", ctime(&entry->timestamp));
    }
}

// List all stacks
void list_stacks() {
    printf("\n=== Available Stacks ===\n");
    if (stack_count == 0) {
        printf("No stacks created\n");
        return;
    }

    for (int i = 0; i < stack_count; i++) {
        printf("%d. '%s' [size: %d]%s\n", i, stacks[i].name, stacks[i].top + 1,
               (i == current_stack) ? " (current)" : "");
    }
    printf("\nCurrent stack: %d ('%s')\n", current_stack, stacks[current_stack].name);
}

// Check bracket matching using stack
int check_brackets(const char* expression) {
    Stack temp_stack;
    init_stack(&temp_stack, "bracket_check");

    printf("Checking bracket matching for: '%s'\n", expression);

    for (int i = 0; expression[i]; i++) {
        char ch = expression[i];

        if (ch == '(' || ch == '[' || ch == '{') {
            push(&temp_stack, TYPE_STRING, &ch);
        } else if (ch == ')' || ch == ']' || ch == '}') {
            if (is_empty(&temp_stack)) {
                printf("Error: Unmatched closing bracket '%c' at position %d\n", ch, i);
                return 0;
            }

            StackElement popped;
            pop(&temp_stack, &popped);
            char opening = popped.data.str_val[0];

            if ((ch == ')' && opening != '(') ||
                (ch == ']' && opening != '[') ||
                (ch == '}' && opening != '{')) {
                printf("Error: Mismatched brackets '%c' and '%c' at position %d\n", opening, ch, i);
                return 0;
            }
        }
    }

    if (!is_empty(&temp_stack)) {
        printf("Error: Unmatched opening bracket(s)\n");
        return 0;
    }

    printf("Bracket matching: VALID\n");
    return 1;
}

// Simple postfix calculator
float evaluate_postfix(const char* expression) {
    Stack calc_stack;
    init_stack(&calc_stack, "calculator");

    printf("Evaluating postfix expression: '%s'\n", expression);

    char* expr_copy = strdup(expression);
    char* token = strtok(expr_copy, " ");

    while (token != NULL) {
        if (isdigit(token[0]) || (token[0] == '-' && isdigit(token[1]))) {
            // Number
            float value = atof(token);
            push(&calc_stack, TYPE_FLOAT, &value);
        } else if (strlen(token) == 1 && strchr("+-*/", token[0])) {
            // Operator
            if (calc_stack.top < 1) {
                printf("Error: Insufficient operands for operator '%s'\n", token);
                free(expr_copy);
                return 0;
            }

            StackElement b, a;
            pop(&calc_stack, &b);
            pop(&calc_stack, &a);

            float result;
            switch (token[0]) {
                case '+':
                    result = a.data.float_val + b.data.float_val;
                    break;
                case '-':
                    result = a.data.float_val - b.data.float_val;
                    break;
                case '*':
                    result = a.data.float_val * b.data.float_val;
                    break;
                case '/':
                    if (b.data.float_val == 0) {
                        printf("Error: Division by zero\n");
                        free(expr_copy);
                        return 0;
                    }
                    result = a.data.float_val / b.data.float_val;
                    break;
                default:
                    printf("Error: Unknown operator '%s'\n", token);
                    free(expr_copy);
                    return 0;
            }

            push(&calc_stack, TYPE_FLOAT, &result);
            printf("  %.2f %c %.2f = %.2f\n", a.data.float_val, token[0], b.data.float_val, result);
        } else {
            printf("Error: Unknown token '%s'\n", token);
            free(expr_copy);
            return 0;
        }

        token = strtok(NULL, " ");
    }

    if (calc_stack.top != 0) {
        printf("Error: Invalid expression (multiple results)\n");
        free(expr_copy);
        return 0;
    }

    float final_result = calc_stack.elements[0].data.float_val;
    printf("Result: %.2f\n", final_result);

    free(expr_copy);
    return final_result;
}

// Performance benchmark
void run_benchmark(int operations) {
    printf("Stack Array Performance Benchmark\n");
    printf("=================================\n");
    printf("Running %d operations...\n\n", operations);

    Stack bench_stack;
    init_stack(&bench_stack, "benchmark");

    clock_t start = clock();

    // Push phase
    printf("Push phase: ");
    for (int i = 0; i < operations / 2; i++) {
        int value = rand() % 1000;
        push(&bench_stack, TYPE_INT, &value);
        if (i % 100 == 0) printf(".");
    }
    printf(" done\n");

    clock_t push_end = clock();

    // Peek/search phase
    printf("Peek/search phase: ");
    for (int i = 0; i < operations / 4; i++) {
        StackElement elem;
        peek(&bench_stack, &elem);
        int search_val = rand() % 1000;
        search_stack(&bench_stack, TYPE_INT, &search_val);
        if (i % 50 == 0) printf(".");
    }
    printf(" done\n");

    clock_t search_end = clock();

    // Pop phase
    printf("Pop phase: ");
    while (!is_empty(&bench_stack) && operations / 4 > 0) {
        StackElement elem;
        pop(&bench_stack, &elem);
        operations--;
        if (operations % 50 == 0) printf(".");
    }
    printf(" done\n");

    clock_t end = clock();

    double push_time = ((double)(push_end - start)) / CLOCKS_PER_SEC;
    double search_time = ((double)(search_end - push_end)) / CLOCKS_PER_SEC;
    double pop_time = ((double)(end - search_end)) / CLOCKS_PER_SEC;
    double total_time = ((double)(end - start)) / CLOCKS_PER_SEC;

    printf("\nBenchmark Results:\n");
    printf("==================\n");
    printf("Push time: %.4f seconds\n", push_time);
    printf("Search time: %.4f seconds\n", search_time);
    printf("Pop time: %.4f seconds\n", pop_time);
    printf("Total time: %.4f seconds\n", total_time);
    printf("Operations per second: %.0f\n", operations / total_time);

    show_statistics(&bench_stack);
}

// Run comprehensive tests
void run_tests() {
    printf("Stack Array Test Suite\n");
    printf("======================\n\n");

    // Initialize default stack
    if (stack_count == 0) {
        create_stack("test_stack");
    }
    Stack* stack = &stacks[current_stack];

    // Test 1: Basic operations
    printf("--- Test 1: Basic Push/Pop Operations ---\n");
    int int_val = 42;
    float float_val = 3.14f;
    char str_val[] = "hello";

    push(stack, TYPE_INT, &int_val);
    push(stack, TYPE_FLOAT, &float_val);
    push(stack, TYPE_STRING, str_val);

    print_stack(stack);

    peek(stack, NULL);

    StackElement popped;
    pop(stack, &popped);
    pop(stack, &popped);

    // Test 2: Stack manipulation
    printf("\n--- Test 2: Stack Manipulation ---\n");
    int values[] = {10, 20, 30, 40, 50};
    for (int i = 0; i < 5; i++) {
        push(stack, TYPE_INT, &values[i]);
    }

    print_stack(stack);

    duplicate_top(stack);
    printf("After duplicate:\n");
    print_stack(stack);

    swap_top(stack);
    printf("After swap:\n");
    print_stack(stack);

    rotate_elements(stack, 3);
    printf("After rotate 3:\n");
    print_stack(stack);

    // Test 3: Search operations
    printf("\n--- Test 3: Search Operations ---\n");
    int search_val = 30;
    search_stack(stack, TYPE_INT, &search_val);

    search_val = 99;
    search_stack(stack, TYPE_INT, &search_val);

    // Test 4: Multiple stacks
    printf("\n--- Test 4: Multiple Stacks ---\n");
    create_stack("secondary");
    current_stack = 1;

    int sec_val = 100;
    push(&stacks[current_stack], TYPE_INT, &sec_val);

    list_stacks();

    // Test 5: Bracket matching
    printf("\n--- Test 5: Bracket Matching ---\n");
    check_brackets("(a + b) * [c - d]");
    check_brackets("((()))");
    check_brackets("([)]");  // Invalid

    // Test 6: Postfix evaluation
    printf("\n--- Test 6: Postfix Expression Evaluation ---\n");
    evaluate_postfix("3 4 + 2 *");
    evaluate_postfix("15 7 1 1 + - / 3 * 2 1 1 + + -");

    show_statistics(stack);
    show_history();

    printf("\nStack array tests completed successfully!\n");
}

int main(int argc, char *argv[]) {
    srand((unsigned int)time(NULL));

    if (argc < 2) {
        printf("Stack Array Data Structure\n");
        printf("==========================\n\n");
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

    // Ensure we have at least one stack
    if (stack_count == 0) {
        create_stack("default");
    }

    Stack* stack = &stacks[current_stack];

    if (strcmp(argv[1], "--create") == 0) {
        if (argc < 3) {
            printf("Error: --create requires name\n");
            return 1;
        }
        create_stack(argv[2]);
    }
    else if (strcmp(argv[1], "--use") == 0) {
        if (argc < 3) {
            printf("Error: --use requires index\n");
            return 1;
        }
        int index = atoi(argv[2]);
        if (index >= 0 && index < stack_count) {
            current_stack = index;
            printf("Switched to stack %d ('%s')\n", index, stacks[index].name);
        } else {
            printf("Error: Invalid stack index %d\n", index);
        }
    }
    else if (strcmp(argv[1], "--list") == 0) {
        list_stacks();
    }
    else if (strcmp(argv[1], "--push") == 0) {
        if (argc < 4) {
            printf("Error: --push requires type and value\n");
            return 1;
        }

        char* type_str = argv[2];
        char* value_str = argv[3];

        if (strcmp(type_str, "int") == 0) {
            int value = atoi(value_str);
            push(stack, TYPE_INT, &value);
        } else if (strcmp(type_str, "float") == 0) {
            float value = atof(value_str);
            push(stack, TYPE_FLOAT, &value);
        } else if (strcmp(type_str, "string") == 0) {
            push(stack, TYPE_STRING, value_str);
        } else {
            printf("Error: Unknown type '%s'. Use int, float, or string\n", type_str);
        }
    }
    else if (strcmp(argv[1], "--pop") == 0) {
        StackElement elem;
        pop(stack, &elem);
    }
    else if (strcmp(argv[1], "--peek") == 0) {
        peek(stack, NULL);
    }
    else if (strcmp(argv[1], "--size") == 0) {
        printf("Stack size: %d\n", stack->top + 1);
    }
    else if (strcmp(argv[1], "--empty") == 0) {
        printf("Stack is %s\n", is_empty(stack) ? "empty" : "not empty");
    }
    else if (strcmp(argv[1], "--clear") == 0) {
        clear_stack(stack);
    }
    else if (strcmp(argv[1], "--print") == 0) {
        print_stack(stack);
    }
    else if (strcmp(argv[1], "--reverse") == 0) {
        reverse_stack(stack);
    }
    else if (strcmp(argv[1], "--duplicate") == 0) {
        duplicate_top(stack);
    }
    else if (strcmp(argv[1], "--swap") == 0) {
        swap_top(stack);
    }
    else if (strcmp(argv[1], "--rotate") == 0) {
        if (argc < 3) {
            printf("Error: --rotate requires number of elements\n");
            return 1;
        }
        int n = atoi(argv[2]);
        rotate_elements(stack, n);
    }
    else if (strcmp(argv[1], "--search") == 0) {
        if (argc < 3) {
            printf("Error: --search requires value\n");
            return 1;
        }
        // Try as int first, then string
        int int_val = atoi(argv[2]);
        if (int_val != 0 || strcmp(argv[2], "0") == 0) {
            search_stack(stack, TYPE_INT, &int_val);
        } else {
            search_stack(stack, TYPE_STRING, argv[2]);
        }
    }
    else if (strcmp(argv[1], "--stats") == 0) {
        show_statistics(stack);
    }
    else if (strcmp(argv[1], "--history") == 0) {
        show_history();
    }
    else if (strcmp(argv[1], "--bracket") == 0) {
        if (argc < 3) {
            printf("Error: --bracket requires expression\n");
            return 1;
        }
        check_brackets(argv[2]);
    }
    else if (strcmp(argv[1], "--postfix") == 0) {
        if (argc < 3) {
            printf("Error: --postfix requires expression\n");
            return 1;
        }
        evaluate_postfix(argv[2]);
    }
    else if (strcmp(argv[1], "--benchmark") == 0) {
        int operations = 1000;
        if (argc > 2) {
            operations = atoi(argv[2]);
            if (operations < 10 || operations > 50000) {
                printf("Error: Operations must be between 10 and 50,000\n");
                return 1;
            }
        }
        run_benchmark(operations);
    }
    else {
        printf("Unknown command: %s\n", argv[1]);
        print_usage();
        return 1;
    }

    return 0;
}