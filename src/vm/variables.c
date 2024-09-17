#include "variables.h"
#include "../utils/utils.h"
#include "../heap/heap.h"
#include <string.h>

HashMap root_vars;

static hash_t HashMap_hash_key(HashMap *map, String *key) {
	hash_t mod = map->bucket_count;
	hash_t p = 131;
	hash_t ppow = p;
	hash_t result = 0;
	for (size_t i = 0; i < key->size; i++) {
		char c = key->data[i];
		result = (result + ppow * (long long)c) % mod;
		ppow = (ppow * (long long)p) % mod;
	}
	result = (result % mod + mod) % mod;
	DBGCHECK(result >= 0 && result < mod);
	return result;
}

static void HashMap_rehash(HashMap *map) {
	HashMap new_map;
	HashMap_init(&new_map, 2 * map->bucket_count);
	for (size_t i = 0; i < map->bucket_count; i++) {
		Bucket *bucket = &map->buckets[i];
		for (size_t j = 0; j < BUCKET_SIZE && bucket->entries[j].key != NULL; j++) {
			HashMap_insert(&new_map, &bucket->entries[j]);
		}
	}
	HashMap_free(map);
	*map = new_map;
}

static MapEntry *HashMap_find(HashMap *map, String *key, int create_if_not_found) {
	size_t i;
	Bucket *bucket;
	do {
		hash_t bucket_idx = HashMap_hash_key(map, key);
		bucket = &root_vars.buckets[bucket_idx];
		for (i = 0; i < BUCKET_SIZE && bucket->entries[i].key != NULL; i++) {
			if (String_cmpstr(key, bucket->entries[i].key) == 0) {
				return &bucket->entries[i];
			}
		}
		if (!create_if_not_found)
			return NULL;
		if (i >= BUCKET_SIZE) {
			HashMap_rehash(map);
		}
	} while (i >= BUCKET_SIZE);
	MapEntry *entry = &bucket->entries[i];
	entry->key = String_new(key->size);
	memcpy(entry->key->data, key->data, key->size);
	entry->value.type = SSVALUE_NUM;	// a newly created entry is set to the number 0
	entry->value.value = 0;
	return entry;
}

void HashMap_insert(HashMap *map, MapEntry *entry) {
	MapEntry *new_entry = HashMap_find(map, entry->key, 1);
	new_entry->value = entry->value;
}

void HashMap_init(HashMap *map, size_t bucket_count) {
	map->bucket_count = bucket_count;
	map->buckets = checked_malloc(map->bucket_count * sizeof(Bucket));
	memset(map->buckets, 0, map->bucket_count * sizeof(Bucket));
}

void HashMap_free(HashMap *map) {
	free(map->buckets);
	map->buckets = NULL;
	map->bucket_count = 0;
}

// free the hashmap and all variables inside it at the end of the program
static void HashMap_destroy(HashMap *map) {
	for (size_t i = 0; i < map->bucket_count; i++) {
		Bucket *bucket = &map->buckets[i];
		for (size_t j = 0; j < BUCKET_SIZE && bucket->entries[j].key != NULL; j++) {
			free(bucket->entries[j].key);
			ss_value_immediately_free(bucket->entries[j].value);
		}
	}
	free(map->buckets);
	map->buckets = NULL;
	map->bucket_count = 0;
}



SSValue *vm_get_var_reference(String *identifier, int create_if_not_found) {
	MapEntry *entry = HashMap_find(&root_vars, identifier, create_if_not_found);
	if (entry == NULL) return NULL;
	return &(entry->value);
}

void vm_variables_init(void) {
	HashMap_init(&root_vars, INITIAL_HASHMAP_SIZE);
}

void vm_variables_destroy(void) {
	HashMap_destroy(&root_vars);
}
