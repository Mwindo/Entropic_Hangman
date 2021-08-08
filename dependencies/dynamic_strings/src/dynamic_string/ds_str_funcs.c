#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>

void ds_trim_string(char * str) {
    char * p = str;
    char * src = str;
    int i;
    for (i = 0; i < strlen(str); i++) {
        if (!isspace(str[i])) {
            *p = *src;
            p++;
        }
        src++;
    }
    *p = '\0';
}

char * ds_to_lower(char * str) {
    char * copy = malloc((strlen(str)+1) * sizeof(char));
    strcpy(copy, str);
    int i;
    for (i = 0; i < strlen(str); i++) {
        copy[i] = tolower(copy[i]);
    }
    return copy;
    //for ( ; *str; ++str) *p = tolower(*str);
}

char * ds_to_upper(char * str) {
    char * copy = malloc((strlen(str)+1) * sizeof(char));
    strcpy(copy, str);
    int i;
    for (i = 0; i < strlen(str); i++) {
        copy[i] = toupper(copy[i]);
    }
    return copy;
}

void ds_sort_chars(char * chars) {
    int i, j;
    for (i = 0; i < strlen(chars) - 1; i++) {
        for (j = i+1; j < strlen(chars); j++) {
            if (chars[i] > chars[j]) {
                char temp = chars[i];
                chars[i] = chars[j];
                chars[j] = temp;
            }
        }
    }
}

char * ds_copy_string(char * str) {
    char * copy = malloc(sizeof(char) * (strlen(str)+1));
    strcpy(copy, str);
    return copy;
}

char * ds_convert_char_to_string(char c) {
    char * str = malloc(sizeof(char) * 2);
    str[1] = '\0';
    str[0] = c;
    return str;
}

void ds_remove_char_at_index(char *str, int index) {
    char *src = str;
    char *dst = str;
    int i;
    for (i = 0; i < strlen(str); i++){
        *dst = *src;
        if (i != index) dst++;   
        src++;  
    }
    *dst = '\0';
}

void ds_insert_char_at_index(char *str, int index, char c) {
    char *src = str;
    char *dst = malloc(sizeof(char) * strlen(str) + 2); //+1 for null, +1 for inserted character
    if (!dst) exit(-1);
    int i;
    for (i = 0; i <= strlen(str); i++){
        if (i == index) dst[i] = c;
        else {
            dst[i] = *src;
            src++;  
        }
    }
    dst[strlen(str)+1] = '\0';
    strcpy(str, dst);
    free(dst);
}

void ds_print_and_flush(const char * str, ...) {
    va_list args;
    va_start(args, str);
    vprintf(str, args);
    va_end(args);
    fflush(stdout);
}