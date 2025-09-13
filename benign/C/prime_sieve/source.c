#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <math.h>

void sieve_of_eratosthenes(bool *is_prime, int limit) {
    for (int i = 0; i <= limit; i++) {
        is_prime[i] = true;
    }
    
    is_prime[0] = is_prime[1] = false;
    
    for (int p = 2; p * p <= limit; p++) {
        if (is_prime[p]) {
            for (int i = p * p; i <= limit; i += p) {
                is_prime[i] = false;
            }
        }
    }
}

int count_primes(bool *is_prime, int limit) {
    int count = 0;
    for (int i = 2; i <= limit; i++) {
        if (is_prime[i]) {
            count++;
        }
    }
    return count;
}

void print_primes(bool *is_prime, int limit, int max_display) {
    int count = 0;
    printf("Prime numbers up to %d: ", limit);
    
    for (int i = 2; i <= limit && count < max_display; i++) {
        if (is_prime[i]) {
            printf("%d", i);
            count++;
            if (count < max_display && i < limit) {
                printf(", ");
            }
        }
    }
    
    int total_primes = count_primes(is_prime, limit);
    if (total_primes > max_display) {
        printf("... (%d more)", total_primes - max_display);
    }
    printf("\nTotal primes found: %d\n", total_primes);
}

bool is_prime_trial_division(int n) {
    if (n < 2) return false;
    if (n == 2) return true;
    if (n % 2 == 0) return false;
    
    for (int i = 3; i * i <= n; i += 2) {
        if (n % i == 0) return false;
    }
    return true;
}

void benchmark_algorithms(int limit) {
    clock_t start, end;
    double sieve_time, trial_time;
    
    printf("\nBenchmarking prime finding algorithms (limit: %d):\n", limit);
    
    bool *is_prime = malloc((limit + 1) * sizeof(bool));
    
    start = clock();
    sieve_of_eratosthenes(is_prime, limit);
    int sieve_count = count_primes(is_prime, limit);
    end = clock();
    sieve_time = ((double)(end - start)) / CLOCKS_PER_SEC;
    
    start = clock();
    int trial_count = 0;
    for (int i = 2; i <= limit; i++) {
        if (is_prime_trial_division(i)) {
            trial_count++;
        }
    }
    end = clock();
    trial_time = ((double)(end - start)) / CLOCKS_PER_SEC;
    
    printf("Sieve of Eratosthenes: %d primes in %.6f seconds\n", sieve_count, sieve_time);
    printf("Trial Division:        %d primes in %.6f seconds\n", trial_count, trial_time);
    
    if (trial_time > 0) {
        printf("Sieve is %.2fx faster than trial division\n", trial_time / sieve_time);
    }
    
    free(is_prime);
}

void analyze_prime_gaps(bool *is_prime, int limit) {
    printf("\nPrime gap analysis:\n");
    int prev_prime = 2;
    int max_gap = 0;
    int max_gap_start = 0;
    
    for (int i = 3; i <= limit; i++) {
        if (is_prime[i]) {
            int gap = i - prev_prime;
            if (gap > max_gap) {
                max_gap = gap;
                max_gap_start = prev_prime;
            }
            prev_prime = i;
        }
    }
    
    printf("Largest prime gap: %d (between %d and %d)\n", 
           max_gap, max_gap_start, max_gap_start + max_gap);
}

int main(int argc, char *argv[]) {
    int limit = 1000;
    
    if (argc > 1) {
        limit = atoi(argv[1]);
        if (limit < 2 || limit > 1000000) {
            printf("Please enter a number between 2 and 1,000,000\n");
            return 1;
        }
    }
    
    printf("Sieve of Eratosthenes - Finding primes up to %d\n", limit);
    printf("Memory usage: %.2f KB\n", (limit + 1) * sizeof(bool) / 1024.0);
    
    bool *is_prime = malloc((limit + 1) * sizeof(bool));
    if (!is_prime) {
        printf("Memory allocation failed!\n");
        return 1;
    }
    
    clock_t start = clock();
    sieve_of_eratosthenes(is_prime, limit);
    clock_t end = clock();
    
    double time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Sieve completed in %.6f seconds\n\n", time_taken);
    
    int display_limit = (limit > 100) ? 50 : limit;
    print_primes(is_prime, limit, display_limit);
    
    if (limit >= 100) {
        analyze_prime_gaps(is_prime, limit);
    }
    
    if (limit <= 50000) {
        benchmark_algorithms(limit);
    }
    
    free(is_prime);
    return 0;
}