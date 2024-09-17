#include "variables.h"
#include "../utils/utils.h"

HashMap root_vars;

static MapEntry *HashMap_find(HashMap *map, String *key, int create_if_not_found) {
	hash_t bucket_idx = HashMap_hash_key(map, key);
	Bucket *bucket = root_vars.buckets[bucket_idx];

}

static void HashMap_init(HashMap *map) {
	map->bucket_count = INITIAL_HASHMAP_SIZE;
	map->buckets = checked_malloc(map->bucket_count * sizeof(Bucket));
}

static void HashMap_free(HashMap *map) {
	free(map->buckets);
	map->buckets = NULL;
	map->bucket_count = 0;
}



SSValue *vm_get_var_reference(String *identifier, int create_if_not_found) {
	MapEntry *entry = HashMap_find(root_vars, identifier, create_if_not_found);
	if (entry == NULL) return NULL;
	return &(entry->value);
}

void vm_variables_init(void) {
	HashMap_init(&root_vars);
}

void vm_variables_destroy(void) {
	HashMap_free(&root_vars);
}
