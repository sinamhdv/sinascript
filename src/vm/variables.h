#include "../objects/ss-value.h"
#include "../types/string-view.h"

#ifndef MYHEADER_VARIABLES_H
#define MYHEADER_VARIABLES_H

#define INITIAL_HASHMAP_SIZE 1033
#define BUCKET_SIZE 8

typedef int hash_t;

typedef struct MapEntry {
	String *key;
	SSValue value;
} MapEntry;

typedef struct Bucket {
	MapEntry entries[BUCKET_SIZE];
} Bucket;

typedef struct HashMap {
	size_t bucket_count;
	Bucket *buckets;
} HashMap;

static void HashMap_insert(HashMap *map, MapEntry *entry);
static void HashMap_init(HashMap *map, size_t bucket_count);
static void HashMap_free(HashMap *map);

void vm_variables_init(void);
void vm_variables_destroy(void);

// returns NULL if create_if_not_found == 0 and the identifier is not found
SSValue *vm_get_var_reference(String *identifier, int create_if_not_found);

#endif
