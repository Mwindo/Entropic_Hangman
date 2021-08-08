/*
game_status.c: code for storing information for each round, like the number of guesses, the known letters, the current word, etc.
*/

#include <string.h>
#include <stdlib.h>
#include "unicode_helper.h"
#include "utils.h"
#include "game_dict.h"
#include "dynamic_strings.h"
#include "str_funcs.h"
#include "game_status.h"

void game_status_init(game_status * gs) {
    gs->num_overall_wrong_guesses = 0;
    gs->max_overall_wrong_guesses = MAX_OVERALL_WRONG_GUESSES;
    gs->guesses = malloc(sizeof(dynamic_string_array));
    gs->known_letters = malloc(sizeof(dynamic_string_array));
    gs->word_history = malloc(sizeof(dynamic_string_array));
    dsa_init(gs->guesses, 5, 25);
    dsa_init(gs->known_letters, 10, 25);
    dsa_init(gs->word_history, 10, 25);
    gs->game_dict = malloc(sizeof(game_dict));
    game_dict_init(gs->game_dict);
}

void game_status_free(game_status * gs) {
    dsa_free(gs->guesses);
    dsa_free(gs->known_letters);
    dsa_free(gs->word_history);
    game_dict_free(gs->game_dict);
    free(gs);
}

void game_status_refresh(game_status * gs) {
    debug_print("\nGame Status refreshing; %i", gs->game_dict->needs_reloading);
    if (gs->game_dict->needs_reloading) {
        debug_print("\nResetting Game Dictionary");
        game_dict_reset(gs->game_dict);
    }
    else {
        debug_print("\nResetting active words in Game Dictionary");
        game_dict_reset_active_words(gs->game_dict);
    }
    debug_print("\nRefreshing game status");
    dsa_clear(gs->guesses);
    debug_print("\nCleared guesses");
    dsa_clear(gs->known_letters);
    debug_print("\nCleared known letters");
    dsa_clear(gs->word_history);
    debug_print("\nCleared word history");
    fflush(stdout);
    gs->num_overall_wrong_guesses = 0;
}

void game_status_refresh_guesses(game_status * gs) {
    debug_print("\nGuesses: %i", gs->guesses->num_elements);
    dsa_clear(gs->guesses);
}

void game_status_update_guesses(game_status * gs, const char * guess) {
    dsa_append(gs->guesses, guess);
}

char * game_status_current_word(const game_status * gs) {
    if (gs->word_history->num_elements > 0) {
        return gs->word_history->array[0];
    }
    return "";
}

dynamic_string_array * game_status_unknown_letters(const game_status * gs) {
    char * word = to_lower(game_status_current_word(gs));
    dynamic_string_array * dsa_word = dsa_create_from_string(word);
    dynamic_string_array * ret = dsa_multiset_difference(dsa_word, gs->known_letters);
    free(word);
    dsa_free(dsa_word);
    return ret;
}

void game_status_set_current_word(game_status * gs, const char * word) {
    debug_print("\nInserting %s into word history", word);
    char * ascii = utf_to_ascii(word);
    debug_print("\nConverted to %s", ascii);
    if (ascii) {
        dsa_insert(gs->word_history, 0, ascii);
        debug_print("\nNum elements in word history: %i", gs->word_history->num_elements);
    }
    else {
        print_and_flush("\nError: invalid character");
        fflush(stdout);
        exit(-1);
    }
}

void game_status_set_first_word(game_status * gs) {
    game_status_set_current_word(gs, game_dict_get_random_word(gs->game_dict));
    /*
    I had originally used a background thread to remove invalid words from the active word list.
    In Python, this sped things up. In C, not so much.
    pthread_t tid;
    int x = pthread_create(&tid, NULL, &game_dict_prune_words_based_on_length, (void *)gs);
    pthread_detach(tid);
    */
}

void game_status_set_non_first_word(game_status * gs) {
    game_status_set_current_word(gs, game_dict_get_equivalent_word(gs->game_dict, game_status_current_word(gs), gs->known_letters));
    /*
    I had originally used a background thread to remove invalid words from the active word list.
    In Python, this sped things up. In C, not so much.
    pthread_t tid;
    int x = pthread_create(&tid, NULL, &game_dict_prune_words_based_on_length, (void *)gs);
    pthread_detach(tid);
    pthread_t tid;
    int x = pthread_create(&tid, NULL, &game_dict_prune_words_based_on_known_letters, (void *)gs);
    pthread_detach(tid);
    */
}

void game_status_update_known_letters(game_status * gs, char letter) {
    debug_print("\nUpdating known letters");
    char * c = convert_char_to_string(letter);
    char * lower = to_lower(c);
    free(c);
    debug_print("\nLetter: %s", lower);
    dsa_append_in_sorted_order(gs->known_letters, lower);
    if (debug) dsa_print(gs->known_letters, "\n");
    debug_print("\n");
    free(lower);
}

void game_status_increment_num_wrong_guesses(game_status * gs) {
    gs->num_overall_wrong_guesses += 1;
}

bool lost_game(const game_status * gs) {
    return gs->num_overall_wrong_guesses == gs->max_overall_wrong_guesses;
}

bool won_game(const game_status * gs) {
    return gs->known_letters->num_elements == strlen(game_status_current_word(gs));
}

bool word_is_current_word(const game_status * gs, const char * word) {
    char * w1 = to_lower(game_status_current_word(gs));
    char * w2 = to_lower(word);
    int v = strcmp(w1, w2);
    free(w1);
    free(w2);
    return v == 0;
}