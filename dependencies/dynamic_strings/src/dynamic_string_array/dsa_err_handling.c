/*
dsa_err_handling.c: error codes
*/

#include "dynamic_string_array.h"

// strings corresponding to error codes
const char* const dsa_error_strings[] = 
{
    "DSA_SUCCESS",
    "DSA_MALLOC_ERROR",
    "DSA_REALLOC_ERROR",
    "DSA_INDEX_ERROR",
    "DSA_INDEX_ERROR"
};

const char * dsa_get_error_str(dsa_error_t errno) {
    if (errno < DSA_NUM_ERROR_CODES) {
        return dsa_error_strings[errno];
    }
    return NULL;
}