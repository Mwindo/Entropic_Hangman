/*
dsa_modify.c: code for modifying the order or count of elements in the dsa
*/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "dynamic_string_array.h"

dsa_error_t dsa_replace(dynamic_string_array * dsa, int index, char * str) {
    if (!dsa_index_is_valid(dsa, index) || strlen(str) > dsa->str_length) return DSA_INDEX_ERROR;
    strcpy(dsa->array[index], str);
    return DSA_SUCCESS;
}

dsa_error_t dsa_swap(dynamic_string_array * dsa, int index1, int index2) {
    if (!dsa_index_is_valid(dsa, index1) || !dsa_index_is_valid(dsa, index2)) return DSA_INDEX_ERROR;
    char * s = malloc(sizeof(char) * (dsa->str_length) + 1);
    if (!s) return DSA_MALLOC_ERROR;
    strcpy(s, dsa->array[index1]);
    strcpy(dsa->array[index1], dsa->array[index2]);
    strcpy(dsa->array[index2], s);
    free(s);
    return DSA_SUCCESS;
}

// given indices i_1, i_2 . . . i_n, set i_1 to i_n, i_2 to i_1 . . . i_n to i_1
dsa_error_t dsa_rotate_indices(dynamic_string_array * dsa, int * indices, unsigned length) {
    if (length < 2) return DSA_SUCCESS; // nothing to rotate
    int i;
    for (i = 1; i < length; i++) {
        dsa_error_t derror = dsa_swap(dsa, indices[0], indices[i]);
        if (derror != DSA_SUCCESS) return derror;
    }
    return DSA_SUCCESS;
}

dsa_error_t dsa_reverse(dynamic_string_array * dsa) {
    if (dsa->num_elements < 2) return DSA_SUCCESS; // nothing to reverse
    int i = 0;
    int j = dsa->num_elements - 1;
    while (i < j) {
        dsa_error_t derror = dsa_swap(dsa, i, j);
        if (derror != DSA_SUCCESS) return derror;
        i += 1;
        j -= 1;
    }
    return DSA_SUCCESS;
}

dsa_error_t dsa_remove_duplicates(dynamic_string_array * dsa) {
    dsa_hash_table * ht = dsa_to_hash_table(dsa);
    if (!ht) return DSA_MALLOC_ERROR;
    dynamic_string_array * copy = malloc(sizeof(*copy)); // potentially faster to create a new dsa and add rather than remove: O(n) vs. O(n^2)
    dsa_init(copy, dsa->num_elements, dsa->str_length);
    int i;
    for (i = 0; i < dsa->num_elements; i++) {
        if (dsa_hash_table_contains(ht, dsa->array[i])) {
            dsa_append(copy, dsa->array[i]);
            dsa_hash_table_remove(ht, dsa->array[i]);
        }
    }
    dsa_hash_table_free(ht);
    *dsa = *copy;
    free(copy);
    return DSA_SUCCESS;
}
