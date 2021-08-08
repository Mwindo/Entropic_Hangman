/*
game_dict_lang_handling.c: code for toggling active/inactive languages
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "utils.h"
#include "game_dict.h"
#include "dynamic_strings.h"


bool game_dict_words_normalized(const game_dict * gd) {
    return gd->dictionary_settings->normalize_languages;
}

bool game_dict_lang_is_active(const game_dict * gd, const char * language) {
    return dsa_contains(gd->dictionary_settings->languages, language);
}

int game_dict_lang_key(const game_dict * gd, const char * language) {
    if (!game_dict_words_normalized(gd)) return 0;
    int i;
    for (i = 0; i < gd->dictionary_settings->languages->num_elements; i++) {
        if (strcmp(language, gd->dictionary_settings->languages->array[i]) == 0) return i;
    }
    return -1;
}

FILE * game_dict_get_lang_fp(const char * language) {
    char * path = game_dict_get_lang_path(language);
    FILE * fp;
    fp = fopen(path, "r"); 
    if (fp == NULL) {
        printf("\nFile %s not found", path);
        exit(-1);
    }
    free(path);
    return fp;
}

char * game_dict_get_lang_path(const char * language) {
    char * path = malloc(sizeof(char) * ((strlen("/.txt") + strlen(DICT_PATH)+strlen(DICT_PREFIX)) + strlen(language) + 1)); 
    strcpy(path, DICT_PATH);
    strcat(path, "/");
    strcat(path, DICT_PREFIX);
    strcat(path, language);
    strcat(path, ".txt");
    printf("%s", path);
    return path;
}

bool game_dict_add_lang(game_dict * gd, const char * language) {
    if (!dsa_contains(gd->dictionary_settings->languages, language)) {
        dsa_append(gd->dictionary_settings->languages, language);
        gd->needs_reloading = true;
        return true;
    }
    return false;
}

bool game_dict_remove_lang(game_dict * gd, const char * language) {
    debug_print("\nRemoving language %s", language);
    if (gd->dictionary_settings->languages->num_elements > 1 && dsa_contains(gd->dictionary_settings->languages, language)) {
        dsa_remove(gd->dictionary_settings->languages, language);
        gd->needs_reloading = true;
        return true;
    }
    return false;
}

char * game_dict_toggle_lang_or_ret_err_message(game_dict * gd, const char * language) {
    if (game_dict_lang_is_active(gd, language)) {
        if (!game_dict_remove_lang(gd, language)) return "Need at least one language active";
    }
    else {
        if (!game_dict_add_lang(gd, language)) return "Max language limit reached!";
    }
    return NULL;
}

void game_dict_toggle_lang_normalization(game_dict * gd) {
    gd->dictionary_settings->normalize_languages = ! gd->dictionary_settings->normalize_languages;
    gd->needs_reloading = true;
}