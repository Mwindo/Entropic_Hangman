/*
dsa_set_ops.c: code for set union, intersection, and difference
*/

#include <string.h>
#include "dynamic_string_array.h"

dynamic_string_array * dsa_set_intersection(dynamic_string_array * dsa1, dynamic_string_array * dsa2) {
    if (!dsa1 || !dsa2) return NULL;
    dsa_hash_table * ht = dsa_to_hash_table(dsa1);
    if (!ht) return NULL;
    dynamic_string_array * dsa3 = malloc(sizeof(*dsa3));
    unsigned length = dsa1->str_length <= dsa2->str_length ? dsa1->str_length : dsa2->str_length; // since min is sufficient for intersect
    if (!dsa_init(dsa3, dsa1->num_elements, length)) {
        dsa_hash_table_free(ht);
        return NULL;
    }
    int i;
    for (i = 0; i < dsa2->num_elements; i++) {
        if (dsa_hash_table_contains(ht, dsa2->array[i])) {
            dsa_hash_table_remove(ht, dsa2->array[i]);
            dsa_append(dsa3, dsa2->array[i]);
        }
    }
    dsa_hash_table_free(ht);
    return dsa3;
}

dynamic_string_array * dsa_set_union(dynamic_string_array * dsa1, dynamic_string_array * dsa2) {
    if (!dsa1) return dsa_copy(dsa2); // need to return copy so that result of function is consistent (i.e., a malloc-ed pointer); might have been better to do * in1, * in2, * out, but here we are 
    if (!dsa2) return dsa_copy(dsa1);
    dynamic_string_array * dsa3 = dsa_copy(dsa1);
    if (dsa2->str_length > dsa3->str_length) dsa3->str_length = dsa2->str_length;
    dsa_remove_duplicates(dsa3); // start with a copy of dsa1 and remove duplicates
    dsa_hash_table * ht = dsa_to_hash_table(dsa3);
    if (!ht) {
        dsa_free(dsa3);
        return NULL;
    }
    int i;
    for (i = 0; i < dsa2->num_elements; i++) { // iterate over hash of set(dsa3) and add to dsa3 and hash if it is not part of the hash already
        if (!dsa_hash_table_contains(ht, dsa2->array[i])) {
            dsa_hash_table_update(ht, dsa2->array[i], 1);
            dsa_append(dsa3, dsa2->array[i]);
        }
    }
    dsa_hash_table_free(ht);
    return dsa3;
}

dynamic_string_array * dsa_set_difference(dynamic_string_array * dsa1, dynamic_string_array * dsa2) {
    if (!dsa1) return NULL;
    if (!dsa2) return dsa_copy(dsa1); // need to return copy so that result of function is consistent (i.e., a malloc-ed pointer); might have been better to do * in1, * in2, * out, but here we are 
    dynamic_string_array * dsa3 = malloc(sizeof(*dsa3));
    dsa_init(dsa3, dsa1->base_size, dsa1->str_length);
    dsa_hash_table * ht = dsa_to_hash_table(dsa2);
    if (!ht) {
        dsa_free(dsa3);
        return NULL;
    }
    int i;
    for (i = 0; i < dsa1->num_elements; i++) { // iterate over hash of set(dsa3) and add to dsa3 and hash if it is not part of the hash already
        if (!dsa_hash_table_contains(ht, dsa1->array[i])) {
            dsa_hash_table_update(ht, dsa1->array[i], 1);
            dsa_append(dsa3, dsa1->array[i]);
        }
    }
    dsa_hash_table_free(ht);
    return dsa3;
}