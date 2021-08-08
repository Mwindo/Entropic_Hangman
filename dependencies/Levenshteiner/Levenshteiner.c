/* Levenshteiner.c

The "main" function is animate_distances, along with a few miscellaneous helpers.

For each consecutive pair of words in a char ** array, animate_distances associates with it a Levenshteiner struct.
It uses the Wagner-Fischer algorithm to compute the Levenshtein distances between the two words, storing this and other info
in the struct. It then uses the Wagner-Fischer matrix that was created in the process to form a char ** array
representing the path from the first word to the second word, e.g., ["cat", "kat", "kit", "kite"] for the pair "[cat, kite]."

Finally, animate_distances loops over each Levenshteiner to print the paths from each word in the original char ** array to the next,
e.g, ["cat", "kat", "kit", "kite", "bite", "bitte", "bitten"] for ["cat", "kite", "bitten"].
It "animates" by clearing the screen between each string.

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "utils.h"
#include "str_funcs.h"
#include "Levenshteiner.h"

/*
====Static functions====
*/

static void print_one_word(char * word, unsigned ms_delay) {
    clear_screen();
    printf("\n%s", word);
    fflush(stdout);
    delay_in_ms(ms_delay);
}

static int max_size_of_words(char * word1, char * word2) {
    int nums[2] = {strlen(word1) + 1, strlen(word2) + 1};
    return max(nums, 2);
}

static int ** initialize_WF_matrix(int m, int n, int val) {
    int i, j;
    int ** arr = malloc(sizeof(*arr) * m);
    if (!arr) exit(-1);
    for (i = 0; i < m; i++) {
        arr[i] = malloc(sizeof(**arr) * n);
        if (!arr[i]) exit(-1);
        for (j = 0; j < n; j++) {
            arr[i][j] = val;
        }
    }
    return arr;
}

static int _Levenshtein_distance(Levenshteiner * l) {
    return l->WF_array[strlen(l->word1)][strlen(l->word2)];
}

static char ** init_Levenshtein_seq_arr(Levenshteiner * l, int size) {
    int distance = _Levenshtein_distance(l);
    l->word_sequence = malloc(sizeof(char *) * (distance + 1));
    int i;
    for (i = 0; i < distance + 1; i++) {
        l->word_sequence[i] = malloc(sizeof(char) * (size + 1));
    }
    return l->word_sequence;
}

static void handle_substitution(Levenshteiner * l) {
    if (l->WF_array[l->m-1][l->n-1] < l->WF_array[l->m][l->n]) { // substitution
        l->cur_string[l->m-1] = l->word2[l->n-1];
        strcpy(l->word_sequence[l->cur_index], l->cur_string);
        l->cur_index += 1;
    }
    l->m -= 1;
    l->n -= 1;
}

static void handle_deletion(Levenshteiner *l) {
    remove_char_at_index(l->cur_string, l->m-1);
    strcpy(l->word_sequence[l->cur_index], l->cur_string);
    l->cur_index += 1;
    l->m -= 1;
}

static void handle_insertion(Levenshteiner *l) {
    insert_char_at_index(l->cur_string, l->m, l->word2[l->n-1]);
    strcpy(l->word_sequence[l->cur_index], l->cur_string);
    l->cur_index += 1;          
    l->n -= 1;
}

static void set_Levenshtein_sequence(Levenshteiner * l) {
    while (l->m > 0 || l->n > 0) {
        int deletion = l->m > 0 ? l->WF_array[l->m-1][l->n] : l->m+l->n; // value can never be bigger than m+n
        int insertion = l->n > 0 ? l->WF_array[l->m][l->n-1] : l->m+l->n;
        int maybesub = l->m > 0 && l->n > 0 ? l->WF_array[l->m-1][l->n-1] : l->m+l->n;
        int nums[3] = {deletion, insertion, maybesub};
        int minimum = min(nums, 3);
        if (minimum == maybesub) {
            handle_substitution(l);
        }
        else if (minimum == deletion) {
            handle_deletion(l);
        }
        else if (minimum == insertion) {
            handle_insertion(l);
        }
    }
}

//deallocate heap memory for an array
static void free_arr(void ** arr, int m) {
    int i;
    for (i = 0; i < m; i++) {
        free(arr[i]);
    }
    free(arr);
}

/*
=====API=====
*/

void animate_distances(char ** words, int num_words, unsigned ms_delay) {
    int i, j;
    for (i = 1; i < num_words; i++) {
        Levenshteiner * l = malloc(sizeof(*l));
        init_Levenshteiner(l, words[i-1], words[i]);
        set_Levenshtein_sequence(l);
        for (j = 0; j <= _Levenshtein_distance(l); j++) {
            print_one_word(l->word_sequence[j], ms_delay);
        }
        free_Levenshteiner(l);
    }
}

int Levenshtein_distance(char * word1, char * word2) {
    int ** wf = Wagner_Fischer_matrix(word1, word2);
    int dist = wf[strlen(word1)][strlen(word2)];
    free_arr((void **)wf, strlen(word1));
    return dist;
}

void init_Levenshteiner(Levenshteiner * l, char * word1, char * word2) {
    int size = max_size_of_words(word1, word2);
    l->word1 = copy_string(word1);
    l->word2 = copy_string(word2);
    l->WF_array = Wagner_Fischer_matrix(l->word1, l->word2);
    l->m = strlen(word1);
    l->n = strlen(word2);
    l->word_sequence = init_Levenshtein_seq_arr(l, size);
    l->cur_index = 1;
    l->cur_string = malloc((size + 1) * sizeof(char));
    strcpy(l->cur_string, l->word1);
    strcpy(l->word_sequence[0], l->cur_string); // the first string in the path is the first word itself
}

int ** Wagner_Fischer_matrix(char * word1, char * word2) {
    int m = strlen(word1) + 1;
    int n = strlen(word2) + 1;
    int ** arr = initialize_WF_matrix(m, n, 0);
    int cost = 0; // for a substitution
    int i, j;
    for (i = 1; i < m; i++) {
        arr[i][0] = i; // row string[n] -> row string[n+1] takes 1 operation
    }
    for (j = 1; j < n; j++) {
        arr[0][j] = j; // column string[n] -> column string[n+1] takes 1 operation
    }
    for (j = 1; j < n; j++) {
        for (i = 1; i < m; i++) {
            cost = word1[i-1] == word2[j-1] ? 0 : 1; // if !=, need to sub one letter for another
            int nums[3] = {arr[i-1][j] + 1, arr[i][j-1] + 1, arr[i-1][j-1] + cost}; //deletion, insertion, substitution
            arr[i][j] = min(nums, 3);
        }
    }
    return arr;
}

void free_Levenshteiner(Levenshteiner *l) {
    free_arr((void **)l->word_sequence, _Levenshtein_distance(l) + 1);
    free(l->cur_string);
    free_arr((void **)l->WF_array, strlen(l->word1) + 1); 
    free(l->word1);
    free(l->word2);
    free(l);
}

/*
===Cursory tests===
Currently: quickly slapped together tests
Better: design and implement a testing library, similar to dsa_tests
*/

bool test_Levenshteiner_memory_leak() {
    int i;
    for (i = 0; i < 10000; i++) {
        char a[] = "thisisatest1asdfa sdfasd fasdfadsf asdf asdf asdf asdfdfdfdfdf";
        char b[] = "thisisatest1asdfa sdfasd fasdfadsf asdf asdf asdf asdfdfdfdfdf1";
        char c[] = "a";
        char d[] = "hellothere";
        char * z[5] = {a, b, c, d, "===Entropic Hangman==="};
        animate_distances(z, 5, 1);
    }
    return true;
}

static bool test_WF_1() {
    Levenshteiner * l = malloc(sizeof(* l));
    init_Levenshteiner(l, "hello", "axe");
    int matrix[6][4] = {{0,1,2,3},{1,1,2,3}, {2,2,2,2}, {3,3,3,3}, {4,4,4,4}, {5,5,5,5}};
    int i, j;
    for (i = 0; i < 6; i++) {
        for (j = 0; j < 4; j++) {
            if (matrix[i][j] != l->WF_array[i][j]) {
                free_Levenshteiner(l);
                return false;
            }
        }
    }
    free_Levenshteiner(l);
    return true;
}

static bool test_WF_2() {
    Levenshteiner * l = malloc(sizeof(* l));
    init_Levenshteiner(l, "convert this", "to that");
    int matrix[13][8] = {{0,1,2,3,4,5,6,7},{1,1,2,3,4,5,6,7}, {2,2,1,2,3,4,5,6}, {3,3,2,2,3,4,5,6}, {4,4,3,3,3,4,5,6}, {5,5,4,4,4,4,5,6}, {6,6,5,5,5,5,5,6}, {7,6,6,6,5,6,6,5}, {8,7,7,6,6,6,7,6}, {9,8,8,7,6,7,7,7}, {10,9,9,8,7,6,7,8}, {11,10,10,9,8,7,7,8}, {12,11,11,10,9,8,8,8}};
    int i, j;
    for (i = 0; i < 13; i++) {
        for (j = 0; j < 8; j++) {
            if (matrix[i][j] != l->WF_array[i][j]) {
                free_Levenshteiner(l);
                return false;
            }
        }
    }
    free_Levenshteiner(l);
    return true;
}

// test the Wagner-Fischer matrix against known values
static bool test_Wagner_Fischer() {
    return test_WF_1() && test_WF_2();
}

static bool test_Levenshteiner_seq() {
    Levenshteiner * l = malloc(sizeof(* l));
    init_Levenshteiner(l, "convert this", "to that");
    set_Levenshtein_sequence(l);
    int i;
    print_and_flush("\n  Testing Levenshtein distance . . .");
    int dist = _Levenshtein_distance(l);
    if (dist != 8) {
        print_and_flush("\nLevenshtein distance is wrong!");
        return false;
    }
    print_and_flush("\n  Levenshtein distance accepted\n  Testing Levenshtein sequence . . .");
    for (i = 1; i < _Levenshtein_distance(l); i++) {
        int k = Levenshtein_distance(l->word_sequence[i-1], l->word_sequence[i]);
        if (k != 1) {
            print_and_flush("\n  Levenshteiner sequence is wrong--not all consecutive pairs have distance of one");
            return false;
        }
    }
    print_and_flush("\n  Levenshtein sequence accepted");
    free_Levenshteiner(l);
    return true;
}

// test the Wagner-Fischer matrix and ensure all consecutive words have a Levenshtein distance of 1
bool test_Levenshteiner() {
    print_and_flush("\nTesting Levenshteiner\n  Testing Wagner-Fischer values . . .");
    if (!test_Wagner_Fischer()) {
        print_and_flush("\n  Wagner-Fischer matrix is wrong");
        return false;
    }
    print_and_flush("\n  Wagner-Fischer values ok!");
    if (!test_Levenshteiner_seq()) {
        print_and_flush("\nLevenshteiner has errors\n");
        return false;
    }
    else {
        print_and_flush("\nLevenshteiner passed all tests\n");
        return true;
    }
}