#ifndef _Levenshteiner_
#define _Levenshteiner_

typedef struct Levenshteiner {
    int ** WF_array;
    char * word1;
    char * word2;
    int m;
    int n;
    char ** word_sequence;
    int cur_index;
    char * cur_string;
} Levenshteiner;

// initializes a Levenshteiner that will 
void init_Levenshteiner(Levenshteiner * l, char * word1, char * word2);

// deallocate all information associated with a Levenshteiner struct
void free_Levenshteiner(Levenshteiner *l);

// allocates memory for and returns the Wagner-Fischer matrix for the path from word1 to word2 using Levenshtein operators 
// (insert, remove, substitute)
// necessary for animate_distances, but a useful function on its own
int ** Wagner_Fischer_matrix(char * word1, char * word2);

// returns the Levenshtein distance between two words
int Levenshtein_distance(char * word1, char * word2);

// given a set of words, animate the path from word1 to word2 to word3 to . . .
void animate_distances(char ** words, int num_words, unsigned ms_delay);

// iterated 10000 times as a first check against memory leaks
bool test_Levenshteiner_memory_leak();

// quick and dirty tests against known values
bool test_Levenshteiner();

#endif