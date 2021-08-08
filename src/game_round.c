/*
game_round.c: one round of Entropic Hangman. Chooses/randomizes words, prints the display, and handles guesses
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include "str_funcs.h"
#include "game_round.h"
#include "game_status.h"
#include "game_stats.h"
#include "utils.h"
#include "game_dict.h"
#include "dynamic_strings.h"

//for testing
static char random_letter() { 
    int x = rand_num(0, 26);
    char lets[] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'};
    return lets[x];
}

void game_round_start_round(game_status * gs) {
    if (!debug) clear_screen();
    print_and_flush("\n\nStarting a new game . . . \n\n");
    delay_in_ms(500);
    game_round_refresh_word(gs);
}

void game_round_refresh_word(game_status * gs) {
    debug_print("\nGetting new word");
    game_status_refresh_guesses(gs);
    if (word_is_current_word(gs, "")) {
        debug_print("\n  Word is empty, so getting first word");
        game_status_set_first_word(gs);
        debug_print("\n  Set the word");
    }
    else {
        debug_print("\n  Word is nonempty, so getting equivalent word");
        game_status_set_non_first_word(gs);
        debug_print("\n  Set the word");
    }
    debug_print("\n  Refreshing guesses");
    game_status_refresh_guesses(gs);
    debug_print("\nDone getting new word");
    game_round_print_status_and_prompt(gs);
}

static void game_round_print_debug_stuff(const game_status * gs) { 
    if (debug) {
        debug_print("Current word: %s", game_status_current_word(gs));
        dsa_print(gs->known_letters, "\n");
    }
}

void game_round_print_status_and_prompt(game_status * gs) {
    if (!debug) clear_screen();
    game_round_print_debug_stuff(gs);
    print_and_flush("Number of guesses: %i/%i", gs->num_overall_wrong_guesses, gs->max_overall_wrong_guesses);
    print_and_flush("\nWrong guesses: ");
    dsa_print(gs->guesses, " ");
    print_and_flush("\n\n");
    game_round_print_display_row(gs);
    print_and_flush("\nYour guess: ");
    char input[DEFAULT_MAX_WORD_LENGTH + 1];
    scanf("%s", input);
    //print_and_flush(input);
    //char * c = convert_char_to_string(random_letter()); // for testing
    game_round_handle_guess(gs, input);
}

void game_round_print_display_row(const game_status * gs) {
    debug_print("\nPrinting display row");
    char * word = game_status_current_word(gs);
    dynamic_string_array * known_letters = dsa_copy(gs->known_letters);
    char s[strlen(word) * 3 + 1]; // each letter will be allocated three char spaces
    int i, k;
    for (i = 0, k=0; i < strlen(word); i++, k+=3) { // loop through word and display it if it is among the known letters
        char * c = convert_char_to_string(word[i]);
        char * lower = to_lower(c);
        free(c);
        bool in = dsa_contains(known_letters, lower);
        s[k] = in ? ' ' : '_';
        s[k+1] = in ? word[i] : '_';
        s[k+2] = ' ';
        if (in) dsa_remove(known_letters, lower); // don't double count, i.e., if only 1 e known, only display 1 e even if there are more
        free(lower);
    }
    debug_print("\nDone printing display row");
    s[strlen(word) * 3] = '\0';
    printf("\n%s\n", s);
}

void game_round_handle_guess(game_status * gs, const char * guess) {
    if (!game_round_guess_is_proper(guess)) {
        game_round_handle_improper_guess(gs, guess);
    }
    else if (game_round_guess_is_redundant(gs, guess)) {
        game_round_handle_redundant_guess(gs, guess);
    }
    else {
        game_round_handle_ok_guess(gs, guess);
    }
}

bool game_round_guess_is_proper(const char * guess) {
    //implement with regex using custom regex library
    int i;
    for (i = 0; i < strlen(guess); i++) {
        if (!isalpha(guess[i]) && guess[i] != '-' && guess[i] != ' ') {
            return false;
        }
    }
    return true;
}

bool game_round_guess_is_redundant(const game_status * gs, const char * guess) {
    char * lower = to_lower(guess);
    bool ret = dsa_contains(gs->guesses, guess);
    free(lower);
    return ret;
}

void game_round_handle_improper_guess(game_status * gs, const char * guess) {
    print_and_flush("\nThat guess doesn't make sense! Guess a letter or a word.");
    delay_in_ms(2000);
    game_round_print_status_and_prompt(gs);
}

void game_round_handle_redundant_guess(game_status * gs, const char * guess) {
    print_and_flush("\nYou have already guessed %s", guess);
    delay_in_ms(1000);
    game_round_print_status_and_prompt(gs);
}

void game_round_handle_ok_guess(game_status * gs, const char * guess) {
    debug_print("\nGuess ok\n");
    game_status_update_guesses(gs, guess);
    if (game_round_guess_is_word(guess)) {
        debug_print("\nGuess is a word\n");
        game_round_handle_word_guess(gs, guess);
    }
    else {
        debug_print("\nGuess is a letter\n");
        game_round_handle_letter_guess(gs, guess[0]);
    }
}

static void gr_handle_letter_guess_results(game_status * gs, char guess, int count) {
    if (count > 0) {
        if (!debug) clear_screen();
        char * is_are = (count > 1) ? "are" : "is";
        char s = (count > 1) ? 's' : '\0';
        print_and_flush("\nYes, there %s %i %c%c . . .\n", is_are, count, guess, s);
        game_round_handle_correct_letter_guess(gs, guess, count);
    }
    else {
        game_round_handle_incorrect_letter_guess(gs, guess);
    }
}

void game_round_handle_letter_guess(game_status * gs, char guess) {
    debug_print("\nGuess is %c\n", guess);
    dynamic_string_array * copy = dsa_copy(game_status_unknown_letters(gs));
    int i, count;
    count = 0;
    for (i = 0; i < copy->num_elements; i++) {
        char * l = to_lower(copy->array[i]);
        if (l[0] == tolower(guess)) count += 1;
        free(l);
    }
    gr_handle_letter_guess_results(gs, guess, count);
}

void game_round_handle_correct_letter_guess(game_status * gs, char guess, int count) {
    int i;
    for (i = 0; i < count; i++) {
        game_status_update_known_letters(gs, tolower(guess));
    }
    if (won_game(gs)) {
        game_round_handle_win(gs);
    }
    else {
        game_round_handle_nonwinning_correct_guess(gs, guess);
    }
}

bool game_round_guess_is_word(const char * guess) {
    return strlen(guess) != 1;
}

void game_round_handle_word_guess(game_status * gs, const char * guess) {
    if (word_is_current_word(gs, guess)) {
        game_round_handle_win(gs);
    }
    else {
        game_round_handle_incorrect_word_guess(gs, guess);
    }
}

void game_round_handle_nonwinning_correct_guess(game_status * gs, const char guess) {
    print_and_flush("\nPresenting new word . . .\n");
    if (!debug) {
        delay_in_ms(1000);
        clear_screen();
    }
    //animate
    game_round_refresh_word(gs);
}

void game_round_handle_win(const game_status * gs) {
    print_and_flush("\nYep! The word was ");
    game_round_print_word_history(gs);
    print_and_flush("\n");
    game_stats_update(1);
}

void game_round_handle_incorrect_letter_guess(game_status * gs, char guess) {
    game_status_increment_num_wrong_guesses(gs);
    if (lost_game(gs)) {
        game_round_handle_lost_game(gs);
    }
    else {
        game_round_print_status_and_prompt(gs);
    }
}

void game_round_handle_incorrect_word_guess(game_status * gs, const char * guess) {
    game_status_increment_num_wrong_guesses(gs);
    if (lost_game(gs)) {
        game_round_handle_lost_game(gs);
    }
    else {
        game_round_print_status_and_prompt(gs);
    }
}

void game_round_handle_lost_game(const game_status * gs) {
    print_and_flush("\nNice try! The word was ");
    game_round_print_word_history(gs);
    game_stats_update(0);
    print_and_flush("\n");
}

void game_round_print_word_history(const game_status * gs) {
    char s[gs->word_history->num_elements * DEFAULT_MAX_WORD_LENGTH];
    strcpy(s, gs->word_history->array[0]);
    int i;
    for (i = 1; i < gs->word_history->num_elements; i++) {
        strcat(s, " <- ");
        strcat(s, gs->word_history->array[i]);
    }
    print_and_flush("%s", s);
}