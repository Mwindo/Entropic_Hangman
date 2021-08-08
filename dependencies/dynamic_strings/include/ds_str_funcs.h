// ds_str_funcs.h -- some basic string functions

#define DS_COLOR_RED     "\x1B[31m"
#define DS_COLOR_YELLOW  "\x1B[33m" 
#define DS_COLOR_GREEN   "\x1B[32m"
#define DS_COLOR_RESET   "\x1B[0m"

void ds_trim_string(char * str);

char * ds_to_upper(char * str);

char * ds_to_lower(char * str);

void ds_sort_chars(char * chars);

char * ds_copy_string(char * str);

char * ds_convert_char_to_string(char c);

void ds_remove_char_at_index(char *str, int index);

void ds_insert_char_at_index(char *str, int index, char c);

void ds_print_and_flush(const char * str, ...);