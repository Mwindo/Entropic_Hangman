/*
dsa_sort.c: an implementation of quicksort that switches to insertion sort on small arrays
*/

#include <string.h>
#include "dynamic_string_array.h"

static void swap(char * s1, char * s2) {
    char temp[strlen(s1)+1];
    strcpy(temp, s1);
    strcpy(s1, s2);
    strcpy(s2, temp);    
}

static void insertion_sort(char ** array, int num) {
    int i, j;
    for (i = 1; i < num; i++) {
        j = i;
        while (j > 0 && strcmp(array[j-1], array[j]) > 0) {
            swap(array[j], array[j-1]);
            j -= 1;
        } 
    }
}

static int partition(char ** array, int low, int high) {
    int start = low;
    int end = high;
    while (start < end) {
        while (strcmp(array[start], array[high]) <= 0 && start < high) start += 1; // array[high] not a great partition function
        while (strcmp(array[end], array[high]) > 0) end -= 1;
        if (start < end) {
            swap(array[start], array[end]);
        }
    }
    if (end < high) {
        swap(array[end], array[high]);
    }
    return end;
}

static void quick_insertion_sort(char ** array, int low, int high) {
    if (low >= high) { //nothing to sort
        return;
    }
    if (high - low < 15) { //switch to insertion sort for small arrays
        insertion_sort(array, high - low + 1);
        return;
    }
    else {
        int p = partition(array, low, high);
        quick_insertion_sort(array, low, p-1);
        quick_insertion_sort(array, p+1, high);
    }
}

void dsa_sort(dynamic_string_array * dsa) {
    quick_insertion_sort(dsa->array, 0, dsa->num_elements-1);
}