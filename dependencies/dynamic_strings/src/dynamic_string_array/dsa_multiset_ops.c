/*
dsa_multiset_ops.c: code for "multiset" (= set but with repeats) operations: union (= concatenation), intersection, and difference
*/

#include <string.h>
#include "dynamic_string_array.h"

dynamic_string_array * dsa_multiset_intersection(dynamic_string_array * dsa1, dynamic_string_array * dsa2) {
    if (!dsa1 || !dsa2) return NULL;
    dsa_hash_table * ht = dsa_to_hash_table(dsa2);
    if (!ht) return NULL;
    dynamic_string_array * dsa3 = malloc(sizeof(*dsa3));
    unsigned length = dsa1->str_length <= dsa2->str_length ? dsa1->str_length : dsa2->str_length; // since min is sufficient for intersect
    if (!dsa_init(dsa3, dsa2->base_size, length)) {
        dsa_hash_table_free(ht);
        return NULL;
    }
    int i;
    for (i = 0; i < dsa1->num_elements; i++) {
        if (dsa_hash_table_contains(ht, dsa1->array[i])) {
            dsa_hash_table_update(ht, dsa1->array[i], -1);
            dsa_append(dsa3, dsa1->array[i]);
        }       
    }
    dsa_hash_table_free(ht);
    return dsa3;
}

dynamic_string_array * dsa_multiset_union(dynamic_string_array * dsa1, dynamic_string_array * dsa2) {
    dynamic_string_array * d1 = dsa_copy(dsa1);
    if (!d1) return NULL;
    int i;
    for (i = 0; i < dsa2->num_elements; i++) {
        dsa_append(d1, dsa2->array[i]);
    }
    return d1;
}

dynamic_string_array * dsa_concat(dynamic_string_array * dsa1, dynamic_string_array * dsa2) {
    return dsa_multiset_union(dsa1, dsa2);
}

dynamic_string_array * dsa_multiset_difference(dynamic_string_array * dsa1, dynamic_string_array * dsa2) {
    if (!dsa1) return NULL;
    if (!dsa2) return dsa_copy(dsa1);
    dsa_hash_table * ht = dsa_to_hash_table(dsa2);
    if (!ht) return NULL;
    dynamic_string_array * dsa3 = malloc(sizeof(*dsa3));
    if (!dsa_init(dsa3, dsa1->base_size, dsa1->str_length)) {
        dsa_hash_table_free(ht);
        return NULL;
    }
    int i;
    for (i = 0; i < dsa1->num_elements; i++) {
        dsa_hash_table_print(ht);
        if (!dsa_hash_table_contains(ht, dsa1->array[i])) {
            dsa_append(dsa3, dsa1->array[i]);
        } 
        else {
            dsa_hash_table_update(ht, dsa1->array[i], -1);
        }  
    }
    dsa_hash_table_free(ht);
    return dsa3;
}