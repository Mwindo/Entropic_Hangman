#ifndef _Game_Dict_
#define _Game_Dict_

#define DEFAULT_MAX_WORD_LENGTH 30
#define DEFAULT_MAX_LANGUAGES 6
#define DEFAULT_DICT_NAME_LENGTH 25
#define DEFAULT_BASE_LANGUAGE_SIZE 50000
#define DICT_PATH "./dicts"
#define DICT_PREFIX "d_"

#include "dynamic_strings.h"
#include <pthread.h>
#include <stdio.h>

typedef struct game_dict_settings {
    int max_word_length;
    dynamic_string_array * languages;
    bool normalize_languages; // whether or not to uniformly distribute word selection over active languages
} game_dict_settings;

typedef struct game_dict {
    game_dict_settings * dictionary_settings;
    dynamic_string_array ** original_words; // list of lists, an array-based dictionary (e.g. english: . . ., spanish: . . .)
    dynamic_string_array ** active_words; // this really should be a set of integer indices instead
    int num_keys;
    int language_key; // if multiple languages are active, this specifies which language to choose a word from
    bool needs_reloading; // no need to reload words between rounds if the user hasn't changed anything
    pthread_mutex_t * mutex; // unused because multithreading did not noticeably impact performance
} game_dict;

// initialize fields for a game_dict struct
void game_dict_init(game_dict * gd);

// allocate memory for a game_dict's game_dict_settings and initialize fields
void game_dict_make_settings(game_dict * gd);

// initialize fields for a game_dict_settings struct
void game_dict_settings_init(game_dict_settings * gds);

// allocate memory for a game_dict's word arrays
void game_dict_init_word_arrays(game_dict * gd);

// free a game_dict struct
void game_dict_free(game_dict * gd);

// free a game_dict_settings struct
void game_dict_free_settings(game_dict_settings * gds);

// return whether words are to be selected uniformly over languages
bool game_dict_words_normalized(const game_dict * gd);

// determine whether a language is active
bool game_dict_lang_is_active(const game_dict * gd, const char * language);

// return the number of active words
int game_dict_num_active_words(const game_dict * gd);

// return the current language key (which stores which language to choose a word from)
int game_dict_lang_key(const game_dict * gd, const char * language);

// load the words from active language dictionaries into the gd word arrays
void game_dict_load_all_words(game_dict * gd);

// load the words from one dictionary into the gd word arrays
void game_dict_load_one_dict(game_dict * gd, const char * language);

// get the file pointer to the dictionary file for a language
FILE * game_dict_get_lang_fp(const char * language);

// get the path to the dictionary for a language
char * game_dict_get_lang_path(const char * language);

// allocate memory for an array of words keyed to a language in gd's word arrays
void game_dict_init_words_for_lang_key(game_dict * gd, int lang_index);

// load one word into gd's word arrays
void game_dict_load_one_word(game_dict * gd, char * word, int language_index, int debug_index);

// reset the the words in gd's word arrays
void game_dict_reset(game_dict * gd);

// reset the active words to all the original words
void game_dict_reset_active_words(game_dict * gd);

// replace the active words at a language key with a new set of active words
void game_dict_replace_active_words(game_dict * gd, int l_key, const dynamic_string_array * dsa);

// return an integer from 0 to number of active words (at the active language key) - 1
int game_dict_get_rand_word_index(const game_dict * gd);

// return a random word from among the active words (at the active language key)
char * game_dict_get_random_word(const game_dict * gd);

// return a word from among the active words (at the active language key) such that the word is "equivalent" (same length and contain all knowns)
char * game_dict_get_equivalent_word(game_dict * gd, char * word, dynamic_string_array * sortedknowns);

// return true if two words are "equivalent" (same length and contain all knowns)
bool game_dict_words_are_equivalent(game_dict * gd, char * word1, char * word2, dynamic_string_array * sortedknowns);

// randomize the language key if there are multiple language
void game_dict_choose_random_lang_key(game_dict * gd);

// change the language key (corresponds to selecting a new language to choose a word from)
void game_dict_choose_diff_lang_key(game_dict * gd);

// add a language to active languages
bool game_dict_add_lang(game_dict * gd, const char * language);

// remove a language from active languages
bool game_dict_remove_lang(game_dict * gd, const char * language);

// toggle whether or not a language is active and return an error message if the toggle fails (i.e., trying to deactivate all languages)
char * game_dict_toggle_lang_or_ret_err_message(game_dict * gd, const char * language);

// toggle whether or not to select words uniformly across languages (so that the program attempts to select a word from lang X or Y equally regardless of dictionary size)
void game_dict_toggle_lang_normalization(game_dict * gd);

// void * game_dict_prune_words_based_on_length(void * gsv); // background thread really didn't speed anything up

// void * game_dict_prune_words_based_on_known_letters(void * gsv); // background thread really didn't speed anything up

#endif