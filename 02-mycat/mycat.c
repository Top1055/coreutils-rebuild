#include <errno.h>
#include <stdio.h>
#include <string.h>

#define BUFFER_SIZE 4096

int print_file(FILE *fptr) {
    char data[BUFFER_SIZE];
    size_t data_length = 1;

    while (data_length > 0) {
        data_length = fread(data, sizeof(char), BUFFER_SIZE, fptr);
        fwrite(data, sizeof(char), data_length, stdout);
    }

    int status = ferror(fptr);

    return status;
}

int main(int argc, char *argv[]) {

    if (argc == 1) {
        int print_res = print_file(stdin);
        if (print_res != 0) {
            return 1;
        }
        return 0;
    }

    int status = 0;

    for (int i = 1; i < argc; i++) {
        FILE *fptr = fopen(argv[i], "r");
        int print_res = 0;
        int close_res = 0;
        if (fptr) {
            print_res = print_file(fptr);
            close_res = fclose(fptr);
        }

        if (print_res != 0 || close_res != 0 || !fptr) {
            fprintf(stderr, "%s: %s: %s\n", argv[0], argv[i], strerror(errno));
            status = 1;
        }
    }

    return status;
}
