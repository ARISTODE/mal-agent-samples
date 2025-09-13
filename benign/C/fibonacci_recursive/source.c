#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

unsigned long long fibonacci_recursive(int n) {
    if (n <= 1) {
        return n;
    }
    return fibonacci_recursive(n - 1) + fibonacci_recursive(n - 2);
}

unsigned long long fibonacci_memoized(int n, unsigned long long *memo) {
    if (n <= 1) {
        return n;
    }
    
    if (memo[n] != 0) {
        return memo[n];
    }
    
    memo[n] = fibonacci_memoized(n - 1, memo) + fibonacci_memoized(n - 2, memo);
    return memo[n];
}

unsigned long long fibonacci_iterative(int n) {
    if (n <= 1) {
        return n;
    }
    
    unsigned long long a = 0, b = 1, temp;
    for (int i = 2; i <= n; i++) {
        temp = a + b;
        a = b;
        b = temp;
    }
    return b;
}

void print_fibonacci_sequence(int limit) {
    printf("Fibonacci sequence (first %d numbers):\n", limit);
    for (int i = 0; i < limit; i++) {
        printf("%llu", fibonacci_iterative(i));
        if (i < limit - 1) printf(", ");
    }
    printf("\n\n");
}

void compare_algorithms(int n) {
    clock_t start, end;
    double time_taken;
    unsigned long long result;
    
    printf("Computing Fibonacci(%d) using different methods:\n", n);
    
    if (n <= 35) {
        start = clock();
        result = fibonacci_recursive(n);
        end = clock();
        time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;
        printf("Recursive: %llu (Time: %.6f seconds)\n", result, time_taken);
    } else {
        printf("Recursive: Skipped (too slow for n=%d)\n", n);
    }
    
    unsigned long long *memo = calloc(n + 1, sizeof(unsigned long long));
    start = clock();
    result = fibonacci_memoized(n, memo);
    end = clock();
    time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Memoized:  %llu (Time: %.6f seconds)\n", result, time_taken);
    free(memo);
    
    start = clock();
    result = fibonacci_iterative(n);
    end = clock();
    time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Iterative: %llu (Time: %.6f seconds)\n", result, time_taken);
}

int main(int argc, char *argv[]) {
    int n = 10;
    
    if (argc > 1) {
        n = atoi(argv[1]);
        if (n < 0 || n > 93) {
            printf("Please enter a number between 0 and 93\n");
            return 1;
        }
    }
    
    print_fibonacci_sequence(15);
    
    printf("Performance comparison:\n");
    compare_algorithms(n);
    
    printf("\nRecursive call tree visualization for small values:\n");
    for (int i = 1; i <= 6; i++) {
        printf("fib(%d) = %llu\n", i, fibonacci_recursive(i));
    }
    
    return 0;
}