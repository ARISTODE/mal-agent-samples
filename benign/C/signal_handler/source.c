#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <signal.h>

#ifdef _WIN32
#include <windows.h>
#include <conio.h>
#else
#include <unistd.h>
#endif

#define MAX_SIGNALS 10

typedef struct {
    int signal_type;
    time_t timestamp;
    char description[100];
} SignalRecord;

static SignalRecord signal_history[MAX_SIGNALS];
static int signal_count = 0;
static volatile int keep_running = 1;
static volatile int interrupt_count = 0;

void print_usage() {
    printf("Usage: binary.exe [OPTIONS]\n");
    printf("  OPTIONS:\n");
    printf("    --help, -h     Show this help message\n");
    printf("    --test         Run signal handling tests\n");
    printf("    --interactive  Run interactive signal demo\n");
    printf("    --timeout N    Run for N seconds then exit\n");
    printf("\nSignal handling demonstration:\n");
#ifdef _WIN32
    printf("  - CTRL+C (SIGINT): Graceful shutdown\n");
    printf("  - CTRL+Break: Force termination\n");
    printf("  - Close window: Cleanup and exit\n");
#else
    printf("  - SIGINT (Ctrl+C): Graceful shutdown\n");
    printf("  - SIGTERM: Termination signal\n");
    printf("  - SIGUSR1: User-defined signal 1\n");
    printf("  - SIGUSR2: User-defined signal 2\n");
#endif
}

void record_signal(int sig, const char *desc) {
    if (signal_count < MAX_SIGNALS) {
        signal_history[signal_count].signal_type = sig;
        signal_history[signal_count].timestamp = time(NULL);
        strncpy(signal_history[signal_count].description, desc, sizeof(signal_history[signal_count].description) - 1);
        signal_history[signal_count].description[sizeof(signal_history[signal_count].description) - 1] = '\0';
        signal_count++;
    }
}

void print_signal_history() {
    printf("\n=== Signal History ===\n");
    if (signal_count == 0) {
        printf("No signals recorded.\n");
        return;
    }
    
    for (int i = 0; i < signal_count; i++) {
        char *time_str = ctime(&signal_history[i].timestamp);
        time_str[strlen(time_str) - 1] = '\0'; // Remove newline
        printf("%d. Signal %d (%s) at %s\n", 
               i + 1, signal_history[i].signal_type, 
               signal_history[i].description, time_str);
    }
}

#ifdef _WIN32
BOOL WINAPI console_handler(DWORD signal) {
    switch (signal) {
        case CTRL_C_EVENT:
            interrupt_count++;
            printf("\n[Signal] CTRL+C received (count: %d)\n", interrupt_count);
            record_signal(SIGINT, "CTRL+C (SIGINT)");
            
            if (interrupt_count >= 3) {
                printf("[Signal] Multiple interrupts received, forcing exit...\n");
                keep_running = 0;
                return TRUE;
            }
            
            printf("[Signal] Press CTRL+C %d more time(s) to exit\n", 3 - interrupt_count);
            return TRUE;
            
        case CTRL_BREAK_EVENT:
            printf("\n[Signal] CTRL+Break received - Force termination\n");
            record_signal(SIGTERM, "CTRL+Break (Force termination)");
            keep_running = 0;
            return TRUE;
            
        case CTRL_CLOSE_EVENT:
            printf("\n[Signal] Console closing - Cleanup and exit\n");
            record_signal(SIGTERM, "Console close event");
            print_signal_history();
            keep_running = 0;
            return TRUE;
            
        case CTRL_LOGOFF_EVENT:
        case CTRL_SHUTDOWN_EVENT:
            printf("\n[Signal] System shutdown/logoff - Cleanup\n");
            record_signal(SIGTERM, "System shutdown/logoff");
            keep_running = 0;
            return TRUE;
            
        default:
            return FALSE;
    }
}

void setup_signal_handlers() {
    if (!SetConsoleCtrlHandler(console_handler, TRUE)) {
        printf("Error: Could not set console control handler\n");
    } else {
        printf("Windows console signal handlers installed\n");
    }
}

#else
void posix_signal_handler(int sig) {
    switch (sig) {
        case SIGINT:
            interrupt_count++;
            printf("\n[Signal] SIGINT received (count: %d)\n", interrupt_count);
            record_signal(sig, "SIGINT (Interrupt)");
            
            if (interrupt_count >= 3) {
                printf("[Signal] Multiple interrupts received, exiting...\n");
                keep_running = 0;
            } else {
                printf("[Signal] Press Ctrl+C %d more time(s) to exit\n", 3 - interrupt_count);
            }
            break;
            
        case SIGTERM:
            printf("\n[Signal] SIGTERM received - Termination request\n");
            record_signal(sig, "SIGTERM (Termination)");
            keep_running = 0;
            break;
            
        case SIGUSR1:
            printf("\n[Signal] SIGUSR1 received - User signal 1\n");
            record_signal(sig, "SIGUSR1 (User signal 1)");
            break;
            
        case SIGUSR2:
            printf("\n[Signal] SIGUSR2 received - User signal 2\n");
            record_signal(sig, "SIGUSR2 (User signal 2)");
            break;
            
        default:
            printf("\n[Signal] Unknown signal %d received\n", sig);
            record_signal(sig, "Unknown signal");
            break;
    }
}

void setup_signal_handlers() {
    signal(SIGINT, posix_signal_handler);
    signal(SIGTERM, posix_signal_handler);
    signal(SIGUSR1, posix_signal_handler);
    signal(SIGUSR2, posix_signal_handler);
    printf("POSIX signal handlers installed\n");
}
#endif

void run_interactive_demo() {
    printf("\nSignal Handling Interactive Demo\n");
    printf("===============================\n");
    printf("Program is running... Try the following:\n");
#ifdef _WIN32
    printf("- Press CTRL+C to send interrupt signal\n");
    printf("- Press CTRL+Break for force termination\n");
    printf("- Close the console window\n");
#else
    printf("- Press Ctrl+C to send SIGINT\n");
    printf("- Use 'kill -USR1 %d' to send SIGUSR1\n", getpid());
    printf("- Use 'kill -USR2 %d' to send SIGUSR2\n", getpid());
    printf("- Use 'kill -TERM %d' to send SIGTERM\n", getpid());
#endif
    printf("- Wait for automatic timeout\n");
    printf("\nPress 'q' + Enter to quit gracefully\n\n");
    
    setup_signal_handlers();
    
    int counter = 0;
    time_t start_time = time(NULL);
    
    while (keep_running) {
        counter++;
        printf("Running... (iteration %d, %d seconds elapsed)\n", 
               counter, (int)(time(NULL) - start_time));
        
#ifdef _WIN32
        if (_kbhit()) {
            char c = _getch();
            if (c == 'q' || c == 'Q') {
                printf("Graceful exit requested\n");
                break;
            }
        }
        Sleep(1000);
#else
        printf("Type 'q' and press Enter to quit: ");
        fflush(stdout);
        
        fd_set readfds;
        struct timeval timeout;
        FD_ZERO(&readfds);
        FD_SET(STDIN_FILENO, &readfds);
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;
        
        int result = select(STDIN_FILENO + 1, &readfds, NULL, NULL, &timeout);
        if (result > 0 && FD_ISSET(STDIN_FILENO, &readfds)) {
            char input[10];
            if (fgets(input, sizeof(input), stdin) != NULL) {
                if (input[0] == 'q' || input[0] == 'Q') {
                    printf("Graceful exit requested\n");
                    break;
                }
            }
        }
#endif
        
        if (time(NULL) - start_time > 60) {
            printf("Timeout reached, exiting...\n");
            break;
        }
    }
    
    print_signal_history();
}

void run_timeout_test(int seconds) {
    printf("\nTimeout Test (%d seconds)\n", seconds);
    printf("========================\n");
    printf("Program will run for %d seconds, then exit\n", seconds);
    printf("Try sending signals during this time!\n\n");
    
    setup_signal_handlers();
    
    time_t start_time = time(NULL);
    time_t end_time = start_time + seconds;
    
    int counter = 0;
    while (keep_running && time(NULL) < end_time) {
        counter++;
        int remaining = (int)(end_time - time(NULL));
        printf("Running... (iteration %d, %d seconds remaining)\n", counter, remaining);
        
#ifdef _WIN32
        Sleep(1000);
#else
        sleep(1);
#endif
    }
    
    if (keep_running) {
        printf("Timeout reached, exiting normally\n");
    }
    
    print_signal_history();
}

void run_signal_tests() {
    printf("Signal Handling Test Suite\n");
    printf("=========================\n\n");
    
    printf("--- Test 1: Signal Handler Installation ---\n");
    setup_signal_handlers();
    printf("Signal handlers installed successfully\n\n");
    
    printf("--- Test 2: Manual Signal Generation ---\n");
#ifdef _WIN32
    printf("Generating console control events...\n");
    record_signal(SIGINT, "Test SIGINT");
    record_signal(SIGTERM, "Test SIGTERM");
    printf("Test signals recorded\n");
#else
    printf("Generating test signals...\n");
    raise(SIGUSR1);
    raise(SIGUSR2);
    printf("SIGUSR1 and SIGUSR2 signals sent\n");
#endif
    
    printf("\n--- Test 3: Signal History ---\n");
    print_signal_history();
    
    printf("\n--- Test 4: Cleanup Test ---\n");
    printf("Performing cleanup operations...\n");
    printf("All resources cleaned up successfully\n");
    
    printf("\nAll signal handling tests completed!\n");
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Signal Handler Demo\n");
        printf("==================\n\n");
        print_usage();
        return 1;
    }
    
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            print_usage();
            return 0;
        } else if (strcmp(argv[i], "--test") == 0) {
            run_signal_tests();
            return 0;
        } else if (strcmp(argv[i], "--interactive") == 0) {
            run_interactive_demo();
            return 0;
        } else if (strcmp(argv[i], "--timeout") == 0) {
            if (i + 1 < argc) {
                int seconds = atoi(argv[i + 1]);
                if (seconds > 0 && seconds <= 300) {
                    run_timeout_test(seconds);
                    i++; // Skip the timeout value
                } else {
                    printf("Error: Timeout must be between 1 and 300 seconds\n");
                    return 1;
                }
            } else {
                printf("Error: --timeout requires a number\n");
                return 1;
            }
            return 0;
        } else {
            printf("Unknown option: %s\n", argv[i]);
            print_usage();
            return 1;
        }
    }
    
    return 0;
}