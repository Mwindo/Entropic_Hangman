/*
dsa_add_remove.c: code for appending and removing elements
*/

#include "dynamic_string_array.h"
#include <string.h>
#include "ds_str_funcs.h"

dsa_error_t dsa_append(dynamic_string_array * dsa, const char * str) {
    if (!dsa_enlarge_if_needed(dsa, 1)) return DSA_REALLOC_ERROR;
    if (!dsa_alloc_str_mem(dsa, dsa->num_elements)) return DSA_MALLOC_ERROR;
    strcpy(dsa->array[dsa->num_elements], str);
    dsa->num_elements += 1;
    return DSA_SUCCESS;
}

dsa_error_t dsa_insert(dynamic_string_array * dsa, int index, const char *  str) {
    if (!dsa_index_is_valid(dsa, index)) return DSA_INDEX_ERROR;
    if (!dsa_enlarge_if_needed(dsa, 1)) return DSA_REALLOC_ERROR;
    dsa_error_t derror = dsa_shift_strings_right(dsa, index);
    if (derror != DSA_SUCCESS) return derror;
    strcpy(dsa->array[index], str);
    dsa->num_elements += 1;
    return DSA_SUCCESS;
}

dsa_error_t dsa_remove_at_index(dynamic_string_array * dsa, int index) {
    if (!dsa_index_is_valid(dsa, index)) return DSA_INDEX_ERROR;
    dsa_shift_strings_left(dsa, index);
    dsa->num_elements -= 1;
    if (!dsa_shrink_array_if_big(dsa)) return DSA_MALLOC_ERROR;
    return DSA_SUCCESS;
}

// removes first instance
dsa_error_t dsa_remove(dynamic_string_array * dsa, const char * str) {
    int i;
    for (i = 0; i < dsa->num_elements; i ++) {
        if (strcmp(dsa->array[i], str) == 0) return dsa_remove_at_index(dsa, i);
    }
    return DSA_SUCCESS;
}

// slightly more efficient than calling append_string multiple times since it checks/reallocs dsa->strings once at most
dsa_error_t dsa_append_multiple(dynamic_string_array * dsa, char ** strings, unsigned length) {
    if (length < 1) return DSA_SUCCESS; // nothing to append
    if (!dsa_enlarge_if_needed(dsa, length)) return DSA_REALLOC_ERROR;
    int i;
    for (i = 0; i < length; i++) {
        if (dsa_alloc_str_mem(dsa, dsa->num_elements) == 0) return DSA_MALLOC_ERROR;
        strcpy(dsa->array[dsa->num_elements], strings[i]);
        dsa->num_elements += 1;
    }
    return DSA_SUCCESS;
}

// more efficient than calling insert_string multiple times since it shifts/allocates/copies all at once
dsa_error_t dsa_insert_multiple(dynamic_string_array * dsa, int index, char ** strings, unsigned length) {
    if (length < 1) return DSA_SUCCESS; // nothing to insert
    int i;
    int affected = dsa->num_elements - index;
    if (!dsa_enlarge_if_needed(dsa, length)) return DSA_REALLOC_ERROR;
    for (i = affected; i > 0; i--) { // all elements after the index need to have their data copied forward
        if (i + index + length > dsa->num_elements) {
            if (dsa_alloc_str_mem(dsa, i + index + length- 1) == 0) return DSA_MALLOC_ERROR;
        }
        strcpy(dsa->array[i + index + length- 1], dsa->array[i+index-1]);            
    }
    for (i = 0; i < length; i++) { // all elements from index to index + length - 1 need to have their data inserted
        if (index + i >= dsa->num_elements) {
            if (dsa_alloc_str_mem(dsa, index + i) == 0) return DSA_MALLOC_ERROR;
        }
        strcpy(dsa->array[index + i], strings[i]); 
    }
    dsa->num_elements += length;
    return DSA_SUCCESS;
}

// to do this more efficiently is somewhat complicated
// could do a pointer with offset for guaranteed O(n+length) (recopy array, deallocate unused tail)
// but this is not efficient for removing the last two strings, for example, which is O(1)
dsa_error_t dsa_remove_at_indices(dynamic_string_array * dsa, const int * indices, unsigned length) {
    if (length < 1) return DSA_SUCCESS; // nothing to delete
    int i;
    for (i = 0; i < length; i++) {
        dsa_error_t derror = dsa_remove_at_index(dsa, indices[i]);
        if (derror != DSA_SUCCESS) return derror;
    }
    return DSA_SUCCESS;
}

// make cleaner
dsa_error_t dsa_append_in_sorted_order(dynamic_string_array * dsa, const char * str) {
    int i;
    for (i = 0; i < dsa->num_elements; i++) {
        if (strcmp(dsa->array[i], str) >= 0) return dsa_insert(dsa, i, str);
    }
    return dsa_append(dsa, str);
}

void dsa_clear(dynamic_string_array * dsa) {
    int i;
    for (i = dsa->base_size; i < dsa->num_elements; i++) {
        free(dsa->array[i]);
    }
    dsa->num_elements = 0;
}