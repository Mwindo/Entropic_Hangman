/* dsa_tests.c - functions to test the appropriate behavior of the dynamic_string_array API */
/* These are just a first line of defense and are not foolproof (e.g. several edge cases are not tested for)! */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "ds_str_funcs.h"
#include "dynamic_string_array.h"

static bool free_dsa_and_return_test_result(dynamic_string_array * dsa, bool val) {
    dsa_free(dsa);
    return val;
}

static bool dsa_return_test_result(dynamic_string_array * dsa, char * testname, bool val) {
    if (val) ds_print_and_flush(DS_COLOR_GREEN "\n  • PASS %s" DS_COLOR_RESET, testname); else ds_print_and_flush(DS_COLOR_RED "\n  X FAIL %s" DS_COLOR_RESET, testname);
    return free_dsa_and_return_test_result(dsa, val);
}

static bool dsa_init_and_free_test() {
    char * name = "dsa_init_and_free_test";
    dynamic_string_array * dsa = malloc(sizeof(dynamic_string_array));
    if (!dsa_init(dsa, 5, 20)) return dsa_return_test_result(dsa, name, false);
    return dsa_return_test_result(dsa, name, true);
}

static bool dsa_append_contains_test() {
    char * name = "dsa_append_contains_test";
    dynamic_string_array * dsa = malloc(sizeof(dynamic_string_array));
    dsa_init(dsa, 5, 20);
    if (dsa_append(dsa, "First String") != DSA_SUCCESS) return dsa_return_test_result(dsa, name, false);
    return dsa_return_test_result(dsa, name, dsa_contains(dsa, "First String"));
}

static bool dsa_insert_test() {
    char * name = "dsa_insert_test";
    dynamic_string_array * dsa = malloc(sizeof(dynamic_string_array));
    dsa_init(dsa, 5, 20);
    if (dsa_insert(dsa, 0, "1") != DSA_SUCCESS) return dsa_return_test_result(dsa, name, false);
    if (dsa_insert(dsa, 0, "0") != DSA_SUCCESS) return dsa_return_test_result(dsa, name, false);
    if (dsa_insert(dsa, 2, "error") == DSA_SUCCESS) dsa_return_test_result(dsa, name, false);
    if (dsa_insert(dsa, 1, "first 1") != DSA_SUCCESS) dsa_return_test_result(dsa, name, false);
    // contents should be 0, first 1, 1
    if (strcmp(dsa->array[0], "0") != 0) return dsa_return_test_result(dsa, name, false);
    if (strcmp(dsa->array[1], "first 1") != 0) return dsa_return_test_result(dsa, name, false);
    if (strcmp(dsa->array[2], "1") != 0) return dsa_return_test_result(dsa, name, false);
    return dsa_return_test_result(dsa, name, true);
}

static bool dsa_remove_test() {
    char * name = "dsa_remove_test";
    dynamic_string_array * dsa = malloc(sizeof(dynamic_string_array));
    dsa_init(dsa, 5, 20);
    char * s[3] = {"delete this", "delete this first", "delete this"};
    dsa_append_multiple(dsa, s, 3);
    if (dsa_remove(dsa, "delete this first") != DSA_SUCCESS) return dsa_return_test_result(dsa, name, false);
    if (dsa_remove(dsa, "delete this") != DSA_SUCCESS) return dsa_return_test_result(dsa, name, false);
    if (dsa->num_elements != 1) return dsa_return_test_result(dsa, name, false);
    if (dsa_remove(dsa, "delete this") != DSA_SUCCESS) return dsa_return_test_result(dsa, name, false); 
    if (dsa_remove(dsa, "delete this") != DSA_SUCCESS) return dsa_return_test_result(dsa, name, false); // check no effect deleting nonexistent thing
    return dsa_return_test_result(dsa, name, dsa->num_elements == 0);
}

static bool dsa_append_enlarge_test() {
    char * name = "dsa_append_enlarge_test";
    dynamic_string_array * dsa = malloc(sizeof(dynamic_string_array));
    dsa_init(dsa, 3, 20);
    dsa_append(dsa, "1");
    dsa_append(dsa, "2");
    dsa_append(dsa, "3");
    return dsa_return_test_result(dsa, name, dsa_append(dsa, "4") == DSA_SUCCESS && dsa->max_num_elements > 3 && dsa->max_num_elements >= dsa->num_elements);
}

static bool dsa_shrink_test() {
    char * name = "dsa_shrink_test";
    dynamic_string_array * dsa = malloc(sizeof(dynamic_string_array));
    dsa_init(dsa, 3, 20);
    dsa->max_num_elements = 3000;
    dsa_append(dsa, "0");
    dsa_remove(dsa, "0");
    return dsa_return_test_result(dsa, name, dsa->max_num_elements < 3000);
}

// should be split up
static bool dsa_append_multiple_and_count_test() {
    char * name = "dsa_append_multiple_and_count_test";
    dynamic_string_array * dsa = malloc(sizeof(dynamic_string_array));
    dsa_init(dsa, 5, 20);
    unsigned num = 3;
    char ** strings = malloc(num * sizeof(char *));
    int i;
    for (i = 0; i < num; i++) {
        strings[i] = "000";
    }
    if (dsa_append_multiple(dsa, strings, num) != DSA_SUCCESS) return dsa_return_test_result(dsa, name, false);
    return dsa_return_test_result(dsa, name, dsa_count(dsa, "000") == num);
}

// this test is unusual: shifts are not generally meant to be used except for inserting/removing
// so there is some fiddling here (e.g. setting dsa->num_elements at the end) to account for that
static bool dsa_shift_right_left() {
    char * name = "dsa_shift_right_left_test";
    dynamic_string_array * dsa = malloc(sizeof(dynamic_string_array));
    dsa_init(dsa, 5, 20);
    char * s[3] = {"1", "2", "3"};
    dsa_append_multiple(dsa, s, 3);
    if (dsa_shift_strings_right(dsa, 0) != DSA_SUCCESS) return dsa_return_test_result(dsa, name, false);
    int i;
    for (i = 1; i < 3; i++) {
        if (dsa->array[i][0] != i + '0') return dsa_return_test_result(dsa, name, false); 
    }
    if (dsa_shift_strings_left(dsa, 0) != DSA_SUCCESS) return dsa_return_test_result(dsa, name, false);
    for (i = 0; i < 2; i++) {
        if (dsa->array[i][0] != (i + 1) + '0') return dsa_return_test_result(dsa, name, false); 
    }
    dsa->num_elements -= 1; // otherwise will try to free something that has already been freed
    return dsa_return_test_result(dsa, name, true);
}

static bool dsa_reverse_test() {
    char * name = "dsa_reverse_test";
    dynamic_string_array * dsa = malloc(sizeof(dynamic_string_array));
    dsa_init(dsa, 5, 20);
    char * s[4] = {"1", "2", "3", "4"};
    dsa_append_multiple(dsa, s, 4);
    dsa_reverse(dsa);
    int i;
    for (i = 0; i < 4; i++) {
        if (dsa->array[i][0] != (4-i) + '0') return dsa_return_test_result(dsa, name, false);
    }
    return dsa_return_test_result(dsa, name, true); 
}

static bool dsa_swap_test() {
    char * name = "dsa_swap_test";
    dynamic_string_array * dsa = malloc(sizeof(dynamic_string_array));
    dsa_init(dsa, 5, 20);
    char * s[4] = {"1", "2", "3", "4"};
    dsa_append_multiple(dsa, s, 4);
    dsa_swap(dsa, 0, 3);
    if (dsa->array[0][0] != '4') return dsa_return_test_result(dsa, name, false);
    if (dsa->array[1][0] != '2') return dsa_return_test_result(dsa, name, false);
    if (dsa->array[2][0] != '3') return dsa_return_test_result(dsa, name, false);
    if (dsa->array[3][0] != '1') return dsa_return_test_result(dsa, name, false);
    return dsa_return_test_result(dsa, name, true); 
}

static bool dsa_rotate_test() {
    char * name = "dsa_rotate_test";
    dynamic_string_array * dsa = malloc(sizeof(dynamic_string_array));
    dsa_init(dsa, 5, 20);
    char * s[4] = {"1", "2", "3", "4"};
    dsa_append_multiple(dsa, s, 4);
    int indices[3] = {0, 1, 3};
    if (dsa_rotate_indices(dsa, indices, 3) != DSA_SUCCESS) return dsa_return_test_result(dsa, name, false);;
    if (dsa->array[0][0] != '4') return dsa_return_test_result(dsa, name, false);
    if (dsa->array[1][0] != '1') return dsa_return_test_result(dsa, name, false);
    if (dsa->array[2][0] != '3') return dsa_return_test_result(dsa, name, false);
    if (dsa->array[3][0] != '2') return dsa_return_test_result(dsa, name, false);
    return dsa_return_test_result(dsa, name, true); 
}

static bool dsa_sort_test() {
    char * name = "dsa_sort_test";
    dynamic_string_array * dsa = malloc(sizeof(dynamic_string_array));
    dsa_init(dsa, 5, 20);
    char * s[4] = {"4", "2", "1", "3"};
    dsa_append_multiple(dsa, s, 4);
    dsa_sort(dsa);
    int i;
    for (i = 0; i < 4; i++) {
        if (dsa->array[i][0] != (i+1) + '0') return dsa_return_test_result(dsa, name, false);
    }
    return dsa_return_test_result(dsa, name, true); 
}

static bool dsa_copy_test() {
    char * name = "dsa_copy_test";
    dynamic_string_array * dsa = dsa_create_from_string("hello");
    dynamic_string_array * copy = dsa_copy(dsa);
    if (!copy || copy->base_size != dsa->base_size || copy->num_elements != dsa->num_elements) {
        dsa_free(dsa);
        return dsa_return_test_result(copy, name, false);
    }
    int i;
    for (i = 0; i < dsa->num_elements; i++) {
        if (strcmp(dsa->array[i], copy->array[i]) != 0) return dsa_return_test_result(dsa, name, false);
    }
    return dsa_return_test_result(dsa, name, true);
}

static bool dsa_hash_test() {
    char * name = "dsa_to_hash_test";
    dynamic_string_array * dsa = malloc(sizeof(dynamic_string_array));
    dsa_init(dsa, 5, 20);
    char * s[6] = {"1", "2", "2", "2", "1", "3"};
    dsa_append_multiple(dsa, s, 6);
    dsa_hash_table * ht = dsa_to_hash_table(dsa);
    if (!ht) dsa_return_test_result(dsa, name, false); 
    int count1 = 0;
    int count2 = 0;
    int count3 = 0;
    while (dsa_hash_table_contains(ht, "1")) {
        count1 += 1;
        dsa_hash_table_update(ht, "1", -1);
    }
    while (dsa_hash_table_contains(ht, "2")) {
        count2 += 1;
        dsa_hash_table_update(ht, "2", -1);
    }
    while (dsa_hash_table_contains(ht, "3")) {
        count3 += 1;
        dsa_hash_table_update(ht, "3", -1);
    }
    if (count1 != 2) return dsa_return_test_result(dsa, name, false);
    if (count2 != 3) return dsa_return_test_result(dsa, name, false);
    if (count3 != 1) return dsa_return_test_result(dsa, name, false);
    dsa_hash_table_free(ht);
    return dsa_return_test_result(dsa, name, true); 
}

static bool dsa_remove_duplicates_test() {
    char * name = "dsa_remove_duplicates_test";
    dynamic_string_array * dsa = malloc(sizeof(dynamic_string_array));
    dsa_init(dsa, 5, 20);
    char * s[6] = {"1", "2", "2", "2", "1", "3"};
    dsa_append_multiple(dsa, s, 6);
    if (dsa_remove_duplicates(dsa) != DSA_SUCCESS) return dsa_return_test_result(dsa, name, false);
    if (dsa->num_elements != 3) return dsa_return_test_result(dsa, name, false);
    if (dsa->array[0][0] != '1') return dsa_return_test_result(dsa, name, false); 
    if (dsa->array[1][0] != '2') return dsa_return_test_result(dsa, name, false);
    if (dsa->array[2][0] != '3') return dsa_return_test_result(dsa, name, false);
    return dsa_return_test_result(dsa, name, true); 
}

static bool dsa_set_union_test() {
    char * name = "dsa_set_union_test";
    dynamic_string_array * dsa1 = malloc(sizeof(dynamic_string_array));
    dsa_init(dsa1, 5, 20);
    dynamic_string_array * dsa2 = malloc(sizeof(dynamic_string_array));
    dsa_init(dsa2, 5, 20);
    char * s1[4] = {"4", "2", "4", "3"};
    char * s2[3] = {"4", "2", "1"};
    dsa_append_multiple(dsa1, s1, 4);
    dsa_append_multiple(dsa2, s2, 3);
    dynamic_string_array * dsa3 = dsa_set_union(dsa1, dsa2);    
    dsa_free(dsa1);
    dsa_free(dsa2);
    if (!dsa3) return dsa_return_test_result(dsa3, name, false);
    if (dsa3->num_elements != 4) return dsa_return_test_result(dsa3, name, false);
    if (!dsa_contains(dsa3, "4")) return dsa_return_test_result(dsa3, name, false);
    if (!dsa_contains(dsa3, "2")) return dsa_return_test_result(dsa3, name, false);
    if (!dsa_contains(dsa3, "1")) return dsa_return_test_result(dsa3, name, false);
    if (!dsa_contains(dsa3, "3")) return dsa_return_test_result(dsa3, name, false);
    return dsa_return_test_result(dsa3, name, true);
}

static bool dsa_set_intersection_test() {
    char * name = "dsa_set_intersection_test";
    dynamic_string_array * dsa1 = malloc(sizeof(dynamic_string_array));
    dsa_init(dsa1, 5, 20);
    dynamic_string_array * dsa2 = malloc(sizeof(dynamic_string_array));
    dsa_init(dsa2, 5, 20);
    char * s1[4] = {"4", "2", "4", "3"};
    char * s2[3] = {"4", "2", "1"};
    dsa_append_multiple(dsa1, s1, 4);
    dsa_append_multiple(dsa2, s2, 3);
    dynamic_string_array * dsa3 = dsa_set_intersection(dsa1, dsa2);
    dsa_free(dsa1);
    dsa_free(dsa2);
    if (!dsa3) return dsa_return_test_result(dsa3, name, false);
    if (dsa3->num_elements != 2) return dsa_return_test_result(dsa3, name, false);
    if (!dsa_contains(dsa3, "4")) return dsa_return_test_result(dsa3, name, false);
    if (!dsa_contains(dsa3, "2")) return dsa_return_test_result(dsa3, name, false);
    return dsa_return_test_result(dsa3, name, true);
}

static bool dsa_set_difference_test() {
    char * name = "dsa_set_difference_test";
    dynamic_string_array * dsa1 = malloc(sizeof(dynamic_string_array));
    dsa_init(dsa1, 5, 20);
    dynamic_string_array * dsa2 = malloc(sizeof(dynamic_string_array));
    dsa_init(dsa2, 5, 20);
    char * s1[4] = {"4", "2", "4", "3"};
    char * s2[3] = {"4", "2", "1"};
    dsa_append_multiple(dsa1, s1, 4);
    dsa_append_multiple(dsa2, s2, 3);
    dynamic_string_array * dsa3 = dsa_set_difference(dsa1, dsa2);
    dsa_free(dsa1);
    dsa_free(dsa2);
    if (!dsa3) return dsa_return_test_result(dsa3, name, false);
    if (dsa3->num_elements != 1) return dsa_return_test_result(dsa3, name, false);
    if (!dsa_contains(dsa3, "3")) return dsa_return_test_result(dsa3, name, false);
    return dsa_return_test_result(dsa3, name, true);
}

static bool dsa_multiset_difference_test() {
    char * name = "dsa_multiset_difference_test";
    dynamic_string_array * dsa1 = malloc(sizeof(dynamic_string_array));
    dsa_init(dsa1, 5, 20);
    dynamic_string_array * dsa2 = malloc(sizeof(dynamic_string_array));
    dsa_init(dsa2, 5, 20);
    char * s1[7] = {"4", "2", "4", "3", "1", "5", "7"};
    char * s2[3] = {"4", "2", "1"};
    dsa_append_multiple(dsa1, s1, 7);
    dsa_append_multiple(dsa2, s2, 3);
    dynamic_string_array * dsa3 = dsa_multiset_difference(dsa1, dsa2);
    dsa_free(dsa1);
    dsa_free(dsa2);
    if (!dsa3) return dsa_return_test_result(dsa3, name, false);
    if (dsa3->num_elements != 4) return dsa_return_test_result(dsa3, name, false);
    if (!dsa_contains(dsa3, "4")) return dsa_return_test_result(dsa3, name, false);
    if (!dsa_contains(dsa3, "3")) return dsa_return_test_result(dsa3, name, false);
    return dsa_return_test_result(dsa3, name, true);
}

static bool dsa_multiset_difference_test_2() {
    char * name = "dsa_multiset_difference_test_2";
    dynamic_string_array * dsa1 = dsa_create_from_string("perianthial");
    dynamic_string_array * dsa2 = dsa_create_from_string("erinil");
    dynamic_string_array * dsa3 = dsa_multiset_difference(dsa1, dsa2);
    dsa_free(dsa1);
    dsa_free(dsa2);
    if (!dsa3) return dsa_return_test_result(dsa3, name, false);
    if (dsa3->num_elements != 5) return dsa_return_test_result(dsa3, name, false);
    if (!dsa_contains(dsa3, "p")) return dsa_return_test_result(dsa3, name, false);
    if (dsa_count(dsa3, "a") != 2) return dsa_return_test_result(dsa3, name, false);
    if (!dsa_contains(dsa3, "t")) return dsa_return_test_result(dsa3, name, false);
    if (!dsa_contains(dsa3, "h")) return dsa_return_test_result(dsa3, name, false);
    return dsa_return_test_result(dsa3, name, true);
}

static bool dsa_multiset_intersection_test() {
    char * name = "dsa_multiset_intersection_test";
    dynamic_string_array * dsa1 = malloc(sizeof(dynamic_string_array));
    dsa_init(dsa1, 5, 20);
    dynamic_string_array * dsa2 = malloc(sizeof(dynamic_string_array));
    dsa_init(dsa2, 5, 20);
    char * s1[7] = {"6", "7", "8", "4", "2", "4", "3"};
    char * s2[5] = {"4", "4", "4", "2", "1"};
    dsa_append_multiple(dsa1, s1, 7);
    dsa_append_multiple(dsa2, s2, 5);
    dynamic_string_array * dsa3 = dsa_multiset_intersection(dsa1, dsa2);
    dsa_free(dsa1);
    dsa_free(dsa2);
    if (!dsa3) return dsa_return_test_result(dsa3, name, false);
    if (dsa3->num_elements != 3) return dsa_return_test_result(dsa3, name, false);
    if (dsa_count(dsa3, "4") != 2) return dsa_return_test_result(dsa3, name, false);
    if (!dsa_contains(dsa3, "2")) return dsa_return_test_result(dsa3, name, false);
    return dsa_return_test_result(dsa3, name, true);
}

static bool dsa_concat_test() {
    char * name = "dsa_concat_test";
    dynamic_string_array * dsa1 = malloc(sizeof(dynamic_string_array));
    dsa_init(dsa1, 5, 20);
    dynamic_string_array * dsa2 = malloc(sizeof(dynamic_string_array));
    dsa_init(dsa2, 5, 20);
    char * s1[4] = {"1", "2", "3"};
    char * s2[5] = {"4", "5", "6"};
    dsa_append_multiple(dsa1, s1, 3);
    dsa_append_multiple(dsa2, s2, 3);
    dynamic_string_array * dsa3 = dsa_concat(dsa1, dsa2);
    dsa_free(dsa1);
    dsa_free(dsa2);
    if (!dsa3) return dsa_return_test_result(dsa3, name, false);
    if (dsa3->num_elements != 6) return dsa_return_test_result(dsa3, name, false);
    if (strcmp(dsa3->array[0], "1") != 0 || strcmp(dsa3->array[1], "2") != 0 || strcmp(dsa3->array[2], "3") != 0) return dsa_return_test_result(dsa3, name, false);
    if (strcmp(dsa3->array[3], "4") != 0 || strcmp(dsa3->array[4], "5") != 0 || strcmp(dsa3->array[5], "6") != 0) return dsa_return_test_result(dsa3, name, false);
    return dsa_return_test_result(dsa3, name, true);
}

static bool dsa_create_from_string_test() {
    char * name = "dsa_create_from_string_test";
    char test[] = "hello";
    dynamic_string_array * dsa = dsa_create_from_string(test);
    if (!dsa) return dsa_return_test_result(dsa, name, false);
    int i;
    for (i = 0; i < 5; i++) {
        if (dsa->array[i][0] != test[i] || strlen(dsa->array[i]) != 1) return dsa_return_test_result(dsa, name, false);
    }
    return dsa_return_test_result(dsa, name, true);
}

static dynamic_string_array * dsa_for_string_tests() {
    dynamic_string_array * dsa = malloc(sizeof(*dsa)); 
    if (!dsa_init(dsa, 5, 10)) return NULL;
    dsa_append(dsa, "helLo");
    dsa_append(dsa, "goodbye");
    dsa_append(dsa, "Aloha");
    dsa_append(dsa, "!");
    return dsa;
}

static bool dsa_convert_to_string_test() {
    char * name = "dsa_convert_to_string_test";
    dynamic_string_array * dsa = dsa_for_string_tests();
    if (!dsa) return dsa_return_test_result(dsa, name, false);
    char * test = dsa_convert_to_string(dsa);  
    if (strcmp(test, "helLogoodbyeAloha!") != 0) {
        if (test) free(test);
        return dsa_return_test_result(dsa, name, false);
    }
    if (test) free(test);
    return dsa_return_test_result(dsa, name, true);
}

static bool dsa_convert_to_string_capitalized_test() {
    char * name = "dsa_convert_to_string_capitalized";
    dynamic_string_array * dsa = dsa_for_string_tests();
    if (!dsa) return dsa_return_test_result(dsa, name, false);
    char * test = dsa_convert_to_string_capitalized(dsa);  
    if (strcmp(test, "Hellogoodbyealoha!") != 0) {
        if (test) free(test);
        return dsa_return_test_result(dsa, name, false);
    }
    if (test) free(test);
    return dsa_return_test_result(dsa, name, true);
}

static bool dsa_convert_to_string_lowercase_test() {
    char * name = "dsa_convert_to_string_lowercase_test";
    dynamic_string_array * dsa = dsa_for_string_tests();
    if (!dsa) return dsa_return_test_result(dsa, name, false);
    char * test = dsa_convert_to_string_lowercase(dsa);  
    if (strcmp(test, "hellogoodbyealoha!") != 0) {
        if (test) free(test);
        return dsa_return_test_result(dsa, name, false);
    }
    if (test) free(test);
    return dsa_return_test_result(dsa, name, true);
}

static bool dsa_convert_to_string_all_caps_test() {
    char * name = "dsa_convert_to_string_all_caps_test";
    dynamic_string_array * dsa = dsa_for_string_tests();
    if (!dsa) return dsa_return_test_result(dsa, name, false);
    char * test = dsa_convert_to_string_all_caps(dsa);  
    if (strcmp(test, "HELLOGOODBYEALOHA!") != 0) {
        if (test) free(test);
        return dsa_return_test_result(dsa, name, false);
    }
    if (test) free(test);
    return dsa_return_test_result(dsa, name, true);
}

static bool dsa_convert_to_string_pascal_test() {
    char * name = "dsa_convert_to_string_pascal_test";
    dynamic_string_array * dsa = dsa_for_string_tests();
    if (!dsa) return dsa_return_test_result(dsa, name, false);
    char * test = dsa_convert_to_string_pascal(dsa);  
    if (strcmp(test, "HelloGoodbyeAloha!") != 0) {
        if (test) free(test);
        return dsa_return_test_result(dsa, name, false);
    }
    if (test) free(test);
    return dsa_return_test_result(dsa, name, true);
}

static bool dsa_convert_to_string_camel_test() {
    char * name = "dsa_convert_to_string_camel_test";
    dynamic_string_array * dsa = dsa_for_string_tests();
    if (!dsa) return dsa_return_test_result(dsa, name, false);
    char * test = dsa_convert_to_string_camel(dsa);  
    if (strcmp(test, "helloGoodbyeAloha!") != 0) {
        if (test) free(test);
        return dsa_return_test_result(dsa, name, false);
    }
    if (test) free(test);
    return dsa_return_test_result(dsa, name, true);
}

static bool dsa_sentinel_test() {
    return true;
}

bool dsa_run_tests() {
    ds_print_and_flush(DS_COLOR_YELLOW "\nRunning dsa tests");
    bool (*tests[]) () = {
        dsa_init_and_free_test, 
        dsa_append_contains_test, 
        dsa_insert_test, 
        dsa_remove_test, 
        dsa_append_enlarge_test, 
        dsa_shrink_test, 
        dsa_append_multiple_and_count_test,
        dsa_shift_right_left,
        dsa_reverse_test,
        dsa_swap_test,
        dsa_rotate_test,
        dsa_sort_test, 
        dsa_copy_test,
        dsa_hash_test,
        dsa_remove_duplicates_test,
        dsa_set_union_test,
        dsa_set_intersection_test,
        dsa_set_difference_test,
        dsa_multiset_difference_test,
        dsa_multiset_difference_test_2,
        dsa_multiset_intersection_test,
        dsa_concat_test,
        dsa_create_from_string_test,
        dsa_convert_to_string_test,
        dsa_convert_to_string_capitalized_test,
        dsa_convert_to_string_lowercase_test,
        dsa_convert_to_string_all_caps_test,
        dsa_convert_to_string_pascal_test,
        dsa_convert_to_string_camel_test,
        dsa_sentinel_test, // keep this last!
    };
    unsigned index = 0;
    unsigned numfailed = 0;
    while (tests[index] != dsa_sentinel_test) {
        if (!tests[index]()) numfailed += 1;
        index++;
    }
    if (numfailed > 0) { 
        ds_print_and_flush("\n%i out of %i dsa tests failed\n", numfailed, index);
        return false;
    }
    ds_print_and_flush(DS_COLOR_YELLOW "\nAll %i dsa tests passed\n" DS_COLOR_RESET, index);
    return true;
}
