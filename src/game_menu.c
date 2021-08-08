/* 
game_menu.c: The entry point into Entropic Hangman. It should really make use of some abstract menu template that
takes care of input handling in a centralized place; frankly, a lot of the code here is subpar--user input isn't 
handled well, for example, and there is plenty to refactor--but it is functional. Rather than clean it all up, I need 
to move on from this project to learn new things! 
*/

#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include "game_round.h"
#include "game_status.h"
#include "game_dict.h"
#include "game_stats.h"
#include "Levenshteiner.h"
#include "utils.h"
#include "game_menu.h"
#include "dynamic_strings.h"
#include "unicode_helper.h"
#include "str_funcs.h"


static void force_press_enter() {
    getchar();
    getchar(); // hacky
}

void game_menu_init(game_menu * gm) {
    gm->status = malloc(sizeof(game_status));
    game_status_init(gm->status);
    game_dict_load_all_words(gm->status->game_dict); // load here to get random words for start-up animation
    unicode_helper_init();
}

void game_menu_free(game_menu * gm) {
    game_status_free(gm->status);
    unicode_helper_free();
    free(gm);
}

void game_menu_play_starting_animation(game_menu * gm) {
    char * a = game_dict_get_random_word(gm->status->game_dict);
    char * b = game_dict_get_random_word(gm->status->game_dict);
    char * c = game_dict_get_random_word(gm->status->game_dict);
    char * z[4] = {a, b, c, "===Entropic Hangman==="};
    animate_distances(z, 4, 50);
}

// ===Main Menu===

void game_menu_handle_invalid_main_input(game_menu *gm) {
    clear_screen();
    print_and_flush("Please input 1-5");
    delay_in_ms(1000);
    game_menu_print_main(gm);
}

// yuck
void game_menu_handle_input(game_menu * gm, char * input) {
    //handle invalid more properly
    if (strlen(input) > 1) {
        game_menu_handle_invalid_main_input(gm);
        return;
    }
    if (input[0] == '1') {
        game_menu_start_round(gm);
    }
    else if (input[0] == '2') {
        game_menu_print_rules(gm);
    }
    else if (input[0] == '3') {
        game_menu_print_stats(gm);
    }
    else if (input[0] == '4') {
        game_menu_print_language_options(gm);
    }
    else if (input[0] == '*') {
        toggle_debug_mode();
        game_menu_print_main(gm);
    }
    else if (input[0] == '5') {
        clear_screen();
        print_and_flush("Goodbye!\n\n");
        exit(0);
    }
    else {
        game_menu_handle_invalid_main_input(gm);
    }
}

void game_menu_print_main(game_menu * gm) {
    clear_screen();
    printf(GREEN "\n===Entropic Hangman===" RESET);
    printf("\n\n(1) Play\n\n(2) Rules\n\n(3) Stats\n\n(4) Languages\n\n(5) Quit\n\n: ");
    fflush(stdout);
    char input[10];
    scanf("%s", input);
    game_menu_handle_input(gm, input);
}

// ===Rules===

static void game_menu_no_rules_found(game_menu * gm) {
    clear_screen();
    print_and_flush("\n\nCould not locate rules.txt in Entropic_Hangman/");
    delay_in_ms(1000);
    game_menu_print_main(gm);
}

static void game_menu_rules_found(game_menu * gm, FILE * fp) {
    clear_screen();
    char buffer[300];
    while(fgets(buffer, 300, fp)) {
        printf("%s", buffer);
    }
    fflush(stdout);
    fclose(fp);
    print_and_flush("\n\nPress Enter to go back to the main menu\n\n: ");
    force_press_enter();
    game_menu_print_main(gm);
}

void game_menu_print_rules(game_menu * gm) {
    FILE * fp = fopen("./rules.txt", "r");
    if (!fp) {
        game_menu_no_rules_found(gm);
    }
    else {
        game_menu_rules_found(gm, fp);
    }       
}

// ====Stats====

void game_menu_print_stats(game_menu * gm) {
    clear_screen();
    print_and_flush("\n===Entropic Hangman===");
    printf("\n");
    game_stats_print();
    print_and_flush("\n\n(B) Back\n\n(C) Clear Stats\n\n: ");
    char input[10];
    scanf("%s", input);
    game_menu_handle_stats_input(gm, input);
}

void game_menu_handle_stats_input(game_menu * gm, char * input) {
    if (input[0] == 'B' || input[0] == 'b') {
        game_menu_print_main(gm);
    }
    else if (input[0] == 'C' || input[0] == 'c') {
        clear_screen();
        print_and_flush("Are you sure you want to clear the stats? (Y)es (N)o\n\n: ");
        char sure[10];
        scanf("%s", sure);
        if (sure[0] == 'Y' || sure[0] == 'y') {
            game_stats_reset();
            print_and_flush("\nCleared");
            delay_in_ms(1000);
            game_menu_print_stats(gm);
        }
        else {
            game_menu_print_stats(gm);
        }
    }
}

// ===Languages===

dynamic_string_array * game_menu_supported_langs() {
    dynamic_string_array * dsa = malloc(sizeof(dynamic_string_array));
    dsa_init(dsa, 1, 25); // fix
    char path[strlen(DICT_PATH)+1] = DICT_PATH;
    DIR *dir = opendir(path);
    struct dirent *dp;
    while ((dp = readdir(dir)) != NULL) {
        if (dp->d_name[0] == 'd' && dp->d_name[1] == '_') {
            dsa_append(dsa, dp->d_name);
        }
    }
    return dsa;
}

// yuck -- refactor
void game_menu_handle_language_input(game_menu * gm, char * input, dynamic_string_array * langs) {
    if (input[0] == 'B' || input[0] == 'b') {
        dsa_free(langs);
        game_menu_print_main(gm);
    }
    else if (input[0] == 'N' || input[0] == 'n') {
        dsa_free(langs);
        game_dict_toggle_lang_normalization(gm->status->game_dict);
        game_menu_print_language_options(gm);
    }
    else {
        int a  = atoi(input);
        if (a > langs->num_elements) {
            game_menu_print_language_options(gm);
            dsa_free(langs);
        }
        else {
            char * message = game_dict_toggle_lang_or_ret_err_message(gm->status->game_dict, langs->array[a]);
            if (message) {
                print_and_flush("\nCannot remove--you need at least one active dictionary!");
                delay_in_ms(1500);
            }
            dsa_free(langs);
            game_menu_print_language_options(gm);
        }
    }
}

// yuck -- refactor
void game_menu_print_language_options(game_menu * gm) {
    clear_screen();
    print_and_flush("\n===Entropic Hangman===");
    char * s = game_dict_words_normalized(gm->status->game_dict) ? "(on)" : "(off)";
    print_and_flush("\n\n(B) Back\n\n(N) Uniform selection across languages %s\n", s);
    int i, j;
    dynamic_string_array * langs = game_menu_supported_langs();
    for (i = 0; i < langs->num_elements; i++) {
        for (j = 0; j < 2; j++) remove_char_at_index(langs->array[i], 0); // remove d_
        for (j = 0; j < 4; j++) remove_char_at_index(langs->array[i], strlen(langs->array[i])-1); // remove .txt
        printf("\n(%i) %s", i, langs->array[i]);
        if (game_dict_lang_is_active(gm->status->game_dict, langs->array[i])) printf(" (remove)");
        else printf(" (add)");
    }
    print_and_flush("\n\n: ");
    char input[10];
    scanf("%s", input);
    game_menu_handle_language_input(gm, input, langs);
}

// ===Starting the Game===

void game_menu_start_round(game_menu * gm) {
    game_status_refresh(gm->status);
    game_round_start_round(gm->status);
    print_and_flush("\nPress Enter to continue");
    force_press_enter();
    game_menu_print_main(gm);
}

void start_game(game_menu * gm) {
    game_menu_init(gm);
    game_menu_play_starting_animation(gm);
    printf("\n");
    fflush(stdout);
    delay_in_ms(200);
}

int main() {
    game_menu * gm = malloc(sizeof(* gm));
    start_game(gm);
    //debug = true;
    game_menu_print_main(gm);
    game_menu_free(gm);
}



