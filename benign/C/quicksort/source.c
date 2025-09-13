#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef int (*compare_func_t)(const void *a, const void *b);

int compare_int(const void *a, const void *b) {
    int ia = *(const int*)a;
    int ib = *(const int*)b;
    return (ia > ib) - (ia < ib);
}

void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

int partition(int arr[], int low, int high, compare_func_t cmp) {
    int pivot = arr[high];
    int i = (low - 1);
    
    for (int j = low; j <= high - 1; j++) {
        if (cmp(&arr[j], &pivot) <= 0) {
            i++;
            swap(&arr[i], &arr[j]);
        }
    }
    swap(&arr[i + 1], &arr[high]);
    return (i + 1);
}

void quicksort(int arr[], int low, int high, compare_func_t cmp) {
    if (low < high) {
        int pi = partition(arr, low, high, cmp);
        quicksort(arr, low, pi - 1, cmp);
        quicksort(arr, pi + 1, high, cmp);
    }
}

void print_array(int arr[], int size) {
    for (int i = 0; i < size; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

int main() {
    int data[] = {64, 34, 25, 12, 22, 11, 90, 88, 76, 50, 42};
    int n = sizeof(data) / sizeof(data[0]);
    
    printf("Original array: ");
    print_array(data, n);
    
    compare_func_t cmp_func = compare_int;
    
    clock_t start = clock();
    quicksort(data, 0, n - 1, cmp_func);
    clock_t end = clock();
    
    printf("Sorted array: ");
    print_array(data, n);
    
    double cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Time taken: %f seconds\n", cpu_time_used);
    
    return 0;
}