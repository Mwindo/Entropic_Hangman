#ifndef _Game_Status_
#define _Game_Status_

#include "game_dict.h"
#include "dynamic_strings.h"

#define MAX_OVERALL_WRONG_GUESSES 8

typedef struct game_status {
    game_dict * game_dict;
    dynamic_string_array * guesses;
    dynamic_string_array * word_history;
    dynamic_string_array * known_letters;
    int num_overall_wrong_guesses;
    int max_overall_wrong_guesses;
} game_status;

void game_status_init(game_status * gs);

void game_status_free(game_status * gs);

void game_status_refresh(game_status * gs);

void game_status_refresh_guesses(game_status * gs);

void game_status_update_guesses(game_status * gs, const char * guess);

char * game_status_current_word(const game_status * gs);

bool word_is_current_word(const game_status * gs, const char * word);

dynamic_string_array * game_status_unknown_letters(const game_status * gs);

void game_status_set_current_word(game_status * gs, const char * word);

void game_status_set_first_word(game_status * gs);

void game_status_set_non_first_word(game_status * gs);

void game_status_update_known_letters(game_status * gs, char letter);

void game_status_increment_num_wrong_guesses(game_status * gs);

bool lost_game(const game_status * gs);

bool won_game(const game_status * gs);

#endif