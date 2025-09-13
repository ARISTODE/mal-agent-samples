#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdarg.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <signal.h>
#endif

#define MAX_CALLBACKS 10
#define MAX_MESSAGE_LEN 256
#define MAX_EVENT_TYPES 5

// Event types for the callback system
typedef enum {
    EVENT_INFO,
    EVENT_WARNING,
    EVENT_ERROR,
    EVENT_TIMER,
    EVENT_CUSTOM
} EventType;

// Callback function signature
typedef void (*CallbackFunction)(EventType event, const char* message, void* user_data);

// Callback registration structure
typedef struct {
    CallbackFunction function;
    EventType event_type;
    void* user_data;
    char name[64];
    int enabled;
    int call_count;
    time_t last_called;
} CallbackEntry;

// Global callback registry
static CallbackEntry callback_registry[MAX_CALLBACKS];
static int callback_count = 0;
static int system_running = 0;

// Event statistics
typedef struct {
    int event_counts[MAX_EVENT_TYPES];
    int total_callbacks_fired;
    time_t start_time;
} EventStats;

static EventStats stats = {0};

void print_usage() {
    printf("Usage: binary.exe [OPTIONS]\n");
    printf("Callback System Demonstration\n\n");
    printf("Options:\n");
    printf("  --help, -h     Show this help message\n");
    printf("  --test         Run comprehensive callback tests\n");
    printf("  --demo         Run interactive demonstration\n");
    printf("  --timer N      Run timer callback demo for N seconds\n");
    printf("  --stats        Show callback statistics\n");
    printf("\nFeatures:\n");
    printf("  - Function pointer callbacks\n");
    printf("  - Event-based callback system\n");
    printf("  - Callback registration and management\n");
    printf("  - Statistics tracking\n");
    printf("  - Timer-based callbacks\n");
}

const char* event_type_to_string(EventType event) {
    switch (event) {
        case EVENT_INFO:    return "INFO";
        case EVENT_WARNING: return "WARNING";
        case EVENT_ERROR:   return "ERROR";
        case EVENT_TIMER:   return "TIMER";
        case EVENT_CUSTOM:  return "CUSTOM";
        default:            return "UNKNOWN";
    }
}

// Register a callback function
int register_callback(const char* name, CallbackFunction func, EventType event_type, void* user_data) {
    if (callback_count >= MAX_CALLBACKS) {
        printf("[CALLBACK] Registry full, cannot register '%s'\n", name);
        return 0;
    }

    CallbackEntry* entry = &callback_registry[callback_count];
    entry->function = func;
    entry->event_type = event_type;
    entry->user_data = user_data;
    strncpy(entry->name, name, sizeof(entry->name) - 1);
    entry->name[sizeof(entry->name) - 1] = '\0';
    entry->enabled = 1;
    entry->call_count = 0;
    entry->last_called = 0;

    printf("[CALLBACK] Registered '%s' for %s events\n", name, event_type_to_string(event_type));
    callback_count++;
    return 1;
}

// Unregister a callback by name
int unregister_callback(const char* name) {
    for (int i = 0; i < callback_count; i++) {
        if (strcmp(callback_registry[i].name, name) == 0) {
            // Shift remaining callbacks down
            for (int j = i; j < callback_count - 1; j++) {
                callback_registry[j] = callback_registry[j + 1];
            }
            callback_count--;
            printf("[CALLBACK] Unregistered '%s'\n", name);
            return 1;
        }
    }
    printf("[CALLBACK] Callback '%s' not found\n", name);
    return 0;
}

// Enable/disable a callback
int toggle_callback(const char* name, int enabled) {
    for (int i = 0; i < callback_count; i++) {
        if (strcmp(callback_registry[i].name, name) == 0) {
            callback_registry[i].enabled = enabled;
            printf("[CALLBACK] %s '%s'\n", enabled ? "Enabled" : "Disabled", name);
            return 1;
        }
    }
    return 0;
}

// Fire callbacks for a specific event type
void fire_callbacks(EventType event_type, const char* message) {
    printf("[EVENT] Firing %s: %s\n", event_type_to_string(event_type), message);

    stats.event_counts[event_type]++;
    time_t now = time(NULL);

    for (int i = 0; i < callback_count; i++) {
        CallbackEntry* entry = &callback_registry[i];

        if (entry->enabled && entry->event_type == event_type) {
            printf("[CALLBACK] Calling '%s'\n", entry->name);
            entry->function(event_type, message, entry->user_data);
            entry->call_count++;
            entry->last_called = now;
            stats.total_callbacks_fired++;
        }
    }
}

// Sample callback functions
void info_logger_callback(EventType event, const char* message, void* user_data) {
    FILE* log_file = (FILE*)user_data;
    if (log_file) {
        fprintf(log_file, "[INFO] %s\n", message);
        fflush(log_file);
    }
    printf("  -> INFO Logger: Logged message to file\n");
}

void console_printer_callback(EventType event, const char* message, void* user_data) {
    char* prefix = (char*)user_data;
    printf("  -> Console: %s%s\n", prefix ? prefix : "", message);
}

void error_counter_callback(EventType event, const char* message, void* user_data) {
    int* error_count = (int*)user_data;
    (*error_count)++;
    printf("  -> Error Counter: Total errors = %d\n", *error_count);
}

void warning_beep_callback(EventType event, const char* message, void* user_data) {
    printf("  -> Warning Beep: *BEEP* Warning detected!\n");
    // On Windows, we could use Beep() function
#ifdef _WIN32
    // Beep(750, 300);  // Frequency, Duration - commented out to avoid noise
#endif
}

void timer_callback(EventType event, const char* message, void* user_data) {
    static int tick_count = 0;
    tick_count++;
    printf("  -> Timer: Tick %d - %s\n", tick_count, message);
}

void custom_validator_callback(EventType event, const char* message, void* user_data) {
    if (strstr(message, "test") != NULL) {
        printf("  -> Validator: Message contains 'test' - validation passed\n");
    } else {
        printf("  -> Validator: Message validation failed\n");
    }
}

// List all registered callbacks
void list_callbacks() {
    printf("\n=== Registered Callbacks ===\n");
    if (callback_count == 0) {
        printf("No callbacks registered\n");
        return;
    }

    for (int i = 0; i < callback_count; i++) {
        CallbackEntry* entry = &callback_registry[i];
        printf("%d. %s\n", i + 1, entry->name);
        printf("   Event Type: %s\n", event_type_to_string(entry->event_type));
        printf("   Status: %s\n", entry->enabled ? "Enabled" : "Disabled");
        printf("   Call Count: %d\n", entry->call_count);
        if (entry->last_called > 0) {
            printf("   Last Called: %s", ctime(&entry->last_called));
        } else {
            printf("   Last Called: Never\n");
        }
        printf("\n");
    }
}

// Print event statistics
void print_statistics() {
    printf("\n=== Callback System Statistics ===\n");
    printf("System uptime: %d seconds\n", (int)(time(NULL) - stats.start_time));
    printf("Total callbacks fired: %d\n", stats.total_callbacks_fired);
    printf("Registered callbacks: %d\n", callback_count);
    printf("\nEvent counts:\n");
    for (int i = 0; i < MAX_EVENT_TYPES; i++) {
        if (stats.event_counts[i] > 0) {
            printf("  %s: %d\n", event_type_to_string((EventType)i), stats.event_counts[i]);
        }
    }
}

// Run comprehensive callback tests
void run_callback_tests() {
    printf("Callback System Test Suite\n");
    printf("==========================\n\n");

    // Initialize statistics
    stats.start_time = time(NULL);

    // Test 1: Basic callback registration
    printf("--- Test 1: Callback Registration ---\n");
    FILE* log_file = fopen("callback_test.log", "w");
    char console_prefix[] = "[CONSOLE] ";
    int error_count = 0;

    register_callback("info_logger", info_logger_callback, EVENT_INFO, log_file);
    register_callback("console_printer", console_printer_callback, EVENT_WARNING, console_prefix);
    register_callback("error_counter", error_counter_callback, EVENT_ERROR, &error_count);
    register_callback("warning_beep", warning_beep_callback, EVENT_WARNING, NULL);
    register_callback("validator", custom_validator_callback, EVENT_CUSTOM, NULL);

    list_callbacks();

    // Test 2: Fire different types of events
    printf("\n--- Test 2: Event Firing ---\n");
    fire_callbacks(EVENT_INFO, "System initialized successfully");
    fire_callbacks(EVENT_WARNING, "Low memory warning");
    fire_callbacks(EVENT_ERROR, "Failed to open configuration file");
    fire_callbacks(EVENT_CUSTOM, "Custom test event validation");
    fire_callbacks(EVENT_CUSTOM, "Another custom event without test");

    // Test 3: Callback management
    printf("\n--- Test 3: Callback Management ---\n");
    toggle_callback("warning_beep", 0);  // Disable
    fire_callbacks(EVENT_WARNING, "This warning should not beep");

    toggle_callback("warning_beep", 1);  // Re-enable
    fire_callbacks(EVENT_WARNING, "This warning should beep again");

    // Test 4: Cleanup and unregistration
    printf("\n--- Test 4: Cleanup ---\n");
    unregister_callback("warning_beep");
    fire_callbacks(EVENT_WARNING, "Warning with beep callback removed");

    list_callbacks();
    print_statistics();

    // Clean up resources
    if (log_file) {
        fclose(log_file);
        printf("\nLog file 'callback_test.log' created\n");
    }

    // Clean up remaining callbacks
    printf("\n--- Cleanup ---\n");
    while (callback_count > 0) {
        unregister_callback(callback_registry[0].name);
    }

    printf("Callback system test completed successfully!\n");
}

// Run timer callback demonstration
void run_timer_demo(int seconds) {
    printf("Timer Callback Demo (%d seconds)\n", seconds);
    printf("================================\n\n");

    stats.start_time = time(NULL);
    register_callback("timer", timer_callback, EVENT_TIMER, NULL);

    printf("Starting timer callbacks...\n");
    for (int i = 0; i < seconds; i++) {
        char timer_msg[64];
        snprintf(timer_msg, sizeof(timer_msg), "Second %d of %d", i + 1, seconds);
        fire_callbacks(EVENT_TIMER, timer_msg);

#ifdef _WIN32
        Sleep(1000);
#else
        sleep(1);
#endif
    }

    printf("\nTimer demo completed!\n");
    print_statistics();

    unregister_callback("timer");
}

// Interactive demonstration
void run_interactive_demo() {
    printf("Interactive Callback System Demo\n");
    printf("================================\n\n");

    stats.start_time = time(NULL);

    // Set up some callbacks
    char console_prefix[] = "[DEMO] ";
    int error_count = 0;

    register_callback("demo_console", console_printer_callback, EVENT_INFO, console_prefix);
    register_callback("demo_errors", error_counter_callback, EVENT_ERROR, &error_count);
    register_callback("demo_validator", custom_validator_callback, EVENT_CUSTOM, NULL);

    printf("Demo callbacks registered. Simulating various events...\n\n");

    // Simulate a sequence of events
    fire_callbacks(EVENT_INFO, "Demo started");
    fire_callbacks(EVENT_INFO, "Processing user request");
    fire_callbacks(EVENT_ERROR, "Invalid input detected");
    fire_callbacks(EVENT_CUSTOM, "test data validation");
    fire_callbacks(EVENT_INFO, "Recovery attempt successful");
    fire_callbacks(EVENT_ERROR, "Timeout occurred");
    fire_callbacks(EVENT_CUSTOM, "final cleanup phase");
    fire_callbacks(EVENT_INFO, "Demo completed");

    printf("\n");
    list_callbacks();
    print_statistics();

    // Cleanup
    while (callback_count > 0) {
        unregister_callback(callback_registry[0].name);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Callback System Manager\n");
        printf("======================\n\n");
        print_usage();
        return 1;
    }

    if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0) {
        print_usage();
        return 0;
    }

    if (strcmp(argv[1], "--test") == 0) {
        run_callback_tests();
        return 0;
    }

    if (strcmp(argv[1], "--demo") == 0) {
        run_interactive_demo();
        return 0;
    }

    if (strcmp(argv[1], "--timer") == 0) {
        int seconds = 5;  // default
        if (argc > 2) {
            seconds = atoi(argv[2]);
            if (seconds < 1 || seconds > 60) {
                printf("Error: Timer duration must be between 1 and 60 seconds\n");
                return 1;
            }
        }
        run_timer_demo(seconds);
        return 0;
    }

    if (strcmp(argv[1], "--stats") == 0) {
        printf("No active callback system to show statistics for.\n");
        printf("Run --test, --demo, or --timer first.\n");
        return 0;
    }

    printf("Unknown option: %s\n", argv[1]);
    print_usage();
    return 1;
}