#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CAPACITY 256

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
    for (size_t i = 0; i < m->capacity; i++) {
        struct entry *e = m->entries[i];

        while (e) {
            if (e->next) {
                struct entry *ptr = e->next;
                free(m->entries[i]->key);
                free(m->entries[i]);
                m->entries[i] = ptr;
                e = m->entries[i];
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

int map_move(struct map *m, uint32_t a, uint32_t b, const char *key) {
    if (!m)
        return -1;

    struct entry *e = m->entries[a];
    // if on top
    if (e && strcmp(e->key, key) == 0) {
        struct entry *end = e->next;
        m->entries[a] = end;

        e->next = m->entries[b];
        m->entries[b] = e;
        return 0;
    } else {
        if (e) {
            while (e->next) {
                if (strcmp(e->next->key, key) == 0) {
                    struct entry *end = e->next->next;
                    struct entry *move = e->next;
                    e->next = end;
                    move->next = m->entries[b];
                    m->entries[b] = move;
                    return 0;
                } else {
                    e = e->next;
                }
            }
        }
    }
    return -1;
}

int map_grow(struct map *m) {
    if (!m)
        return -1;
    if (m->capacity == 0)
        return -1;
    void *ptr = reallocarray(m->entries, m->capacity * 2, sizeof(*m->entries));
    if (!ptr)
        return -1;
    m->capacity *= 2;
    m->entries = ptr;

    // Zero new entries
    for (size_t i = m->capacity / 2; i < m->capacity; i++)
        m->entries[i] = NULL;

    for (size_t i = 0; i < m->capacity / 2; i++) {
        struct entry *e = m->entries[i];
        while (e) {

            uint32_t hash = fnv1a(e->key);
            uint32_t index = hash % m->capacity;
            if (index != i) {
                map_move(m, i, index, e->key);
                e = m->entries[i];
                continue;
            } else {
                e = e->next;
            }
        }
    }
    return 0;
}

// insert or update
int map_set(struct map *m, const char *key, int value) {

    if (!m)
        return -1;
    if (m->len * 4 >= m->capacity * 3) {
        // if above 75%
        int status = map_grow(m);
        if (status != 0) {
            if (m->len == m->capacity)
                fprintf(stderr,
                        "WARN: hash map length exceeding capacity and cannot "
                        "grow %zu/%zu\n",
                        m->len, m->capacity);
            else
                fprintf(stderr, "WARN: hash map cannot grow %zu/%zu\n", m->len,
                        m->capacity);
        }
    }

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

    char *k = strdup(key);
    if (!k)
        return -1;
    struct entry *ptr = malloc(sizeof(struct entry));
    if (!ptr) {
        free(k);
        return -1;
    }
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

int map_del(struct map *m, const char *key) {
    uint32_t hash = fnv1a(key);
    uint32_t index = hash % m->capacity;

    struct entry *e = m->entries[index];

    if (e) {
        if (strcmp(e->key, key) == 0) {
            m->entries[index] = e->next;
            free(e->key);
            free(e);
            m->len--;
            return 0;
        } else {
            while (e->next) {
                if (strcmp(e->next->key, key) == 0) {
                    struct entry *ptr = e->next->next;
                    free(e->next->key);
                    free(e->next);
                    e->next = ptr;
                    m->len--;
                    return 0;
                } else {
                    e = e->next;
                }
            }
        }
    }
    return -1;
}

size_t map_len(const struct map *m) { return m->len; }

int main() {
    struct map m;
    map_init(&m);

    char key[32];

    size_t failures = 0;

    // Set 10k values
    for (int i = 0; i < 10000; i++) {
        snprintf(key, sizeof(key), "test%d", i);

        if (map_set(&m, key, i) != 0)
            failures++;
    }

    printf("10k entries created\n");

    // Access them all
    for (int i = 0; i < 10000; i++) {
        snprintf(key, sizeof(key), "test%d", i);

        int out = -1;
        map_get(&m, key, &out);
        if (out != i)
            failures++;
    }

    // update half
    for (int i = 0; i < 10000 / 2; i++) {
        snprintf(key, sizeof(key), "test%d", i);

        if (map_set(&m, key, i * 10) != 0)
            failures++;

        // check they're updated
        int out = -1;
        map_get(&m, key, &out);
        if (out != i * 10)
            failures++;
    }
    printf("updated 5k\n");

    // delete a third
    for (int i = 0; i < 10000 / 3; i++) {
        snprintf(key, sizeof(key), "test%d", i);

        if (map_del(&m, key) != 0)
            failures++;

        // check they're deleted
        int out = -1;
        map_get(&m, key, &out);
        if (out == i * 10)
            failures++;
    }
    printf("deleted a third, %zu remain\n", map_len(&m));

    map_free(&m);

    printf("failure count: %zu\n", failures);
    return 0;
}
