/*
dsa_strings.c: code for converting a string to a dsa or vice-versa, including things like toupper/tolower/pascal case, etc.
*/

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "dynamic_string_array.h"
#include "ds_str_funcs.h"
#include <stdio.h>

#define NO_CHANGE 0
#define FIRST_UPPER 1
#define ALL_LOWER 2
#define ALL_UPPER 3
#define PASCAL 4
#define CAMEL 5

dynamic_string_array * dsa_create_from_string(const char * str) {
    dynamic_string_array * ret = malloc(sizeof(*ret));
    if (!ret) return NULL;
    if (!dsa_init(ret, strlen(str), 1)) return NULL;
    int i;
    for (i=0; i<strlen(str); i++) {
        if (dsa_append(ret, ds_convert_char_to_string(str[i])) != DSA_SUCCESS) return NULL;
    }
    return ret;
}

static char return_char_based_on_case_arg(char c, int case_arg, int word_index, int letter_index) {
    switch(case_arg) {
        case NO_CHANGE:
            return c;
        case FIRST_UPPER:
            if (word_index == 0 && letter_index == 0) return toupper(c); else return tolower(c);
        case ALL_UPPER:
            return toupper(c);
        case ALL_LOWER:
            return tolower(c);
        case PASCAL:
            if (letter_index == 0) return toupper(c); else return tolower(c);
        case CAMEL:
            if (word_index > 0 && letter_index == 0) return toupper(c); else return tolower(c);  
    }
    return c;
} 

// improve
static char * dsa_convert_to_string_full(const dynamic_string_array * dsa, int case_arg) {
    if (!dsa) return NULL;
    int i, j;
    int index = 0;
    unsigned num_chars = 0;
    for (i = 0; i < dsa->num_elements; i++) {
        num_chars += strlen(dsa->array[i]);      
    }
    char * str = malloc(sizeof(char) * (num_chars + 1));
    if (!str) return NULL;
    for (i = 0; i < dsa->num_elements; i++) {
        for (j = 0; j < strlen(dsa->array[i]); j++) {
            str[index] = return_char_based_on_case_arg(dsa->array[i][j], case_arg, i, j);
            index += 1;
        }  
    }
    str[index] = '\0';
    return str;
}

char * dsa_convert_to_string(const dynamic_string_array * dsa) {
    return dsa_convert_to_string_full(dsa, NO_CHANGE);
}

char * dsa_convert_to_string_capitalized(const dynamic_string_array * dsa) {
    return dsa_convert_to_string_full(dsa, FIRST_UPPER);
}

char * dsa_convert_to_string_lowercase(const dynamic_string_array * dsa) {
    return dsa_convert_to_string_full(dsa, ALL_LOWER);
}

char * dsa_convert_to_string_all_caps(const dynamic_string_array * dsa) {
    return dsa_convert_to_string_full(dsa, ALL_UPPER);
}

char * dsa_convert_to_string_pascal(const dynamic_string_array * dsa) {
    return dsa_convert_to_string_full(dsa, PASCAL);
}

char * dsa_convert_to_string_camel(const dynamic_string_array * dsa) {
    return dsa_convert_to_string_full(dsa, CAMEL);
}

dynamic_string_array * dsa_to_lower(const dynamic_string_array * dsa) {
    if (!dsa) return NULL;
    dynamic_string_array * ret = malloc(sizeof(*ret));
    if (!dsa_init(ret, dsa->base_size, dsa->str_length)) return NULL;
    int i;
    for (i = 0; i < dsa->num_elements; i++) {
        char * lower = ds_to_lower(dsa->array[i]);
        dsa_append(ret, lower);
    }
    return ret;
}

dynamic_string_array * dsa_to_upper(const dynamic_string_array * dsa) {
    if (!dsa) return NULL;
    dynamic_string_array * ret = malloc(sizeof(*ret));
    if (!dsa_init(ret, dsa->base_size, dsa->str_length)) return NULL;
    int i;
    for (i = 0; i < dsa->num_elements; i++) {
        char * lower = ds_to_upper(dsa->array[i]);
        dsa_append(ret, lower);
    }
    return ret;
}