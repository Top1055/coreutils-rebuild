#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

size_t my_strlen(const char *s) {
    size_t len = 0;
    while (s[len] != '\0') {
        len++;
    }
    return len;
}

char *my_strcpy(char *dest, const char *src) {
    size_t len = my_strlen(src);
    for (size_t i = 0; i < len; i++) {
        dest[i] = src[i];
    }
    dest[len] = '\0';
    return dest;
}

char *my_strdup(const char *s) {
    size_t len = my_strlen(s);
    char *p = malloc(len + 1);
    if (!p)
        return NULL;
    return my_strcpy(p, s);
}

int my_strcmp(const char *a, const char *b) {
    size_t i = 0;
    while (1) {
        if (a[i] != b[i] || (a[i] == '\0' || b[i] == '\0'))
            return (unsigned char)a[i] - (unsigned char)b[i];
        i++;
    }
    return 0;
}

char *my_strcat(char *a, const char *b) {
    size_t len_a = my_strlen(a);
    my_strcpy(a + len_a, b);
    return a;
}

void split_free(char **tokens) {
    if (!tokens)
        return;
    for (size_t i = 0; tokens[i] != NULL; i++) {
        free(tokens[i]);
    }
    free(tokens);
}

char **split(const char *str, char delim) {
    size_t str_len = my_strlen(str);
    size_t splits = 2; // Keep one for NULL and one for the first entry
    for (size_t i = 0; i < str_len; i++) {
        if (str[i] == delim)
            splits++;
    }

    char **data = malloc(sizeof(char *) * splits);
    if (!data)
        return NULL;

    size_t split = 0;
    size_t curr_char = 0;
    size_t last_delim = 0;
    while (1) {
        if (str[curr_char] == '\0' || str[curr_char] == delim) {
            data[split] = malloc(curr_char - last_delim + 1);
            if (!data[split]) {
                data[split] = NULL;
                goto cleanup;
            }
            for (size_t i = last_delim; i < curr_char; i++) {
                data[split][i - last_delim] = str[i];
            }
            data[split][curr_char - last_delim] = '\0';
            split++;
            last_delim = curr_char + 1;
            if (str[curr_char] == '\0')
                break;
        }
        curr_char++;
    }
    data[split] = NULL;
    return data;

cleanup:
    split_free(data);
    return NULL;
}

static void check_split(const char *in, char d, const char **expect, size_t n) {
    char **t = split(in, d);
    assert(t != NULL);
    size_t i = 0;
    while (t[i]) {
        assert(my_strcmp(t[i], expect[i]) == 0);
        i++;
    }
    assert(i == n);
    split_free(t);
}

int main(int argc, char *argv[]) {

    if (argc > 1) {
        size_t size = my_strlen(argv[1]) + 1;
        char *str = malloc(size);
        if (!str)
            return 1;
        my_strcpy(str, argv[1]);
        for (int i = 2; i < argc; i++) {
            // Plus one for the extra spaces
            size += my_strlen(argv[i]) + 1;
            str = realloc(str, size);
            if (!str)
                return 1;
            str = my_strcat(str, " ");
            str = my_strcat(str, argv[i]);
        }
        printf("All args: %s\n", str);
        free(str);
        return 0;
    }

    check_split("a,b,c", ',', (const char *[]){"a", "b", "c"}, 3);
    check_split("a,b,,c", ',', (const char *[]){"a", "b", "", "c"}, 4);
    check_split("abc", ',', (const char *[]){"abc"}, 1);
    check_split("", ',', (const char *[]){""}, 1);

    assert(my_strlen("hello") == 5);
    assert(my_strcmp("abc", "abc") == 0);
    assert(my_strcmp("abc", "abd") < 0);

    const char *a = "Hello";
    const char *b = " World!";
    char *c = malloc(my_strlen(a) + my_strlen(b) + 1);
    if (!c)
        return 1;
    my_strcpy(c, a);
    c = my_strcat(c, b);
    char *d = malloc(my_strlen(c) + 1);
    if (!d)
        return 1;
    assert(my_strcpy(d, c) == d);
    assert(my_strcmp(c, d) == 0);
    free(c);
    free(d);
    return 0;
}
