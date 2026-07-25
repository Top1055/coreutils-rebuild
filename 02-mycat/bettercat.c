#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define BUFFER_SIZE 4096

int handle_write(int fd, const void *buf, size_t count) {
    const char *p = buf;
    size_t bytes_written = 0;
    while (bytes_written < count) {
        ssize_t bytes = write(fd, p + bytes_written, count - bytes_written);
        if (bytes < 0)
            return -1; // Error
        bytes_written += bytes;
    }
    return 0;
}

int cat_fd(int fd) {
    char data[BUFFER_SIZE];
    int status = 0;
    while (1) {
        ssize_t n;

        n = read(fd, data, BUFFER_SIZE);
        if (n == 0)
            break; // EOF
        if (n < 0) {
            status = -1; // Error
            break;
        }

        if (handle_write(STDOUT_FILENO, data, n) < 0) {
            status = -1; // Error
            break;
        }
    }
    return status;
}

int cat_path(const char *path) {

    int file = open(path, O_RDONLY);
    if (file < 0)
        return -1; // Error

    int status = cat_fd(file);

    int res = close(file);
    if (res < 0)
        return -1; // Error

    return status;
}

int main(int argc, char *argv[]) {

    if (argc == 1) {
        int res = cat_fd(STDIN_FILENO);
        if (res < 0) {
            fprintf(stderr, "%s: %s\n", argv[0], strerror(errno));
            return 1;
        }
        return 0;
    }

    int status = 0;

    for (int i = 1; i < argc; i++) {
        int res = cat_path(argv[i]);
        if (res < 0) {
            // Error
            status = 1;
            fprintf(stderr, "%s: %s: %s\n", argv[0], argv[i], strerror(errno));
        }
    }

    return status;
}
