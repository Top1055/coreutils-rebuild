#include <stdio.h>

int main(int argc, char **argv) {
    if (argc == 1) {
        fprintf(stderr, "Usage: %s <args...>\n", argv[0]);
        return 1;
    }

    for (int i = 1; i < argc; i++) {
        printf("%i. %s\n", i, argv[i]);
    }
    return 0;
}
