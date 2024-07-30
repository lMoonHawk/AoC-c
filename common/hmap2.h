#ifndef HMAP_H
#define HMAP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include <sys/types.h>
/* Main modifications:
    - key values are stored in a dense array / the hashmap is a sparse array of indexes
    - allow overriding the default key comparison function (permits structs containing pointers as keys)
    - allow overriding the default key hashing function
    - use siphash
*/

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
    ssize_t* htable;
    Slot* slots;
} Hmap;

typedef Hmap Hset;

typedef struct {
    Hmap* hm;
    size_t index;
} Hmap_iter;

typedef Hmap_iter Hset_iter;

Hmap* hm_create(size_t key_size, size_t value_size);
void hm_slots_free(size_t n, Slot slots[n]);
void hm_free(Hmap* hm);
Hmap_iter hm_iter(Hmap* hm);
bool hm_iter_next(Hmap_iter* iter, void* key, void* value);
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
bool hm_contains(Hset hs, void* key);
size_t hm_length(Hmap hm);
// Hashset functions (TODO: might #define instead)
Hset* hs_create(size_t key_size);
void hs_free(Hset* hs);
Hset_iter hs_iter(Hset* hs);
bool hs_iter_next(Hset_iter* iter, void** key);
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
    hm->htable = malloc(hm->capacity * sizeof(ssize_t));
    memset(hm->htable, -1, hm->capacity * sizeof(ssize_t));
    hm->slots = calloc(hm->capacity, sizeof(Slot));
    assert(hm->htable != NULL);
    assert(hm->slots != NULL);
    hm->key_size = key_size;
    hm->value_size = value_size;
    return hm;
}

Hmap_iter hm_iter(Hmap* hm) {
    return (Hmap_iter) { hm, 0 };
}

Hset_iter hs_iter(Hset* hs) {
    return (Hset_iter) { hs, 0 };
}

bool hm_iter_next(Hmap_iter* iter, void* key, void* value) {
    if (iter->index >= iter->hm->used_cnt + iter->hm->deleted_cnt)
        return false;
    Slot* slot = &iter->hm->slots[iter->index];
    if (slot->deleted) {
        ++iter->index;
        return hm_iter_next(iter, key, value);
    }
    if (key != NULL) {
        if (iter->hm->key_size == HM_STRING)
            strcpy(key, slot->key);
        else
            memcpy(key, slot->key, iter->hm->key_size);
    }
    if (value != NULL) {
        if (iter->hm->value_size == HM_STRING)
            strcpy(value, slot->value);
        else
            memcpy(value, slot->value, iter->hm->value_size);
    }
    ++iter->index;
    return true;
}

bool hs_iter_next(Hset_iter* iter, void** key) {
    return hm_iter_next(iter, key, NULL);
}

void hm_slots_free(size_t n, Slot slots[n]) { 
    for (size_t i = 0; i < n; ++i) {
        free(slots[i].key);
        free(slots[i].value);
    }
}

void hm_free(Hmap* hm) {
    hm_slots_free(hm->used_cnt + hm->deleted_cnt, hm->slots);
    free(hm->htable);
    free(hm->slots);
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
    size_t prev_len = hm->used_cnt + hm->deleted_cnt;
    Slot* prev_slots = hm->slots;
    hm->capacity = new_capacity;
    free(hm->htable);
    hm->htable = malloc(new_capacity * sizeof(ssize_t));
    memset(hm->htable, -1, new_capacity * sizeof(ssize_t));
    hm->slots = calloc(new_capacity, sizeof(Slot));
    assert(hm->htable != NULL);
    assert(hm->slots != NULL);
    hm->used_cnt = 0;
    hm->deleted_cnt = 0;
    hm_slots_reloc(hm, prev_len, prev_slots);
    hm_slots_free(prev_len, prev_slots);
    free(prev_slots);
}


void* hm_insert_hashed(Hmap* hm, size_t hash, void* key, void* value) {
    if (hm->used_cnt + hm->deleted_cnt > hm->capacity * HM_LOADFACTOR)
        _hm_resize(hm, hm->capacity * 2);
    if (hm->deleted_cnt > 0 && hm->used_cnt + hm->deleted_cnt < hm->capacity * (1 - HM_LOADFACTOR))
        _hm_resize(hm, hm->capacity / 2);

    size_t ht_index = hash & (hm->capacity - 1);
    ssize_t arr_index = hm->htable[ht_index];
    Slot* slot = NULL;
    bool replace = false;
    bool deleted = false;
    while (arr_index != -1 && !deleted && !replace) {
        slot = &hm->slots[arr_index];
        deleted = slot->deleted;
        if (!deleted && hash == slot->hash && _hm_keycmp(*hm, key, slot->key) == 0)
            replace = true;
        if (!deleted && !replace) {
            ht_index = (ht_index + 1) % hm->capacity;
            arr_index = hm->htable[ht_index];
        }
    }
    if (arr_index == -1) {
        arr_index = hm->used_cnt + hm->deleted_cnt;
        hm->htable[ht_index] = arr_index;
        slot = &hm->slots[arr_index];
    } else if (deleted) {
        --hm->deleted_cnt;
        slot->deleted = false;
    }

    // Value
    if (hm->value_size == HM_STRING) {
        free(slot->value);
        slot->value = strdup(value);
    } else {
        if (!replace)
            slot->value = malloc(hm->value_size);
        memcpy(slot->value, value, hm->value_size);
    }

    // Key
    if (!replace) {
        slot->hash = hash;
        ++hm->used_cnt;
        if (hm->key_size == HM_STRING) {
            slot->key = strdup(key);
        } else {
            slot->key = malloc(hm->key_size);
            memcpy(slot->key, key, hm->key_size);
        }
    }
    return slot->value;
}
// #endif // HMAP_IMPLEMENTATION

void hm_slots_reloc(Hmap* hm, size_t nsource, Slot source[nsource]) {
    for (size_t i = 0; i < nsource; ++i) {
        if (!source[i].deleted)
            hm_insert_hashed(hm, source[i].hash, source[i].key, source[i].value);
    }
}

Hmap* hm_copy(Hmap hm) {
    Hmap* new_hm = calloc(1, sizeof(Hmap));
    new_hm->key_size = hm.key_size;
    new_hm->value_size = hm.value_size;
    new_hm->capacity = hm.capacity;
    new_hm->htable = malloc(new_hm->capacity * sizeof(ssize_t));
    memset(new_hm->htable, -1, new_hm->capacity * sizeof(ssize_t));
    new_hm->slots = calloc(new_hm->capacity, sizeof(Slot));
    hm_slots_reloc(new_hm, hm.used_cnt + hm.deleted_cnt, hm.slots);
    return new_hm;
}

int _hm_keycmp(Hmap hm, void* k1, void* k2) {
    return hm.key_size == HM_STRING ? strcmp(k1, k2) : memcmp(k1, k2, hm.key_size);
}

void* hm_insert(Hmap* hm, void* key, void* value) {
    size_t hash = hm->key_size == HM_STRING ? hm_hash_string(key) : hm_hash_bytes(key, hm->key_size);
    return hm_insert_hashed(hm, hash, key, value);
}

bool _hm_get_index(Hmap hm, void* key, size_t* ht_index) {
    size_t hash = hm.key_size == HM_STRING ? hm_hash_string(key) : hm_hash_bytes(key, hm.key_size);
    *ht_index = hash & (hm.capacity - 1);
    ssize_t arr_index = hm.htable[*ht_index];
    while (arr_index != -1) {
        Slot* slot = &hm.slots[arr_index];
        if (!slot->deleted && hash == slot->hash) {
            if (_hm_keycmp(hm, slot->key, key) == 0) {
                return true;
            }
        }
        // TODO: change to quadratic probing
        *ht_index = (*ht_index + 1) % hm.capacity;
        arr_index = hm.htable[*ht_index];
    }
    return false;
}

void* hm_search(Hmap hm, void* key) {
    size_t ht_index;
    if (_hm_get_index(hm, key, &ht_index)) {
        return hm.slots[hm.htable[ht_index]].value;
    }
    return NULL;
}

bool hm_remove(Hmap* hm, void* key) {
    size_t ht_index;
    if (_hm_get_index(*hm, key, &ht_index)) {
        ssize_t arr_index = hm->htable[ht_index];
        hm->slots[arr_index].deleted = true;
        free(hm->slots[arr_index].key);
        free(hm->slots[arr_index].value);
        ++hm->deleted_cnt;
        --hm->used_cnt;
        return true;
    }
    return false;
}

bool hm_contains(Hset hs, void* key) {
    return _hm_get_index(hs, key, &(size_t){0});
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