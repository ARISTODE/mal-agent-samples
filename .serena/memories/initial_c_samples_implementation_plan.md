# Initial C Sample Implementation Plan (18 samples)

## Implementation Structure
Each sample will be in: `benign/C/sample_name/`
- `source.c` - The source code
- `binary.exe` - Compiled stripped PE binary  
- `Makefile` - Build instructions

## Sample List

### Algorithm Diversity
1. **quicksort** - Recursive quicksort with function pointers
2. **binary_search** - Iterative binary search with arrays
3. **fibonacci_recursive** - Classic recursive Fibonacci
4. **prime_sieve** - Sieve of Eratosthenes algorithm
5. **string_parser** - Command-line argument parsing
6. **regex_simple** - Basic pattern matching without libraries

### System Calls & API Diversity
7. **file_copy** - File copying using fopen/fread/fwrite
8. **directory_walker** - Directory traversal using opendir/readdir
9. **fork_exec** - Process creation with fork() and exec()
10. **signal_handler** - Signal handling (SIGINT, SIGTERM)
11. **dynamic_allocator** - Custom memory allocator
12. **memory_mapper** - Memory mapping with mmap()
13. **tcp_client** - Basic TCP client socket
14. **udp_server** - UDP server with bind/listen

### Programming Patterns & Language Features
15. **callback_system** - Callback function demonstration
16. **function_table** - Jump table with function pointers
17. **binary_tree** - Binary search tree implementation
18. **stack_array** - Stack using arrays

## Compilation Requirements
- Compile with MinGW for Windows PE format
- Use -s flag to strip symbols
- Target stripped binaries for analysis