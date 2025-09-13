# Comprehensive C Sample Programs for Binary Analysis

## Algorithm Diversity

### Sorting & Searching
1. **quicksort** - Recursive quicksort with function pointers
2. **binary_search** - Iterative binary search with arrays
3. **hash_table** - Simple hash table with collision handling
4. **linked_list** - Dynamic linked list operations

### Mathematical
5. **fibonacci_recursive** - Classic recursive Fibonacci
6. **prime_sieve** - Sieve of Eratosthenes algorithm
7. **matrix_multiply** - Matrix multiplication with nested loops
8. **gcd_euclidean** - Euclidean algorithm for GCD

### String Processing
9. **string_parser** - Command-line argument parsing
10. **regex_simple** - Basic pattern matching without libraries

## System Calls & API Diversity

### File I/O
11. **file_copy** - File copying using fopen/fread/fwrite
12. **directory_walker** - Directory traversal using opendir/readdir
13. **file_permissions** - File permission checking with stat()

### Process Management
14. **fork_exec** - Process creation with fork() and exec()
15. **signal_handler** - Signal handling (SIGINT, SIGTERM)
16. **pipe_communication** - Inter-process communication via pipes

### Memory Management
17. **dynamic_allocator** - Custom memory allocator
18. **memory_mapper** - Memory mapping with mmap()
19. **shared_memory** - Shared memory segments

### Network
20. **tcp_client** - Basic TCP client socket
21. **udp_server** - UDP server with bind/listen
22. **http_request** - Simple HTTP request builder

## Programming Patterns & Language Features

### Function Pointers
23. **callback_system** - Callback function demonstration
24. **function_table** - Jump table with function pointers
25. **state_machine** - Finite state machine using function pointers

### Data Structures
26. **binary_tree** - Binary search tree implementation
27. **stack_array** - Stack using arrays
28. **queue_circular** - Circular queue implementation

### Advanced C Features
29. **varargs_function** - Variable arguments with stdarg.h
30. **macro_expansion** - Complex macro definitions and usage
31. **inline_assembly** - Inline assembly snippets
32. **bitwise_operations** - Bit manipulation algorithms

### Memory Patterns
33. **buffer_operations** - Buffer copying with bounds checking
34. **pointer_arithmetic** - Complex pointer manipulations
35. **struct_packing** - Structure padding and alignment

## Control Flow Variations
36. **goto_cleanup** - Proper goto usage for cleanup
37. **nested_loops** - Deep nesting patterns
38. **switch_table** - Large switch statements
39. **recursive_descent** - Recursive descent parser

## Error Handling & Edge Cases
40. **error_propagation** - Error code propagation patterns
41. **null_pointer_checks** - Defensive programming
42. **buffer_overflow_safe** - Safe string operations

Each sample will be in `benign/C/sample_name/` with:
- `source.c` - The source code
- `binary.exe` - Compiled stripped PE binary
- `Makefile` - Build instructions