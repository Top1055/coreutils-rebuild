#include <ctype.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

#define BUFFER_SIZE 4096

struct fileData {
    size_t lines;
    size_t words;
    size_t bytes;
};

int read_file(struct fileData *const data, int fd) {
    char filestr[BUFFER_SIZE];
    int status = 0;

    bool last_space = true;

    while (1) {
        ssize_t n;
        n = read(fd, filestr, BUFFER_SIZE);
        if (n == 0)
            break; // EOF
        else if (n < 0) {
            status = -1;
            break;
        }

        data->bytes += n;

        for (int i = 0; i < n; i++) {
            if (isspace(filestr[i]) != 0) {
                last_space = true;
                if (filestr[i] == '\n')
                    data->lines += 1;
                continue;
            }

            if (last_space) {
                data->words += 1;
                last_space = false;
            }
        }
    }
    return status;
}

int wc_path(struct fileData *const data, const char *path) {
    int status = 0;
    int fd = open(path, O_RDONLY);
    if (fd < 0) {
        return -1;
    }

    status = read_file(data, fd);

    int res = close(fd);
    if (res < 0) {
        return -1;
    }
    return status;
}

int main(int argc, char *argv[]) {

    int status = 0;

    if (argc == 1) {
        return status;
    } else if (argc == 2) {
        struct fileData counts = {0, 0, 0};
        int res = wc_path(&counts, argv[1]);
        if (res < 0)
            status = 1;
        else
            printf("%ld %ld %ld %s\n", counts.lines, counts.words, counts.bytes,
                   argv[1]);
        return status;
    }

    struct fileData total = {0, 0, 0};
    for (int i = 1; i < argc; i++) {
        struct fileData counts = {0, 0, 0};
        int res = wc_path(&counts, argv[i]);
        if (res < 0)
            status = 1;
        else {
            printf("%ld %ld %ld %s\n", counts.lines, counts.words, counts.bytes,
                   argv[i]);
            total.bytes += counts.bytes;
            total.lines += counts.lines;
            total.words += counts.words;
        }
    }

    printf("%ld %ld %ld total\n", total.lines, total.words, total.bytes);
    return status;
}
