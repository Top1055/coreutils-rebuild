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
};

int my_strcmp(const char *a, const char *b) {
    size_t i = 0;
    while (1) {
        if (a[i] != b[i] || (a[i] == '\0' || b[i] == '\0'))
            return a[i] - b[i];
        i++;
    }
    return 0;
}
char *my_strcat(char *a, const char *b) {
    size_t len_a = my_strlen(a);
    my_strcpy(a + len_a, b);
    return a;
};

char **split(const char *str, char delim);
void split_free(char **tokens);

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

    const char *a = "Hello";
    const char *b = " World!";
    char *c = malloc(my_strlen(a) + my_strlen(b) + 1);
    if (!c)
        return 1;
    my_strcpy(c, a);
    c = my_strcat(c, b);
    printf("a: %s\n", a);
    printf("b: %s\n", b);
    printf("c: %s\n", c);
    printf("diff: %d\n", my_strcmp(a, b));
    printf("diff: %d\n", my_strcmp(a, c));
    printf("diff: %d\n", my_strcmp(a, a));
    free(c);
    return 0;
}
