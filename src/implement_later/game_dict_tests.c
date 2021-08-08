bool test_dict_init() {
    game_dict * gd = malloc(sizeof(game_dict));
    game_dict_init(gd);
    //game_dict_free(gd);
    return true;
}

bool test_dict_load() {
    game_dict * gd = malloc(sizeof(game_dict));
    game_dict_init(gd);
    game_dict_load_words(gd);
    printf("\nword: %s\n", gd->active_words[0]->array[501]);
    return true;
}

bool test_dict_load_equivalent_word() {
    game_dict * gd = malloc(sizeof(game_dict));
    game_dict_init(gd);
    game_dict_load_words(gd);
    printf("\nword: %s\n", gd->active_words[0]->array[501]);
    dynamic_string_array * dsa = malloc(sizeof(dynamic_string_array));
    dsa_init(dsa, 10, 25);
    dsa_append(dsa, "a");
    dsa_append(dsa, "a");
    dsa_append(dsa, "b");
    dsa_append(dsa, "c");
    dsa_append(dsa, "n");
    dsa_append(dsa, "o");
    dsa_append(dsa, "s");
    printf("\nnum elements: %i", gd->active_words[0]->num_elements);
    char * new_word = game_dict_get_equivalent_word(gd, gd->active_words[0]->array[501], dsa);
    printf("\nnew word: %s", new_word);
    printf("\nnum elements: %i", gd->active_words[0]->num_elements);
    return true;
}

bool test_dict_load_lang() {
    game_dict * gd = malloc(sizeof(game_dict));
    game_dict_init(gd);
    //game_dict_add_language(gd, "français");
    //game_dict_remove_language(gd, "français");
    //game_dict_add_language(gd, "français");
    game_dict_load_words(gd);
    printf("\nword: %s\n", gd->active_words[0]->array[501]);
    dynamic_string_array * dsa = malloc(sizeof(dynamic_string_array));
    dsa_init(dsa, 10, 25);
    dsa_append(dsa, "a");
    dsa_append(dsa, "a");
    dsa_append(dsa, "b");
    dsa_append(dsa, "c");
    dsa_append(dsa, "n");
    dsa_append(dsa, "o");
    dsa_append(dsa, "s");
    printf("\nnum elements: %i", gd->active_words[0]->num_elements);
    char * new_word = game_dict_get_equivalent_word(gd, gd->active_words[0]->array[501], dsa);
    printf("\nnew word: %s", new_word);
    printf("\nnum elements: %i", gd->active_words[0]->num_elements);
    return true;
}