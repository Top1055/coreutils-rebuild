#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CAPACITY 4096

struct entry {
    char *key;
    int value;
    struct entry *next;
};

struct map {
    size_t capacity;
    size_t len;
    struct entry **entries;
};

uint32_t fnv1a(const char *s) {
    uint32_t h = 2166136261u;
    while (*s) {
        h ^= (unsigned char)*s++;
        h *= 16777619u;
    }
    return h;
}

void map_free(struct map *m) {
    if (!m)
        return;
    if (m->capacity == 0)
        return;
    for (uint32_t i = 0; i < m->capacity; i++) {
        struct entry *e = m->entries[i];

        while (e) {
            if (e->next) {
                struct entry *ptr = e->next;
                free(m->entries[i]->key);
                free(m->entries[i]);
                m->entries[i] = ptr;
            } else {
                free(m->entries[i]->key);
                free(m->entries[i]);
                break;
            }
        }
    }
    m->capacity = 0;
    m->len = 0;
    free(m->entries);
}

int map_init(struct map *m) {
    void *ptr = calloc(CAPACITY, sizeof(*m->entries));
    if (!ptr)
        return -1;
    m->capacity = CAPACITY;
    m->len = 0;
    m->entries = ptr;
    return 0;
}

// insert or update
int map_set(struct map *m, const char *key, int value) {
    // Hash key and find calc index
    uint32_t hash = fnv1a(key);
    uint32_t index = hash % m->capacity;

    struct entry *e = m->entries[index];

    while (e) {
        if (strcmp(e->key, key) == 0) {
            e->value = value;
            return 0;
        }
        e = e->next;
    }
    struct entry *ptr = malloc(sizeof(struct entry));
    if (!ptr)
        return -1;
    char *k = strdup(key);
    if (!k)
        return -1;
    *ptr = (struct entry){
        .key = k,
        .value = value,
        .next = m->entries[index],
    };
    m->entries[index] = ptr;
    m->len++;
    return 0;
}

int map_get(const struct map *m, const char *key, int *out) {
    uint32_t hash = fnv1a(key);
    uint32_t index = hash % m->capacity;

    struct entry *e = m->entries[index];

    while (e) {
        if (strcmp(e->key, key) == 0) {
            *out = e->value;
            return 0;
        }
        e = e->next;
    }
    return -1;
}

int map_del(struct map *m, const char *key);

size_t map_len(const struct map *m);

int main() {
    struct map m;
    map_init(&m);
    if (map_set(&m, "feli", 21) != 0)
        return 1;
    int age = -1;
    if (map_get(&m, "feli", &age) != 0)
        return 1;
    map_free(&m);

    printf("feli: %d\n", age);
}
