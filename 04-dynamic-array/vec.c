#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PAGE_SIZE 4096

struct vec {
    int *data;
    size_t len;
    size_t cap;
};

int vec_init(struct vec *v) {
    int *ptr = malloc(sizeof(int) * PAGE_SIZE);
    if (!ptr)
        return -1;
    // Assign after in case of error
    v->cap = PAGE_SIZE;
    v->len = 0;
    v->data = ptr;
    return 0;
}

int vec_push(struct vec *v, int value) {
    if (v->len == v->cap) {
        // Extend vector
        int *ptr = realloc(v->data, sizeof(int) * v->cap * 2);
        if (!ptr)
            return -1;

        v->cap *= 2;
        v->data = ptr;
    }
    v->data[v->len] = value;
    v->len++;
    return 0;
}

int vec_pop(struct vec *v, int *out) {
    if (v->len == 0)
        return -1;
    *out = v->data[--v->len];
    return 0;
}

int vec_get(const struct vec *v, size_t index, int *out) {
    if (index >= v->len)
        return -1;
    *out = v->data[index];
    return 0;
}

int vec_set(struct vec *v, size_t index, int value) {
    if (index >= v->len)
        return -1;
    v->data[index] = value;
    return 0;
}

void vec_free(struct vec *v) {
    if (v->data)
        free(v->data);
    v->data = NULL;
    v->cap = 0;
    v->len = 0;
};

int main() {
    struct vec v;
    vec_init(&v);

    for (int i = 0; i < 100000; i++)
        vec_push(&v, i);

    printf("Length of vec: %ld\n", v.len);
    int out = -1;
    if (vec_get(&v, 0, &out) != 0) // expect 0
        fprintf(stderr, "Error!\n");
    else
        printf("index: %d, value: %d\n", 100000, out);
    if (vec_get(&v, 99999, &out) != 0) // expect 99999
        fprintf(stderr, "Error!\n");
    else
        printf("index: %d, value: %d\n", 100000, out);
    if (vec_get(&v, 100000, &out) != 0) // expect failure
        fprintf(stderr, "Error!\n");
    else
        printf("index: %d, value: %d\n", 100000, out);

    while (vec_pop(&v, &out) == 0) {
    }
    vec_pop(&v, &out); // expect failure on empty
    printf("Length of vec: %ld\n", v.len);

    vec_free(&v);
    return 0;
}
