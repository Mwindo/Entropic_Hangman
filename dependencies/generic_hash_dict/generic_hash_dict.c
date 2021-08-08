/*
generic_hash_dict.c: an naive, on-the-nonce hash table implementation
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "generic_hash_dict.h"

void hash_dict_init(hash_dict * hd, int size, int key_type, int val_type) {
    hd->hashtable = malloc(sizeof(bucket *) * size);
    int i;
    for (i = 0; i < size; i++) {
        hd->hashtable[i] = malloc(sizeof(bucket));
        hd->hashtable[i]->next = NULL;
        hd->hashtable[i]->key = NULL;
        hd->hashtable[i]->value = NULL;
    }
    hd->size = size;
    hd->key_type = key_type;
    hd->value_type = val_type;
}

void hash_dict_free(hash_dict * hd) {
    int i;
    for (i = 0; i < hd->size; i++) {
        bucket * data = hd->hashtable[i];
        while (data) {
            if (data->key) free(data->key);
            if (data->value) free(data->value);
            bucket * copy = data;
            data = data->next;
            free(copy);
        }
    }
    free(hd);
}

static int djb2(const char * str, int table_size) {
    int hash = 5381;
    int c;
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash % table_size;
}

int hash(const void * key, const hash_dict * hd) {
    switch(hd->key_type) {
        case CHAR_TYPE || STR_TYPE:
            return djb2(key, hd->size);
        case INT_TYPE:
            return 1; // could do this later, but only use case for now is char/str
        case FLOAT_TYPE:
            return 1; // could do this later
    }
    return 1;
}

bool keys_equivalent(const void * key1, const void * key2, int key_type) {
    if (!key2) return false;
    switch (key_type) {
        case 0:
            return *(char *)key1 == *(char *)key2;
        case 1 :
            return strcmp((char *)key1, (char *)key2) == 0;
        case 2:
        case 3:
            return *(int *)key1 == *(int *)key2;
    }
    return false; // improve
}

int type_size(int type) {
    switch (type) {
        case 0:
        case 1:
            return sizeof(char);
        case 2:
            return sizeof(int);
        case 3:
            return sizeof(float);
    }
    return 0; // improve
}

int key_size(const void * key, int type) {
    switch (type) {
        case 0:
            return type_size(type);
        case 1:
            return (strlen(key) + 1) * sizeof(char);
        case 2:
        case 3:
            return type_size(type);
    }
    return 0; // improve
}

bucket * hash_dict_get_value(const hash_dict * hd, const void * key) {
    bucket * data;
    for (data = hd->hashtable[hash(key, hd)]; data != NULL; data = data->next) {
        if (keys_equivalent(key, data->key, hd->key_type)) {
            return data;
        }
    }
    return NULL;
}

void hash_dict_add(hash_dict * hd, void * key, void * value) {
    bucket * toadd = hash_dict_get_value(hd, key);
    int hash_val;
    if (toadd == NULL) {
        toadd = (bucket *)malloc(sizeof(*toadd));
        toadd->key = malloc(key_size(key, hd->key_type));
        memcpy(toadd->key, key, key_size(key, hd->key_type));
        hash_val = hash(key, hd);
        toadd->next = hd->hashtable[hash_val]; // i.e. set this to the beginning ot the list
        toadd->value = value;
        hd->hashtable[hash_val] = toadd;
    }
}

void hash_dict_add_char(hash_dict * hd, char key, char value) {
    char *a = malloc(sizeof(char));
    char *b = malloc(sizeof(char));
    *a = key;
    *b = value;
    fflush(stdout);
    hash_dict_add(hd, a, b);
}


/*
void test3() {
    hash_dict * hd = malloc(sizeof(hash_dict));
    hash_dict_init(hd, 10, 2, 2);
    int * a = malloc(sizeof(int));
    *a = 1;
    int * b = malloc(sizeof(int));
    *b = 5;
    hash_dict_add(hd, a, b);
    hash_dict_add(hd, b, a);
    bucket * data = hash_dict_get_value(hd, a);
    bucket * data1 = hash_dict_get_value(hd, b);
}

void test2() {
    hash_dict * hd = malloc(sizeof(hash_dict));
    hash_dict_init(hd, 10, 1, 1);
    char * a = malloc(sizeof(char) * 4);
    a[0] = 'a';
    a[1] = 'b';
    a[2] = 'c';
    a[3] = '\0';
    char * b = malloc(sizeof(char) * 2);
    b[0] = 'n';
    b[1] = '\0';
    hash_dict_add(hd, a, b);
    hash_dict_add(hd, b, a);
    bucket * data = hash_dict_get_value(hd, a);
    printf("\nvalue for %s is %s", a, (char*)data->value);
    fflush(stdout);
    bucket * data1 = hash_dict_get_value(hd, b);
    printf("\nvalue for %s is %s", b, (char*)data1->value);
    fflush(stdout);
}

int main() {
    test2();
}
*/