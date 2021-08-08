#ifndef _En_Hangman_Utils_
#define _En_Hangman_Utils_

#include <stdbool.h>

#define RED     "\x1B[31m"
#define YELLOW  "\x1B[33m" 
#define GREEN   "\x1B[32m"
#define RESET   "\x1B[0m"

extern bool debug;

void clear_screen();

int rand_num(int lbound, int ubound);

void delay_in_ms(int m);

int min(int * nums, int length);

int max(int * nums, int length);

void set_debug_mode_active();

void set_debug_mode_inactive();

void debug_print(const char * str, ...);

void toggle_debug_mode();

void print_and_flush(const char * str, ...);

#endif