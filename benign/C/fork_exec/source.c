#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef _WIN32
#include <windows.h>
#include <process.h>
#else
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#endif

#define MAX_CMD_LEN 1024
#define MAX_PROCESSES 10

typedef struct {
    int pid;
    char command[MAX_CMD_LEN];
    time_t start_time;
    time_t end_time;
    int exit_code;
    int completed;
} ProcessInfo;

void print_usage() {
    printf("Usage: binary.exe [COMMAND] [ARGS...]\n");
    printf("  COMMAND: Command to execute\n");
    printf("  ARGS:    Arguments for the command\n");
    printf("\nOptions:\n");
    printf("  --help, -h     Show this help message\n");
    printf("  --test         Run built-in process tests\n");
    printf("  --multiple N   Launch N processes in parallel\n");
    printf("\nExamples:\n");
#ifdef _WIN32
    printf("  binary.exe dir\n");
    printf("  binary.exe echo \"Hello World\"\n");
    printf("  binary.exe ping -n 3 127.0.0.1\n");
#else
    printf("  binary.exe ls -la\n");
    printf("  binary.exe echo \"Hello World\"\n");
    printf("  binary.exe ping -c 3 127.0.0.1\n");
#endif
}

#ifdef _WIN32
int execute_process(const char *command, ProcessInfo *proc_info) {
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    char cmd_line[MAX_CMD_LEN];
    // Use cmd.exe /C to execute shell commands and built-ins
    snprintf(cmd_line, MAX_CMD_LEN, "cmd.exe /C %s", command);
    cmd_line[MAX_CMD_LEN - 1] = '\0';

    printf("Executing: %s\n", command);
    proc_info->start_time = time(NULL);
    strncpy(proc_info->command, command, MAX_CMD_LEN - 1);
    proc_info->command[MAX_CMD_LEN - 1] = '\0';

    if (!CreateProcessA(NULL, cmd_line, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        printf("CreateProcess failed (%ld).\n", GetLastError());
        return 0;
    }
    
    proc_info->pid = pi.dwProcessId;
    printf("Process started with PID: %d\n", proc_info->pid);
    
    WaitForSingleObject(pi.hProcess, INFINITE);
    
    DWORD exit_code;
    if (GetExitCodeProcess(pi.hProcess, &exit_code)) {
        proc_info->exit_code = exit_code;
    } else {
        proc_info->exit_code = -1;
    }
    
    proc_info->end_time = time(NULL);
    proc_info->completed = 1;
    
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    
    return 1;
}

int execute_process_async(const char *command, ProcessInfo *proc_info) {
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    char cmd_line[MAX_CMD_LEN];
    // Use cmd.exe /C to execute shell commands and built-ins
    snprintf(cmd_line, MAX_CMD_LEN, "cmd.exe /C %s", command);
    cmd_line[MAX_CMD_LEN - 1] = '\0';

    proc_info->start_time = time(NULL);
    strncpy(proc_info->command, command, MAX_CMD_LEN - 1);
    proc_info->command[MAX_CMD_LEN - 1] = '\0';
    proc_info->completed = 0;

    if (!CreateProcessA(NULL, cmd_line, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        printf("CreateProcess failed (%ld).\n", GetLastError());
        return 0;
    }
    
    proc_info->pid = pi.dwProcessId;
    
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    
    return 1;
}

#else
int execute_process(const char *command, ProcessInfo *proc_info) {
    pid_t pid;
    int status;
    
    printf("Executing: %s\n", command);
    proc_info->start_time = time(NULL);
    strncpy(proc_info->command, command, MAX_CMD_LEN - 1);
    proc_info->command[MAX_CMD_LEN - 1] = '\0';
    
    pid = fork();
    
    if (pid == 0) {
        // Child process
        char *args[64];
        char *token;
        char cmd_copy[MAX_CMD_LEN];
        int argc = 0;
        
        strncpy(cmd_copy, command, MAX_CMD_LEN - 1);
        cmd_copy[MAX_CMD_LEN - 1] = '\0';
        
        token = strtok(cmd_copy, " ");
        while (token != NULL && argc < 63) {
            args[argc++] = token;
            token = strtok(NULL, " ");
        }
        args[argc] = NULL;
        
        if (execvp(args[0], args) == -1) {
            perror("execvp failed");
            exit(1);
        }
    } else if (pid > 0) {
        // Parent process
        proc_info->pid = pid;
        printf("Process started with PID: %d\n", proc_info->pid);
        
        waitpid(pid, &status, 0);
        
        if (WIFEXITED(status)) {
            proc_info->exit_code = WEXITSTATUS(status);
        } else {
            proc_info->exit_code = -1;
        }
        
        proc_info->end_time = time(NULL);
        proc_info->completed = 1;
        
        return 1;
    } else {
        perror("fork failed");
        return 0;
    }
    
    return 1;
}

int execute_process_async(const char *command, ProcessInfo *proc_info) {
    pid_t pid;
    
    proc_info->start_time = time(NULL);
    strncpy(proc_info->command, command, MAX_CMD_LEN - 1);
    proc_info->command[MAX_CMD_LEN - 1] = '\0';
    proc_info->completed = 0;
    
    pid = fork();
    
    if (pid == 0) {
        // Child process
        char *args[64];
        char *token;
        char cmd_copy[MAX_CMD_LEN];
        int argc = 0;
        
        strncpy(cmd_copy, command, MAX_CMD_LEN - 1);
        cmd_copy[MAX_CMD_LEN - 1] = '\0';
        
        token = strtok(cmd_copy, " ");
        while (token != NULL && argc < 63) {
            args[argc++] = token;
            token = strtok(NULL, " ");
        }
        args[argc] = NULL;
        
        if (execvp(args[0], args) == -1) {
            perror("execvp failed");
            exit(1);
        }
    } else if (pid > 0) {
        // Parent process
        proc_info->pid = pid;
        return 1;
    } else {
        perror("fork failed");
        return 0;
    }
    
    return 1;
}
#endif

void print_process_info(const ProcessInfo *proc_info) {
    printf("\n=== Process Information ===\n");
    printf("Command:    %s\n", proc_info->command);
    printf("PID:        %d\n", proc_info->pid);
    printf("Start time: %s", ctime(&proc_info->start_time));
    
    if (proc_info->completed) {
        printf("End time:   %s", ctime(&proc_info->end_time));
        printf("Duration:   %d seconds\n", (int)(proc_info->end_time - proc_info->start_time));
        printf("Exit code:  %d\n", proc_info->exit_code);
        printf("Status:     %s\n", proc_info->exit_code == 0 ? "SUCCESS" : "FAILED");
    } else {
        printf("Status:     RUNNING\n");
    }
}

void run_basic_tests() {
    printf("Process Execution Test Suite\n");
    printf("===========================\n\n");
    
    const char *test_commands[] = {
#ifdef _WIN32
        "echo Hello from Windows process!",
        "dir /b",
        "ping -n 2 127.0.0.1",
#else
        "echo Hello from Unix process!",
        "ls -la",
        "ping -c 2 127.0.0.1",
#endif
        "echo Process test complete"
    };
    
    const int num_tests = sizeof(test_commands) / sizeof(test_commands[0]);
    
    for (int i = 0; i < num_tests; i++) {
        printf("--- Test %d ---\n", i + 1);
        
        ProcessInfo proc_info = {0};
        
        if (execute_process(test_commands[i], &proc_info)) {
            print_process_info(&proc_info);
        } else {
            printf("Failed to execute command: %s\n", test_commands[i]);
        }
        
        printf("\n");
    }
}

void run_parallel_tests(int num_processes) {
    printf("Parallel Process Test\n");
    printf("====================\n");
    printf("Launching %d processes in parallel...\n\n", num_processes);
    
    ProcessInfo processes[MAX_PROCESSES];
    
    if (num_processes > MAX_PROCESSES) {
        num_processes = MAX_PROCESSES;
        printf("Limited to %d processes\n", MAX_PROCESSES);
    }
    
    // Launch all processes
    for (int i = 0; i < num_processes; i++) {
        char command[MAX_CMD_LEN];
#ifdef _WIN32
        snprintf(command, sizeof(command), "echo Process %d started && ping -n 3 127.0.0.1 >nul && echo Process %d finished", i + 1, i + 1);
#else
        snprintf(command, sizeof(command), "echo Process %d started && sleep 2 && echo Process %d finished", i + 1, i + 1);
#endif
        
        printf("Starting process %d: %s\n", i + 1, command);
        if (!execute_process_async(command, &processes[i])) {
            printf("Failed to start process %d\n", i + 1);
            continue;
        }
        printf("Process %d started with PID: %d\n", i + 1, processes[i].pid);
    }
    
    printf("\nAll processes started. Waiting for completion...\n");
    
#ifdef _WIN32
    // On Windows, we'll just wait a bit since async monitoring is complex
    Sleep(5000);
    printf("Parallel test completed (processes may still be running)\n");
#else
    // On Unix, wait for all child processes
    int status;
    pid_t wpid;
    while ((wpid = wait(&status)) > 0) {
        for (int i = 0; i < num_processes; i++) {
            if (processes[i].pid == wpid) {
                processes[i].end_time = time(NULL);
                processes[i].completed = 1;
                if (WIFEXITED(status)) {
                    processes[i].exit_code = WEXITSTATUS(status);
                } else {
                    processes[i].exit_code = -1;
                }
                printf("Process %d (PID %d) completed with exit code %d\n", 
                       i + 1, wpid, processes[i].exit_code);
                break;
            }
        }
    }
#endif
    
    printf("\n=== Parallel Test Results ===\n");
    for (int i = 0; i < num_processes; i++) {
        printf("Process %d: ", i + 1);
        if (processes[i].completed) {
            printf("Completed in %d seconds (exit code: %d)\n", 
                   (int)(processes[i].end_time - processes[i].start_time), 
                   processes[i].exit_code);
        } else {
            printf("Status unknown (may still be running)\n");
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Process Execution and Management\n");
        printf("===============================\n\n");
        print_usage();
        return 1;
    }
    
    if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0) {
        print_usage();
        return 0;
    }
    
    if (strcmp(argv[1], "--test") == 0) {
        run_basic_tests();
        return 0;
    }
    
    if (strcmp(argv[1], "--multiple") == 0) {
        if (argc < 3) {
            printf("Error: --multiple requires a number\n");
            return 1;
        }
        int num_processes = atoi(argv[2]);
        if (num_processes < 1 || num_processes > MAX_PROCESSES) {
            printf("Error: Number of processes must be between 1 and %d\n", MAX_PROCESSES);
            return 1;
        }
        run_parallel_tests(num_processes);
        return 0;
    }
    
    // Build command string from arguments
    char command[MAX_CMD_LEN] = "";
    for (int i = 1; i < argc; i++) {
        if (i > 1) strcat(command, " ");
        if (strlen(command) + strlen(argv[i]) + 1 < MAX_CMD_LEN) {
            strcat(command, argv[i]);
        }
    }
    
    ProcessInfo proc_info = {0};
    
    printf("Single Process Execution\n");
    printf("========================\n");
    
    if (execute_process(command, &proc_info)) {
        print_process_info(&proc_info);
        return proc_info.exit_code;
    } else {
        printf("Failed to execute command: %s\n", command);
        return 1;
    }
}