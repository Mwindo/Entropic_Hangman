/*
game_dict.c: stores and performs searches over the words loaded by the user.
*/

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <pthread.h>
#include "str_funcs.h"
#include "utils.h"
#include "dynamic_strings.h"
#include "game_dict.h"
#include "game_status.h"
#include "unicode_helper.h"

// Currently: Uses a fixed dynamic_string_array (all words) and a dynamic dynamic_string_array (active words)
// Better: uses a fixed dynamic_string_array (all words) and an int array (active words), which is smaller

//Con-/destructor functions

void game_dict_init(game_dict * gd) {
    gd->language_key = 0;
    game_dict_make_settings(gd);
    game_dict_init_word_arrays(gd);
    gd->mutex = malloc(sizeof(pthread_mutex_t));
    gd->needs_reloading = false;
    gd->num_keys = 1;
    pthread_mutex_init (gd->mutex, NULL);
}

void game_dict_make_settings(game_dict * gd) {
    gd->dictionary_settings = malloc(sizeof(game_dict_settings));
    game_dict_settings_init(gd->dictionary_settings);
}

void game_dict_settings_init(game_dict_settings * gds) {
    debug_print("\nInitializing game dictionary settings");
    gds->languages = malloc(sizeof(dynamic_string_array));
    dsa_init(gds->languages, DEFAULT_MAX_LANGUAGES, DEFAULT_DICT_NAME_LENGTH);
    dsa_append(gds->languages, "english");
    debug_print("\nInitialized game dictionary settings languages");
    gds->normalize_languages = false;
    debug_print("\nInitialized game dictionary settings normalize languages boolean");
    gds->max_word_length = DEFAULT_MAX_WORD_LENGTH;
    debug_print("\nDone initializing game dictionary settings");
}

void game_dict_init_word_arrays(game_dict * gd) {
    debug_print("\nInitializing words");
    gd->original_words = malloc(sizeof(dynamic_string_array *) * DEFAULT_MAX_LANGUAGES); // check
    debug_print("\n  Initialized original words");
    gd->active_words = malloc(sizeof(dynamic_string_array *) * DEFAULT_MAX_LANGUAGES);
    debug_print("\n  Initialized active words");
    debug_print("\nInitialized words");
}

void game_dict_free(game_dict * gd) {
    int i, j;
    int m = game_dict_words_normalized(gd) ? gd->dictionary_settings->languages->num_elements : 1;
    for (i = 0; i < m; i++) {
        dsa_free(gd->active_words[i]);
        dsa_free(gd->original_words[i]);
    }
    game_dict_free_settings(gd->dictionary_settings);
}

void game_dict_free_settings(game_dict_settings * gds) {
    dsa_free(gds->languages);
    free(gds);
}

// General purpose functions

int game_dict_num_active_words(const game_dict * gd) {
    return gd->active_words[gd->language_key]->num_elements;
}

// Word (re)loading functions

void game_dict_load_all_words(game_dict * gd) {
    debug_print("\nLoading words");
    game_dict_init_words_for_lang_key(gd, 0);
    int i;
    for (i = 0; i < gd->dictionary_settings->languages->num_elements; i++) {
        game_dict_load_one_dict(gd, gd->dictionary_settings->languages->array[i]);
    }
    gd->num_keys = game_dict_words_normalized(gd) ? gd->dictionary_settings->languages->num_elements-1: 1;
}

void game_dict_load_one_dict(game_dict * gd, const char * language) {
    int lang_index = game_dict_lang_key(gd, language);
    FILE * fp = game_dict_get_lang_fp(language);
    if (game_dict_words_normalized(gd) && lang_index != 0) {
        game_dict_init_words_for_lang_key(gd, lang_index);
    }
    char * buffer = malloc(sizeof(char) * (DEFAULT_MAX_WORD_LENGTH + 1)); // could pass the buffer for efficiency, but more modular here
    int debug = 0;
    while(fgets(buffer, DEFAULT_MAX_WORD_LENGTH, fp)) {
        // does not handle words longer than DEFAULT_MAX_WORD_LENGTH
        // so A ... BCD could be cut into multiple words A ..., ..., and ...BCD for exceptionally long ...
        game_dict_load_one_word(gd, buffer, lang_index, debug);
        debug += 1;
    }
    debug_print("\nDone loading one dict, freeing buffer");
    free(buffer);   
    fclose(fp);
}  

void game_dict_init_words_for_lang_key(game_dict * gd, int lang_index) {
    debug_print("\nLoading words at lang index %i", lang_index);
    gd->original_words[lang_index] = malloc(sizeof(dynamic_string_array));
    gd->active_words[lang_index] = malloc(sizeof(dynamic_string_array));
    dsa_init(gd->original_words[lang_index], DEFAULT_BASE_LANGUAGE_SIZE, DEFAULT_MAX_WORD_LENGTH); // only init these if user opts to uniformly sample across languages
    dsa_init(gd->active_words[lang_index], DEFAULT_BASE_LANGUAGE_SIZE, DEFAULT_MAX_WORD_LENGTH); // in which case we need to separate the languages
    debug_print("\nLoaded words at lang index %i", lang_index);
}

void game_dict_load_one_word(game_dict * gd, char * word, int language_index, int debug_index) {
    debug_print("\n%i: Loading %s", debug_index, word);
    trim_string(word);
    debug_print("\nTrimmed %s, entering into language index %i", word, language_index);
    debug_print("\n%i: %s", debug_index, word);
    dsa_append(gd->original_words[language_index], word);
    dsa_append(gd->active_words[language_index], word);
    debug_print("\nAppended %s", word);
}

void game_dict_reset(game_dict * gd) {
    debug_print("\nResetting game dict");
    gd->needs_reloading = false;
    int i, j;
    for (i = 0; i < gd->num_keys; i++) {
        dsa_free(gd->active_words[i]);
        dsa_free(gd->original_words[i]);
    }
    game_dict_load_all_words(gd);
    game_dict_choose_random_lang_key(gd);
}

void game_dict_reset_active_words(game_dict * gd) {
    game_dict_replace_active_words(gd, 0, gd->original_words[0]);
    if (game_dict_words_normalized(gd)) {
        int i;
        for (i = 1; i < gd->dictionary_settings->languages->num_elements; i++) {
            game_dict_replace_active_words(gd, i, gd->original_words[i]);
        }
    }
}

void game_dict_replace_active_words(game_dict * gd, int l_key, const dynamic_string_array * dsa) {
    pthread_mutex_lock(gd->mutex);
    debug_print("\nFreeing active words at key %i", l_key);
    dsa_free(gd->active_words[l_key]);
    debug_print("\nFreed active words at key %i", l_key);
    gd->active_words[l_key] = dsa_copy(dsa);
    pthread_mutex_unlock(gd->mutex);
}

// Finding a word

int game_dict_get_rand_word_index(const game_dict * gd) {
    return rand_num(0, gd->active_words[gd->language_key]->num_elements-1);
}

char * game_dict_get_random_word(const game_dict * gd) {
    int r = game_dict_get_rand_word_index(gd);
    return gd->active_words[gd->language_key]->array[r];
}

//agnostic about chars, number, etc.
static bool game_dict_word_at_index_is_valid(game_dict * gd, int index, char * word, dynamic_string_array * sortedknowns) {
    int l_key = gd->language_key;
    debug_print("\n    index valid check: ");
    if (index < 0) return false;
    char * copy = utf_to_ascii(gd->active_words[l_key]->array[index]); // convert accented characters to non-accented characters
    if (!copy) return false; // ascii conversion failed
    if (strlen(copy) != strlen(word)) return false;
    char * lower = to_lower(copy);
    free(copy);
    sort_chars(lower); // sort chars so we can compare against sortedknowns
    int p1 = 0;
    int p2 = 0;
    int count = 0;
    while (p1 < strlen(lower) && count < sortedknowns->num_elements) {
        if (lower[p1] == sortedknowns->array[p2][0]) {
            p2 += 1;
            count += 1;
        }
        p1 += 1;
    }
    free(lower);
    return count == sortedknowns->num_elements;
}

// we could reduce # parameter here (e.g. a "word knowledge" struct, or passing game_status)
// but the former seems unnecessary, and the latter introduces unnecessary coupling
// probably should just be in game_status.c
static char * game_dict_get_eq_word_at_start_index(game_dict * gd, int index, char * word, dynamic_string_array * sortedknowns) {
    char * copy = to_lower(word); 
    debug_print("\nSearching for %s with language key %i", copy, gd->language_key);
    //pthread_mutex_lock(gd->mutex);
    int count = 0;
    while (1) { // in "worst" case, guaranteed to return the starting word
        int num_words = game_dict_num_active_words(gd);
        while (count >= num_words) { // if we loop through all the words in the dict, choose a new language key
            count = 0;
            game_dict_choose_diff_lang_key(gd);
        }
        if (game_dict_word_at_index_is_valid(gd, index, copy, sortedknowns)) {
            free(copy);
            //pthread_mutex_unlock(gd->mutex);
            return gd->active_words[gd->language_key]->array[index];
        }
        index = (index + 1) % num_words;
        count += 1; 
    }
}

char * game_dict_get_equivalent_word(game_dict * gd, char * word, dynamic_string_array * sortedknowns) {
    debug_print("\nGetting random number to start search");
    game_dict_choose_random_lang_key(gd);
    int r = game_dict_get_rand_word_index(gd);
    debug_print("\nGot random number to start search; known letters: ");
    char * new_word = game_dict_get_eq_word_at_start_index(gd, r, word, sortedknowns);
    return new_word;
}

void game_dict_choose_random_lang_key(game_dict * gd) {
    if (game_dict_words_normalized(gd)) {
        debug_print("\nRandomizing language");
        gd->language_key = rand_num(0, gd->dictionary_settings->languages->num_elements-1);
    }
}

void game_dict_choose_diff_lang_key(game_dict * gd) {
    int key = gd->language_key;
    while (gd->language_key == key) {
        game_dict_choose_random_lang_key(gd);
    }
}