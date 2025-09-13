#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int binary_search_iterative(int arr[], int size, int target) {
    int left = 0;
    int right = size - 1;
    
    while (left <= right) {
        int mid = left + (right - left) / 2;
        
        if (arr[mid] == target) {
            return mid;
        }
        
        if (arr[mid] < target) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }
    
    return -1;
}

int linear_search(int arr[], int size, int target) {
    for (int i = 0; i < size; i++) {
        if (arr[i] == target) {
            return i;
        }
    }
    return -1;
}

void print_array(int arr[], int size) {
    printf("[");
    for (int i = 0; i < size; i++) {
        printf("%d", arr[i]);
        if (i < size - 1) printf(", ");
    }
    printf("]\n");
}

int compare_performance(int arr[], int size, int target) {
    clock_t start, end;
    double binary_time, linear_time;
    int result;
    
    start = clock();
    for (int i = 0; i < 100000; i++) {
        result = binary_search_iterative(arr, size, target);
    }
    end = clock();
    binary_time = ((double)(end - start)) / CLOCKS_PER_SEC;
    
    start = clock();
    for (int i = 0; i < 100000; i++) {
        result = linear_search(arr, size, target);
    }
    end = clock();
    linear_time = ((double)(end - start)) / CLOCKS_PER_SEC;
    
    printf("Performance comparison (100k iterations):\n");
    printf("Binary search: %.6f seconds\n", binary_time);
    printf("Linear search: %.6f seconds\n", linear_time);
    printf("Binary search is %.2fx faster\n", linear_time / binary_time);
    
    return result;
}

int main() {
    int sorted_data[] = {1, 3, 5, 7, 9, 11, 13, 15, 17, 19, 21, 23, 25, 27, 29, 31, 33, 35, 37, 39};
    int size = sizeof(sorted_data) / sizeof(sorted_data[0]);
    int targets[] = {7, 15, 25, 40, 1, 39};
    int num_targets = sizeof(targets) / sizeof(targets[0]);
    
    printf("Sorted array: ");
    print_array(sorted_data, size);
    printf("Array size: %d elements\n\n", size);
    
    for (int i = 0; i < num_targets; i++) {
        int target = targets[i];
        int result = binary_search_iterative(sorted_data, size, target);
        
        printf("Searching for %d: ", target);
        if (result != -1) {
            printf("Found at index %d\n", result);
        } else {
            printf("Not found\n");
        }
    }
    
    printf("\n");
    compare_performance(sorted_data, size, 25);
    
    return 0;
}