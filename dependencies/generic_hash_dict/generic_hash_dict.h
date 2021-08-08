#ifndef _Generic_Hash_Dict_
#define _Generic_Hash_Dict_

#define HASHSIZE 101
#define CHAR_TYPE 0
#define STR_TYPE 1
#define INT_TYPE 2
#define FLOAT_TYPE 3

typedef struct bucket {
    struct bucket * next;
    void * key;
    void * value;
} bucket;

typedef struct hash_dict {
    bucket ** hashtable;
    int size;
    int key_type;
    int value_type;
} hash_dict;

void hash_dict_init(hash_dict * hd, int size, int key_type, int val_type);

void hash_dict_add_char(hash_dict * hd, char key, char value);

void hash_dict_free(hash_dict * hd);

int hash(const void * key, const hash_dict * hd);

bool keys_equivalent(const void * key1, const void * key2, int key_type);

int type_size(int type);

int key_size(const void * key, int type);

bucket * hash_dict_get_value(const hash_dict * hd, const void * key);

void hash_dict_add(hash_dict * hd, void * key, void * value);

#endif