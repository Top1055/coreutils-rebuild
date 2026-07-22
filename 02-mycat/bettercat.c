#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#define BUFFER_SIZE 4096

int main(int argc, char *argv[]) {

    int file = open(argv[1], O_RDONLY);
    char data[BUFFER_SIZE];
    int data_len = 1;
    do {
        data_len = read(file, data, BUFFER_SIZE);
        write(STDOUT_FILENO, data, data_len);
    } while (data_len > 0);
    close(file);

    printf("file: %d\n", file);
    return 0;
}
