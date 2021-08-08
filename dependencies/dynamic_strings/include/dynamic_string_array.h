/* dynamic_string_array.h - include all dynamic_string_array functions, and the struct */

#ifndef _Dynamic_String_Array_
#define _Dynamic_String_Array_

#include <stdbool.h>
#include <stdlib.h>

// not-super-granular error codes
typedef enum dsa_error_t {
    DSA_SUCCESS = 0,
    DSA_MALLOC_ERROR,
    DSA_REALLOC_ERROR,
    DSA_INDEX_ERROR,
    DSA_NUM_ERROR_CODES, //must be last
} dsa_error_t;

// the main struct
typedef struct dynamic_string_array {
    char ** array;
    unsigned str_length; // could make this dynamic, but it complicates things unnecessarily
    unsigned num_elements;
    unsigned max_num_elements;
    unsigned base_size; // never shrink smaller than the base size
} dynamic_string_array;

// basic hash bucket
typedef struct dsa_hash_bucket {
    struct dsa_hash_bucket * next;
    char * key;
    int value;
} dsa_hash_bucket;

// basic hash table
typedef struct dsa_hash_table {
    dsa_hash_bucket ** hashtable;
    unsigned num_keys_used;
    unsigned num_keys_total;
} dsa_hash_table;

// dsa_init: initializes a pointer to a dynamic_string_array dsa, setting dsa->base_size to size and dsa->str_length to str_length
// returns NULL on failure
dynamic_string_array * dsa_init(dynamic_string_array * dsa, unsigned size, unsigned str_length);

// dsa_get_error_str: returns a description for a given dsa_error code
const char * dsa_get_error_str(dsa_error_t errno);

// dsa_free: frees the pointer to a dynamic_string_array dsa
void dsa_free(dynamic_string_array * dsa);

// dsa_index_is_valid: checks that the passed index is within the bounds of dsa->array; returns success/failure
bool dsa_index_is_valid(const dynamic_string_array * dsa, int index);

// dsa_alloc_str_mem: allocates memory for a char * at position index of dsa->array; returns size of allocation (size = 0 on failure)
size_t dsa_alloc_str_mem(dynamic_string_array * dsa, int index);

// dsa_dealloc_str_mem: frees memory for a char * at position index of dsa->array
void dsa_dealloc_str_mem(dynamic_string_array * dsa, int index);

// dsa_is_full: checks and returns whether dsa->num_elements = dsa->max_num_elements
bool dsa_is_full(const dynamic_string_array * dsa);

// dsa_would_be_full: checks and returns whether dsa->num_elements would = dsa->max_num_elements if length number elements were added
bool dsa_would_be_full(const dynamic_string_array * dsa, unsigned length);

// dsa_enlarge: tries to reallocate memory to expand the size of dsa->array based on length number elements being added
// returns a pointer to dsa (or NULL on failure)
dynamic_string_array * dsa_enlarge(dynamic_string_array * dsa, unsigned length);

// dsa_enlarge_if_needed: checks whether or not the dsa would be full; if so, it enlarges; returns pointer to dsa (or NULL on failure)
dynamic_string_array * dsa_enlarge_if_needed(dynamic_string_array * dsa, unsigned length);

// dsa_shrink_array_if_big: checks whether or not dsa->max_num_elements is considerably bigger than dsa->num_elements
// if so, it attempts to reallocate memory to shrink the size of dsa->array
// returns pointer to dsa (or NULL on failure)
dynamic_string_array * dsa_shrink_array_if_big(dynamic_string_array * dsa);

// dsa_shift_strings_right: shifts all char * in dsa->array to the right starting at (including) startindex; returns error code
dsa_error_t dsa_shift_strings_right(dynamic_string_array * dsa, int startindex);

// dsa_shift_strings_left: shifts all char * in dsa->array to the left starting at (including) startindex; returns error code
dsa_error_t dsa_shift_strings_left(dynamic_string_array * dsa, int startindex);

// dsa_append: appends str to dsa->array, allocating memory and updating dsa->num_elements; returns error code
dsa_error_t dsa_append(dynamic_string_array * dsa, const char * str);

// dsa_insert: inserts str into dsa->array at index, allocating memory and updating dsa->num_elements; returns error code
// note: dsa_insert does not support inserting at the end; for that, use dsa_append instead
dsa_error_t dsa_insert(dynamic_string_array * dsa, int index, const char * str);

// dsa_remove_at_index: removes char * from dsa->array at index, freeing memory and updating dsa->num_elements; returns error code
dsa_error_t dsa_remove_at_index(dynamic_string_array *dsa, int index);

// dsa_remove: removes first instance char * that matches str from dsa->array, freeing memory and updating dsa->num_elements; returns error code
dsa_error_t dsa_remove(dynamic_string_array *dsa, const char * str);

// dsa_replace: replaces char * at index of dsa->array with str; returns error code
dsa_error_t dsa_replace(dynamic_string_array * dsa, int index, char * str);

// dsa_swap: swaps char * at index1 with char * at index2 of dsa->array; returns error code
dsa_error_t dsa_swap(dynamic_string_array *dsa, int index1, int index2);

// dsa_append_multiple: appends length number strings to dsa->array; returns error code
// this is slightly more efficient than calling dsa_append multiple times since it checks/reallocs dsa->strings once at most 
dsa_error_t dsa_append_multiple(dynamic_string_array * dsa, char ** strings, unsigned length);

// dsa_insert_multiple: inserts length number string to dsa->array at index; returns error code
// more efficient than calling dsa_insert multiple times since it shifts/allocates/copies all at once
dsa_error_t dsa_insert_multiple(dynamic_string_array *dsa, int index, char ** strings, unsigned length);

// dsa_remove_at_indices: removes length number elements from dsa->array at indices; returns error code
dsa_error_t dsa_remove_at_indices(dynamic_string_array * dsa, const int * indices, unsigned length);

// dsa_rotate_indices: for length number indices, the element at indices[i] is moved to indices[i+1],
// except for the element at index length-1, which is moved to index 0; returns error code
dsa_error_t dsa_rotate_indices(dynamic_string_array * dsa, int * indices, unsigned length);

// dsa_reverse: reverses dsa->array; returns error code
dsa_error_t dsa_reverse(dynamic_string_array * dsa);

// dsa_remove_duplicates: removes duplicate elements from dsa->array
dsa_error_t dsa_remove_duplicates(dynamic_string_array * dsa);

// dsa_contains: checks and returns whether str is among the elements in dsa->array
bool dsa_contains(const dynamic_string_array * dsa, const char * str);

// dsa_copy: returns a malloced copy of dsa (or NULL on failure)
dynamic_string_array * dsa_copy(const dynamic_string_array * dsa);

// dsa_append_in_sorted_order: appends str to dsa->array such that, if dsa->array is sorted before the call, dsa->array is sorted after the call
// returns error code
dsa_error_t dsa_append_in_sorted_order(dynamic_string_array * dsa, const char * str);

// dsa_sort: sorts the dsa->array in alphabetical (abc, not zyx) order
void dsa_sort(dynamic_string_array * dsa);

// dsa_create_from_string: allocates memory for, initializes, and returns a dynamic_string_array whose elements are the letters in str
// returns NULL on failure
dynamic_string_array * dsa_create_from_string(const char * str);

// dsa_to_lower: allocates memory for, initializes, and returns a dsa with elements all the same as the passed dsa, but lowercase
// returns NULL on failure
dynamic_string_array * dsa_to_lower(const dynamic_string_array * dsa);

// dsa_to_upper: allocates memory for, initializes, and returns a dsa with elements all the same as the passed dsa, but uppercase
// returns NULL on failure
dynamic_string_array * dsa_to_upper(const dynamic_string_array * dsa);

// dsa_convert_to_string: returns a string consisting of the characters in dsa->array (if dsa->array's elements are indeed one-character strings)
// returns NULL on failure
char * dsa_convert_to_string(const dynamic_string_array * dsa);

// dsa_convert_to_string_capitalized: same as dsa_convert_to_string, but capitalizes first letter and lowercases the rest
// returns NULL on failure
char * dsa_convert_to_string_capitalized(const dynamic_string_array * dsa);

// dsa_convert_to_string_lowercase: same as dsa_convert_to_string, but lowercases all letters
// returns NULL on failure
char * dsa_convert_to_string_lowercase(const dynamic_string_array * dsa);

// dsa_convert_to_string_all_caps: same as dsa_convert_to_string, but capitalizes all letters
// returns NULL on failure
char * dsa_convert_to_string_all_caps(const dynamic_string_array * dsa);

// dsa_convert_to_string_pascal: same as dsa_convert_to_string, but returns string in PascalCase
// returns NULL on failure
char * dsa_convert_to_string_pascal(const dynamic_string_array * dsa);

// dsa_convert_to_string_camel: same as dsa_convert_to_string, but returns string in camelCase
// returns NULL on failure
char * dsa_convert_to_string_camel(const dynamic_string_array * dsa);

// dsa_set_union: allocates memory for, iniializes, and returns a dsa whose elements are the union of distinct elements from dsa1, dsa2
// returns NULL on failure
dynamic_string_array * dsa_set_union(dynamic_string_array * dsa1, dynamic_string_array * dsa2);

// dsa_set_union: allocates memory for, iniializes, and returns a dsa whose elements are the intersection of distinct elements from dsa1, dsa2
// returns NULL on failure
dynamic_string_array * dsa_set_intersection(dynamic_string_array * dsa1, dynamic_string_array * dsa2);

// dsa_set_union: allocates memory for, iniializes, and returns a dsa whose elements are the distinct elements in dsa1 not in dsa2
// returns NULL on failure
dynamic_string_array * dsa_set_difference(dynamic_string_array * dsa1, dynamic_string_array * dsa2);

// dsa_multiset_intersection: allocates memory for, iniializes, and returns a dsa representing all the elements in both dsa1->array and dsa2->array
// e.g., {1, 1, 2, 2, 3, 4, 5} intersect {1, 2, 2, 7} --> {1, 2, 2}
// returns the elements sorted upon success otherwise NULL on failure
dynamic_string_array * dsa_multiset_intersection(dynamic_string_array * dsa1, dynamic_string_array * dsa2);

// dsa_multiset_union:  allocates memory for, iniializes, and returns a dsa that has the elements from dsa1 and dsa2 concatenated
// returns NULL on failure
dynamic_string_array * dsa_multiset_union(dynamic_string_array * dsa1, dynamic_string_array * dsa2);

// dsa_concatenate: a wrapper for dsa_multiset_union
dynamic_string_array * dsa_concat(dynamic_string_array * dsa1, dynamic_string_array * dsa2);

// dsa_multiset_difference:  allocates memory for, iniializes, and returns a dsa representing all the elements in dsa1->array but not in dsa2->array
// this includes repetitions, e.g., {1, 1, 1, 2, 3} - {1, 2} --> {1, 1, 3}
// returns the elements sorted upon success otherwise NULL on failure
dynamic_string_array * dsa_multiset_difference(dynamic_string_array * dsa1, dynamic_string_array * dsa2);

// dsa_clear: frees memory and removes all elements from dsa->array
void dsa_clear(dynamic_string_array * dsa);

// dsa_print: prints the elements in dsa->array with interleaving separator string
void dsa_print(const dynamic_string_array * dsa, const char * separator);

// dsa_count: get the number of times word appears in dsa->array
unsigned dsa_count(const dynamic_string_array * dsa, const char * word);

// dsa_to_hash_table: allocates memory for, iniializes, and returns a hash table such that every element in dsa->array is a key in the table with value = multiplicity
dsa_hash_table * dsa_to_hash_table(dynamic_string_array * dsa);

// dsa_hash_table_contains: checks if passed string is a key in passed dsa_hash_table struct
bool dsa_hash_table_contains(dsa_hash_table * ht, const char * key);

// dsa_hash_table_update: increment (or decrement) the count of the key in ht
void dsa_hash_table_update(dsa_hash_table * ht, const char * key, int increment);

// dsa_hash_table_free: frees the passed dsa_hash_table struct
void dsa_hash_table_free(dsa_hash_table * ht);

// dsa_hash_table_remove: removes the given key from ht
void dsa_hash_table_remove(dsa_hash_table * ht, const char * key);

// to see the contents of the hash table
void dsa_hash_table_print(dsa_hash_table * ht);

// dsa_run_tests: run test suite to ensure library is working
bool dsa_run_tests();

#endif