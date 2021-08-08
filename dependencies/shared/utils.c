#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <sodium.h>
#include <time.h>
#include <stdarg.h>
#include <stdbool.h>
#include "utils.h"

#ifdef _WIN32
#define clear() system("cls")
#else
#include <stdio.h>
#define clear() system("clear")
#endif

bool debug = false;

void clear_screen() {
    if (!debug) clear();
}

int rand_num(int lbound, int ubound) {
    debug_print("\nInitializing sodium");
    if (sodium_init() < 0) {
        exit(-1); // uh-oh!
    }
    debug_print("\nInitialized sodium");
    char buf[32];
    uint32_t num;
    debug_print("\nCalling random bytes");
    randombytes_buf(buf, 32);
    debug_print("\nGetting number between %i and %i", lbound, ubound);
    num = randombytes_uniform(ubound-lbound) + lbound; 
    debug_print("\nGot random number: %i", num);
    return num; 
}

void delay_in_ms(int m) {
    if (debug) return;
    struct timespec ts;
    int sec = (int)(m / 1000);
    m = m % 1000;
    ts.tv_sec = sec;
    ts.tv_nsec = m * 1000000;
    nanosleep(&ts, &ts);
}

int max(int * nums, int length) {
    int i;
    if (length == 0) { 
        return -1;
    }
    int maximum = nums[0];
    if (length == 1) {
        return maximum;
    }
    for (i = 1; i < length; i++) {
        if (nums[i] > maximum) {
            maximum = nums[i];
        }
    }
    return maximum;
}

int min(int * nums, int length) {
    int i;
    if (length == 0) { 
        return -1;
    }
    int minimum = nums[0];
    if (length == 1) {
        return minimum;
    }
    for (i = 1; i < length; i++) {
        if (nums[i] < minimum) {
            minimum = nums[i];
        }
    }
    return minimum;
}

void set_debug_mode_active() {
    debug = true;
}

void set_debug_mode_inactive() {
    debug = false;
}

void toggle_debug_mode() {
    debug = !debug;
}

void debug_print(const char * str, ...) {
    if (debug) {
        va_list args;
        va_start(args, str);
        vprintf(str, args);
        va_end(args);
        fflush(stdout);
    }
}

void print_and_flush(const char * str, ...) {
    va_list args;
    va_start(args, str);
    vprintf(str, args);
    va_end(args);
    fflush(stdout);
}