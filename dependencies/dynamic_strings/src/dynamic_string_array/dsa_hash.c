/*
dsa_hash.c: code for associating dsa elements with their counts, which improves time complexity for several (multi)set-based operations.
*/

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "dynamic_string_array.h"

static void dsa_free_bucket(dsa_hash_bucket * hb) {
    dsa_hash_bucket * prev;
    while (hb) {
        if (hb->key) free(hb->key);
        prev = hb;
        hb = hb->next;
        free(prev);
    }
}

static void dsa_alloc_one_bucket(dsa_hash_table * ht, unsigned index) {
    ht->hashtable[index] = malloc(sizeof(dsa_hash_bucket));
    ht->hashtable[index]->next = NULL;
    ht->hashtable[index]->key = NULL;   
    ht->hashtable[index]->value = 0;    
}

static void dsa_hash_table_init(dsa_hash_table * ht, unsigned size) {
    ht->hashtable = malloc(sizeof(dsa_hash_bucket *) * size);
    unsigned i;
    for (i = 0; i < size; i++) {
        dsa_alloc_one_bucket(ht, i);
    }
    ht->num_keys_total = size;
    ht->num_keys_used = 0;
}

// I've implemented djb2 for simplicity and because it works well for small data sets
// For larger data sets, xxHash or murmur would be better (as of mid-2021)
static unsigned long dsa_hash(const char * str, unsigned table_size) {
    unsigned long hash = 5381;
    int c;
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash % table_size;
}

static dsa_hash_bucket * hash_table_get_value(dsa_hash_table * ht, const char * key) {
    dsa_hash_bucket * data;
    for (data = ht->hashtable[dsa_hash(key, ht->num_keys_total)]; data != NULL; data = data->next) {
        if (data->key && strcmp(key, data->key) == 0) {
            return data;
        }
    }
    return NULL;
}

static void dsa_expand_table_if_needed(dsa_hash_table * ht) {
    while (ht->num_keys_used >= ht->num_keys_total) {
        ht->num_keys_total += 10;
        ht->hashtable = realloc(ht->hashtable, sizeof(dsa_hash_bucket *) * (ht->num_keys_total));
        int i;
        for (i = 0; i < 10; i++) {
            dsa_alloc_one_bucket(ht, ht->num_keys_total - i);
        }
    }
}

void dsa_hash_table_update(dsa_hash_table * ht, const char * key, int increment) {
    dsa_hash_bucket * toadd = hash_table_get_value(ht, key);
    unsigned hash_val;
    if (toadd == NULL) {
        ht->num_keys_used += 1;
        dsa_expand_table_if_needed(ht);
        toadd = (dsa_hash_bucket *)malloc(sizeof(*toadd)); 
        toadd->key = malloc((strlen(key) + 1) * sizeof(char));
        strcpy(toadd->key, key);
        hash_val = dsa_hash(key, ht->num_keys_total); 
        toadd->next = ht->hashtable[hash_val]; // i.e. set this to the beginning ot the list
        toadd->value = 1;
        ht->hashtable[hash_val] = toadd;
    }
    else {
        toadd->value += increment;
        if (toadd->value == 0) {
            dsa_hash_table_remove(ht, key);
        }
    }
}

void dsa_hash_table_remove(dsa_hash_table * ht, const char * key) {
    dsa_hash_bucket * data = hash_table_get_value(ht, key);
    unsigned hash_val = dsa_hash(key, ht->num_keys_total);
    dsa_hash_bucket * start = ht->hashtable[hash_val];
    if (start == data) {
        ht->hashtable[hash_val] = start->next;
    }
    else {
        while (start) {
            if (start->next == data) {
                start->next = start->next->next;
                break;
            }
            start = start->next;
        }
    }
    if (data->key) free(data->key);
    if (data) free(data);
}

dsa_hash_table * dsa_to_hash_table(dynamic_string_array * dsa) {
    dsa_hash_table * ht = malloc(sizeof(*ht));
    unsigned size = (unsigned)((dsa->num_elements + 1) * 1.3);
    dsa_hash_table_init(ht, size);
    int i;
    for (i = 0; i < dsa->num_elements; i++) {
        dsa_hash_table_update(ht, dsa->array[i], 1);
    }
    return ht;
}

void dsa_hash_table_free(dsa_hash_table * ht) {
    int i;
    for (i = 0; i < ht->num_keys_total; i++) {
        dsa_hash_bucket * data = ht->hashtable[i];
        dsa_free_bucket(data);
    }
    free(ht);    
}

bool dsa_hash_table_contains(dsa_hash_table * ht, const char * key) {
    return hash_table_get_value(ht, key) != NULL;
}

void dsa_hash_table_print(dsa_hash_table * ht) {
    int i;
    for (i = 0; i < ht->num_keys_total; i++) {
        if (ht->hashtable) {
            dsa_hash_bucket * bucket = ht->hashtable[i];
            while (bucket) {
                bucket = bucket->next;
            }
        }
    }
}