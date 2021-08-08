#ifndef _Game_Menu_
#define _Game_Menu_

#include "dynamic_strings.h"

typedef struct game_menu {
    game_status * status;
} game_menu;

void game_menu_init(game_menu * gm);

void game_menu_free(game_menu * gm);

void game_menu_play_starting_animation(game_menu * gm);

void game_menu_print_main(game_menu * gm);

void game_menu_handle_input(game_menu * gm, char * input);

void game_menu_handle_invalid_main_input(game_menu *gm);

void game_menu_print_stats(game_menu * gm);

void game_menu_print_rules(game_menu * gm);

void game_menu_start_round(game_menu * gm);

void game_menu_reset_dict(game_menu * gm);

void game_menu_handle_input(game_menu * gm, char * input);

void game_menu_print_language_options(game_menu * gm);

void game_menu_handle_language_input(game_menu * gm, char * input, dynamic_string_array * langs);

void game_menu_handle_stats_input(game_menu * gm, char * input);

dynamic_string_array * game_menu_supported_langs();

#endif