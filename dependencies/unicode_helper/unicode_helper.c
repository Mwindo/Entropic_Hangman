/*
unicode_helper.c: a tool to convert certain utf characters (é, è, etc.) to ascii (e, e, etc.)
this is pretty sucky and should only be used for unimportant situations
like hangman
*/

// call this on loading a word

#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include "generic_hash_dict.h"
#include "unicode_helper.h"

hash_dict * hd;
bool initialized = false;

void init_hash_dict() {
    hd = malloc(sizeof(hash_dict));
    hash_dict_init(hd, 100, CHAR_TYPE, CHAR_TYPE);
    //A
    hash_dict_add_char(hd, '\x80', 'A');
    hash_dict_add_char(hd, '\x81', 'A');
    hash_dict_add_char(hd, '\x82', 'A');
    hash_dict_add_char(hd, '\x83', 'A');
    hash_dict_add_char(hd, '\x84', 'A');
    hash_dict_add_char(hd, '\x85', 'A');
    //hash_dict_add_char(hd, '\x86', 'A'); // should probably be AE
    hash_dict_add_char(hd, '\xa0', 'a');
    hash_dict_add_char(hd, '\xa1', 'a');
    hash_dict_add_char(hd, '\xa2', 'a');
    hash_dict_add_char(hd, '\xa3', 'a');
    hash_dict_add_char(hd, '\xa4', 'a');
    hash_dict_add_char(hd, '\xa5', 'a');  
    //hash_dict_add_char(hd, '\xa6', 'a'); // should probably be ae
    //C
    hash_dict_add_char(hd, '\x87', 'C');
    hash_dict_add_char(hd, '\xa7', 'c');
    //E
    hash_dict_add_char(hd, '\x88', 'E');
    hash_dict_add_char(hd, '\x89', 'E');
    hash_dict_add_char(hd, '\x8a', 'E');
    hash_dict_add_char(hd, '\x8b', 'E');
    hash_dict_add_char(hd, '\xa8', 'e');
    hash_dict_add_char(hd, '\xa9', 'e');
    hash_dict_add_char(hd, '\xaa', 'e');
    hash_dict_add_char(hd, '\xab', 'e');
    //I
    hash_dict_add_char(hd, '\x8c', 'I');
    hash_dict_add_char(hd, '\x8d', 'I');
    hash_dict_add_char(hd, '\x8e', 'I');
    hash_dict_add_char(hd, '\x8f', 'I');
    hash_dict_add_char(hd, '\xac', 'i');
    hash_dict_add_char(hd, '\xad', 'i');
    hash_dict_add_char(hd, '\xae', 'i');
    hash_dict_add_char(hd, '\xaf', 'i');
    //D
    hash_dict_add_char(hd, '\x90', 'D');
    //N
    hash_dict_add_char(hd, '\x91', 'N');
    hash_dict_add_char(hd, '\xb1', 'n');
    //O
    hash_dict_add_char(hd, '\x92', 'O');
    hash_dict_add_char(hd, '\x93', 'O');
    hash_dict_add_char(hd, '\x94', 'O');
    hash_dict_add_char(hd, '\x95', 'O');
    hash_dict_add_char(hd, '\x96', 'O');
    hash_dict_add_char(hd, '\xb2', 'o');
    hash_dict_add_char(hd, '\xb3', 'o');
    hash_dict_add_char(hd, '\xb4', 'o');
    hash_dict_add_char(hd, '\xb5', 'o');
    hash_dict_add_char(hd, '\xb6', 'o');
    //U
    hash_dict_add_char(hd, '\x99', 'U');
    hash_dict_add_char(hd, '\x9a', 'U');
    hash_dict_add_char(hd, '\x9b', 'U');
    hash_dict_add_char(hd, '\x9c', 'U');
    hash_dict_add_char(hd, '\xb9', 'u');
    hash_dict_add_char(hd, '\xba', 'u');
    hash_dict_add_char(hd, '\xbb', 'u');
    hash_dict_add_char(hd, '\xbc', 'u');
    //Y
    hash_dict_add_char(hd, '\x9d', 'Y');
    hash_dict_add_char(hd, '\xbd', 'y');

    initialized = true;
}

int unicode_helper_init() {
    if (!initialized) init_hash_dict();
    return 1;
}

int unicode_helper_free() {
    hash_dict_free(hd);
    return 1;
}

int next_xc_utf_index(const char * str, int start_index) {
    fflush(stdout);    
    int i;
    for (i = start_index; i < strlen(str); i++) {
        if (str[i] == '\xc3' || str[i] == '\xc4' || str[i] == '\xc5' || str[i] == '\xc6' || str[i] == '\xc7' || str[i] == '\xc8' || str[i] == '\xc9') {
            printf("\n%i\n", i);
            fflush(stdout);
            return i;
        }
    }
    return -1;
}

// valid in this context means "the code here can handle it"
bool char_is_valid_utf_start(char c) {
    return c == '\xc3';
}

bool char_is_invalid_utf_start(char c) {
    return c == '\xc4' || c == '\xc5' || c == '\xc6' || c == '\xc7' || c == '\xc8' || c == '\xc9';
}

char * utf_to_ascii(const char * str) {
    char * ret = malloc(sizeof(char) * (strlen(str)+1)); // maximum size of string
    char * s = ret;
    while (*str != '\0') {
        if (char_is_invalid_utf_start(*str)) {
            free(ret);
            return NULL;
        }
        else if (char_is_valid_utf_start(*str)) {
            bucket * p = hash_dict_get_value(hd, ++str);
            if (!p) {
                free(ret);
                return NULL; //character had no entry in the dictionary, so signal failure
            }
            *ret = *(char *)p->value;
        }
        else if (*str != '\xC3') {
            *ret = *str;
        }
        ret++, str++;
    }
    *ret = '\0';
    return s;
}

bool utf_test1() {
    char * s = "éhéllò";
    char * test = utf_to_ascii(s);
    bool ret = strcmp(test, "ehello") == 0;
    free(test);
    return ret;
}

bool utf_test2() {
    char * s = "héllås there";
    char * test = utf_to_ascii(s);
    bool ret = strcmp(s, "hella there") == 0;
    free(test);
    return ret;
}


/*
int main() {
    init_hash_dict();
    printf("\nHash_dict done!");
    fflush(stdout);
    char s[25] = "vía"; //éHéllòsssñ riñón 
    char * b = utf_to_ascii(s);
    printf("\n%s goes to %s\n", s, b);
    fflush(stdout);
    hash_dict_free(hd);
}

*/