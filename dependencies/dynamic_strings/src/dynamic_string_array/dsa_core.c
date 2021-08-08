/*
dsa_core.c: code for memory management and other basic functions (like copy, contains, etc.)
*/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "ds_str_funcs.h"
#include "dynamic_string_array.h"

//returns pointer to dsa if successful, otherwise NULL
dynamic_string_array * dsa_init(dynamic_string_array * dsa, unsigned size, unsigned str_length) {
    if (!dsa) return NULL;
    if (size == 0) size = 1;
    dsa->str_length = str_length;
    dsa->base_size = size;
    dsa->max_num_elements = size;
    dsa->num_elements = 0;
    dsa->array = malloc(sizeof(char*) * dsa->base_size);
    if (!dsa->array) {
        return NULL;
    }
    return dsa;
}

void dsa_free(dynamic_string_array * dsa) {
    int i;
    if (!dsa) return;
    for (i = 0; i < dsa->num_elements; i++) {
        free(dsa->array[i]);
    }
    free(dsa->array);
    free(dsa);
}

bool dsa_index_is_valid(const dynamic_string_array * dsa, int index) {
    return index == 0 || (index > 0 && index < dsa->num_elements);
}

//returns size of memory allocated for string
size_t dsa_alloc_str_mem(dynamic_string_array * dsa, int index) {
    size_t size = sizeof(char) * dsa->str_length;
    dsa->array[index] = malloc(size);
    if (!dsa->array[index]) {
        return 0;
    }
    return size;
}

void dsa_dealloc_str_mem(dynamic_string_array * dsa, int index) {
    free(dsa->array[index]);
}

bool dsa_is_full(const dynamic_string_array * dsa) {
    return dsa->max_num_elements == dsa->num_elements;
}

bool dsa_would_be_full(const dynamic_string_array * dsa, unsigned length) {
    return dsa->num_elements + length >= dsa->max_num_elements;
}

dynamic_string_array * dsa_enlarge(dynamic_string_array * dsa, unsigned length) {
    dsa->max_num_elements = (dsa->max_num_elements + length) * 2 + 1; // +1 guarantees at least 1 in case of max_num_elements somehow being 0
    size_t size = dsa->max_num_elements * sizeof(char*);
    dsa->array = realloc(dsa->array, size);
    if (!dsa->array) {
        return NULL;
    }
    return dsa;
}

dynamic_string_array * dsa_enlarge_if_needed(dynamic_string_array * dsa, unsigned length) {
    if (dsa_would_be_full(dsa, length)) {
        return dsa_enlarge(dsa, length);
    }
    return dsa;
}

dynamic_string_array * dsa_shrink_array_if_big(dynamic_string_array * dsa) {
    if (dsa->num_elements < (int)(dsa->base_size/2)) {
        dsa->max_num_elements = dsa->base_size;
        dsa->array = realloc(dsa->array, dsa->max_num_elements * sizeof(char*));
        if (!dsa->array) return NULL;
    }
    return dsa;
}

dsa_error_t dsa_shift_strings_right(dynamic_string_array * dsa, int startindex) {
    int i;
    if (dsa_alloc_str_mem(dsa, dsa->num_elements) == 0) return DSA_MALLOC_ERROR;
    for (i = dsa->num_elements; i > startindex; i--) strcpy(dsa->array[i], dsa->array[i-1]);
    return DSA_SUCCESS;
}

dsa_error_t dsa_shift_strings_left(dynamic_string_array * dsa, int startindex) {
    int i;
    for (i = startindex; i < dsa->num_elements-1; i++) strcpy(dsa->array[i], dsa->array[i+1]);
    dsa_dealloc_str_mem(dsa, dsa->num_elements-1);
    return DSA_SUCCESS;
}

dynamic_string_array * dsa_copy(const dynamic_string_array * dsa) {
    dynamic_string_array * copy = malloc(sizeof(*copy));
    if (!copy || !dsa_init(copy, dsa->base_size, dsa->str_length) || dsa_append_multiple(copy, dsa->array, dsa->num_elements) != DSA_SUCCESS) {
        return NULL;
    }
    return copy;
}

bool dsa_contains(const dynamic_string_array * dsa, const char * str) {
    int i;
    for (i = 0; i < dsa->num_elements; i ++) {
        if (strcmp(dsa->array[i], str) == 0) {
            return true;
        }
    }
    return false;
}

void dsa_print(const dynamic_string_array * dsa, const char * separator) {
    int i;
    printf("\n");
    for (i = 0; i < dsa->num_elements; i++) {
        printf("%s%s", separator, dsa->array[i]);
        fflush(stdout);
    }
}

unsigned dsa_count(const dynamic_string_array * dsa, const char * word) {
    int i;
    unsigned count = 0;
    for (i = 0; i < dsa->num_elements; i++) {
        if (strcmp(dsa->array[i], word) == 0) {
            count += 1;
        }
    }
    return count;
}