#ifndef HMAP_H
#define HMAP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

#define HM_INICAP 8
#define HM_LOADFACTOR 0.75
#define HM_STRING 0

typedef struct {
    void* key;
    void* value;
    size_t hash; // Storing the hash for fast compare & fast copying/resizing
    bool deleted;
} Slot;

typedef struct {
    size_t key_size;
    size_t value_size;
    size_t used_cnt;
    size_t deleted_cnt;
    size_t capacity;
    Slot* htable;
} Hmap;

typedef Hmap Hset;

/* TODO:
- Add set operations
    bool hm_in(Hmap hm, void* key) (should be hm_search != NULL)
    maybe facilitate set creation instead of having NULL values
- More consistency on Hmap* v Hmap
- Add an ordered iterator (3 options):
    1) pointer to node in a linked list stored in each slot
    2) maintain a dynamic array of ordered slots (more costly in case of deletes but more cache friendly)
    3) create the iterator with a function, calling next will fetch next valid index (most simple and saves mem but does not allow for insertion order preservation)
*/

// Hashmap functions
Hmap* hm_create(size_t key_size, size_t value_size);
void hm_free_slots(size_t n, Slot slots[n]);
void hm_free(Hmap* hm);
size_t hm_hash_string(void* key);
size_t hm_hash_bytes(void* key, size_t size);
void _hm_resize(Hmap* hm, size_t new_capacity);
void* hm_insert_hashed(Hmap* hm, size_t hash, void* key, void* value);
void hm_slots_reloc(Hmap* hm, size_t nsource, Slot source[nsource]);
Hmap* hm_copy(Hmap hm);
int _hm_keycmp(Hmap hm, void* k1, void* k2);
void* hm_insert(Hmap* hm, void* key, void* value);
bool _hm_get_index(Hmap hm, void* key, size_t* index);
void* hm_search(Hmap hm, void* key);
bool hm_remove(Hmap* hm, void* key);
size_t hm_length(Hmap hm);
// Hashset functions (TODO: might #define instead)
Hset* hs_create(size_t key_size);
void hs_free(Hset* hs);
void* hs_insert(Hset* hs, void* key);
bool hs_remove(Hset* hs, void* key);
bool hs_contains(Hset hs, void* key);
size_t hs_length(Hset hs);

#endif // HMAP_H

#ifdef HMAP_IMPLEMENTATION

Hmap* hm_create(size_t key_size, size_t value_size) {
    Hmap* hm = calloc(1, sizeof(*hm));
    assert(hm != NULL);
    hm->capacity = HM_INICAP;
    hm->htable = calloc(hm->capacity, sizeof(Slot));
    assert(hm->htable != NULL);
    hm->key_size = key_size;
    hm->value_size = value_size;
    return hm;
}

// TODO: Will change if iterator LL implemented
void hm_free_slots(size_t n, Slot slots[n]) {
    for (size_t i = 0; i < n; ++i) {
        free(slots[i].key);
        free(slots[i].value);
    }
}

void hm_free(Hmap* hm) {
    hm_free_slots(hm->capacity, hm->htable);
    free(hm->htable);
    free(hm);
}

// TODO: DJB2, replace with Siphash, or maybe smth bespoke for strings
size_t hm_hash_string(void* key) {
    char* _key = key;
    size_t hashed = 5381;
    while (*_key != '\0')
        hashed = ((hashed << 5) + hashed) + *(_key++);
    return hashed;
}

// TODO: DJB2, replace with Siphash
size_t hm_hash_bytes(void* key, size_t size) {
    char* _key = key;
    size_t hashed = 5381;
    while (size--)
        hashed = ((hashed << 5) + hashed) + *(_key++);
    return hashed;
}

void _hm_resize(Hmap* hm, size_t new_capacity) {
    size_t prev_capacity = hm->capacity;
    Slot* prev_htable = hm->htable;
    hm->capacity = new_capacity;
    hm->htable = calloc(new_capacity, sizeof(Slot));
    hm->used_cnt = 0;
    hm_slots_reloc(hm, prev_capacity, prev_htable);
    hm_free_slots(prev_capacity, prev_htable);
    free(prev_htable);
}

void* hm_insert_hashed(Hmap* hm, size_t hash, void* key, void* value) {
    if (hm->used_cnt + hm->deleted_cnt > hm->capacity * HM_LOADFACTOR)
        _hm_resize(hm, hm->capacity * 2);
    if (hm->deleted_cnt > 0 && hm->used_cnt + hm->deleted_cnt < hm->capacity * (1 - HM_LOADFACTOR))
        _hm_resize(hm, hm->capacity / 2);

    size_t index = hash & (hm->capacity - 1);
    Slot slot = hm->htable[index];
    bool replace = false;
    while (slot.key != NULL || slot.deleted) {
        if (hash == slot.hash && _hm_keycmp(*hm, key, slot.key) == 0) {
            replace = true;
            break;
        }
        index = (index + 1) % hm->capacity;
        slot = hm->htable[index];
    }

    if (slot.deleted)
        --hm->deleted_cnt;

    // Value
    if (hm->value_size == HM_STRING) {
        free(hm->htable[index].value);
        hm->htable[index].value = strdup(value);
    } else {
        if (!replace)
            hm->htable[index].value = malloc(hm->value_size);
        memcpy(hm->htable[index].value, value, hm->value_size);
    }
    // Key
    if (!replace) {
        hm->htable[index].hash = hash;
        ++hm->used_cnt;
        if (hm->key_size == HM_STRING) {
            hm->htable[index].key = strdup(key);
        } else {
            hm->htable[index].key = malloc(hm->key_size);
            memcpy(hm->htable[index].key, key, hm->key_size);
        }
    }
    return hm->htable[index].value;
}

void hm_slots_reloc(Hmap* hm, size_t nsource, Slot source[nsource]) {
    // TODO: Might change if iterator LL implemented
    for (size_t i = 0; i < nsource; ++i) {
        if (source[i].key != NULL && !source[i].deleted) {
            hm_insert_hashed(hm, source[i].hash, source[i].key, source[i].value);
        }
    }
}

Hmap* hm_copy(Hmap hm) {
    Hmap* new_hm = calloc(1, sizeof(Hmap));
    new_hm->key_size = hm.key_size;
    new_hm->value_size = hm.value_size;
    new_hm->capacity = hm.capacity;
    new_hm->htable = calloc(new_hm->capacity, sizeof(Slot));
    hm_slots_reloc(new_hm, hm.capacity, hm.htable);
    return new_hm;
}

int _hm_keycmp(Hmap hm, void* k1, void* k2) {
    return hm.key_size == HM_STRING ? strcmp(k1, k2) : memcmp(k1, k2, hm.key_size);
}


void* hm_insert(Hmap* hm, void* key, void* value) {
    size_t hash = hm->key_size == HM_STRING ? hm_hash_string(key) : hm_hash_bytes(key, hm->key_size);
    return hm_insert_hashed(hm, hash, key, value);
}

bool _hm_get_index(Hmap hm, void* key, size_t* index) {
    size_t hash = hm.key_size == HM_STRING ? hm_hash_string(key) : hm_hash_bytes(key, hm.key_size);
    *index = hash & (hm.capacity - 1);
    Slot slot = hm.htable[*index];
    while (slot.key != NULL) {
        if (!slot.deleted && hash == slot.hash) {
            if (_hm_keycmp(hm, slot.key, key) == 0) {
                return true;
            }
        }
        // TODO: change to quadratic probing
        *index = (*index + 1) % hm.capacity;
        slot = hm.htable[*index];
    }
    return false;
}

void* hm_search(Hmap hm, void* key) {
    size_t index;
    if (_hm_get_index(hm, key, &index)) {
        return hm.htable[index].value;
    }
    return NULL;
}

bool hm_remove(Hmap* hm, void* key) {
    size_t index;
    if (_hm_get_index(*hm, key, &index)) {
        hm->htable[index].deleted = true;
        ++hm->deleted_cnt;
        --hm->used_cnt;
        return true;
    }
    return false;
}

size_t hm_length(Hmap hm) {
    return hm.used_cnt;
}

Hset* hs_create(size_t key_size) {
    return hm_create(key_size, 1);
}

void hs_free(Hset* hs) {
    hm_free(hs);
}

void* hs_insert(Hset* hs, void* key) {
    return hm_insert(hs, key, &(char) { 0 });
}

bool hs_remove(Hset* hs, void* key) {
    return hm_remove(hs, key);
}

bool hs_contains(Hset hs, void* key) {
    return _hm_get_index(hs, key, &(size_t){0});
}

size_t hs_length(Hset hs) {
    return hs.used_cnt;
}

#endif // HMAP_IMPLEMENTATION