#ifndef _Game_Round_
#define _Game_Round_

#include "game_status.h"

void game_round_start_round(game_status * gs);

// gets a new word (if possible) prints the display and prompt
void game_round_refresh_word(game_status * gs);

void game_round_print_status_and_prompt(game_status * gs);

void game_round_print_display_row(const game_status * gs);

void game_round_handle_guess(game_status * gs, const char * guess);

bool game_round_guess_is_proper(const char * guess);

bool game_round_guess_is_redundant(const game_status * gs, const char * guess);

void game_round_handle_improper_guess(game_status * gs, const char * guess);

void game_round_handle_redundant_guess(game_status * gs, const char * guess);

void game_round_handle_ok_guess(game_status * gs, const char * guess);

void game_round_handle_letter_guess(game_status * gs, char guess);

void game_round_handle_correct_letter_guess(game_status * gs, char guess, int count);

bool game_round_guess_is_word(const char * guess);

void game_round_handle_word_guess(game_status * gs, const char * guess);

void game_round_handle_nonwinning_correct_guess(game_status * gs, char guess);

void game_round_handle_win(const game_status * gs);

void game_round_handle_incorrect_letter_guess(game_status * gs, char guess);

void game_round_handle_incorrect_word_guess(game_status * gs, const char * guess);

void game_round_handle_lost_game(const game_status * gs);

// prints the word history on a win/loss (e.g. cat -> has -> ham)
void game_round_print_word_history(const game_status * gs);

#endif