#ifndef _Unicode_Helper_
#define _Unicode_Helper_

#include <stdbool.h>

// init_hash_dict: initialize the utf-to-ascii dictionary
void init_hash_dict();

int unicode_helper_init();

int unicode_helper_free();

// next_xc_utf_index: returns the index of the next xc% non-ascii character 
int next_xc_utf_index(const char * str, int start_index);

// char_is_valid_utf_start: determines whether the character is utf that can be handled appropriately
bool char_is_valid_utf_start(char c);

// char_is_invalid_utf_start: determines whether the character is utf that cannot be handled
bool char_is_invalid_utf_start(char c);

// utf_to_ascii: given a string that might have some "valid" utf, returns a string that has only ascii by converting utf to ascii (e.g., é to e)
char * utf_to_ascii(const char * str);

bool utf_test1();

bool utf_test2();

#endif