#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define BUFFER_SIZE 4096
#define SPACE_MAX 7

struct fileData {
    size_t lines;
    size_t words;
    size_t bytes;
};

int read_file(struct fileData *const data, int fd) {
    unsigned char filestr[BUFFER_SIZE];
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

int get_space(char *argv[], int argc, int start) {
    struct stat buffer;
    int status;
    int fd;

    int space = 0;
    off_t total = 0;

    for (int i = start; i < argc; i++) {
        fd = open(argv[i], O_RDONLY);
        if (fd < 0) {
            continue;
        }
        status = fstat(fd, &buffer);
        close(fd);
        if (status < 0) {
            continue;
        }
        total += buffer.st_size;
    }

    while (total) {
        total /= 10;
        space += 1;
    }

    if (space > SPACE_MAX)
        return SPACE_MAX;
    return space;
}

void display_count(struct fileData const data, const char *file_name, int space,
                   bool words, bool lines, bool characters) {
    bool printed = false;
    if (lines) {
        printf("%*zu", space, data.lines);
        printed = true;
    }
    if (words) {
        if (!printed)
            printf("%*zu", space, data.words);
        else
            printf(" %*zu", space, data.words);
        printed = true;
    }
    if (characters) {
        if (!printed)
            printf("%*zu", space, data.bytes);
        else
            printf(" %*zu", space, data.bytes);
        printed = true;
    }
    printf(" %s\n", file_name);
}

int main(int argc, char *argv[]) {

    bool words = false;
    bool lines = false;
    bool characters = false;
    while (1) {
        int option = getopt(argc, argv, ":lwc");
        if (option == -1) {
            if (words == false && lines == false && characters == false) {
                words = true;
                lines = true;
                characters = true;
            }
            break;
        }

        char option_char = option;
        switch (option_char) {
        case '?':
            fprintf(stderr, "%s: error: unexpected argument found\n", argv[0]);
            return 1;
            break;

        case 'w':
            words = true;
            break;

        case 'l':
            lines = true;
            break;

        case 'c':
            characters = true;
            break;
        }
    }

    if (argc == optind) {
        struct fileData counts = {0, 0, 0};
        int res = read_file(&counts, STDIN_FILENO);
        if (res < 0) {
            fprintf(stderr, "%s: %s\n", argv[0], strerror(errno));
            return 1;
        }
        display_count(counts, "", SPACE_MAX, words, lines, characters);
        return 0;
    }

    int status = 0;
    int space = get_space(argv, argc, optind);

    struct fileData total = {0, 0, 0};
    for (int i = optind; i < argc; i++) {
        struct fileData counts = {0, 0, 0};
        int res = wc_path(&counts, argv[i]);
        if (res < 0) {
            fprintf(stderr, "%s: %s: %s\n", argv[0], argv[i], strerror(errno));
            status = 1;
        } else {
            display_count(counts, argv[i], space, words, lines, characters);
            total.bytes += counts.bytes;
            total.lines += counts.lines;
            total.words += counts.words;
        }
    }

    if (argc - optind > 1) {
        display_count(total, "total", space, words, lines, characters);
    }
    return status;
}
