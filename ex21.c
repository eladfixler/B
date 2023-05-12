//elad fixler 215200684
#include <stdio.h>
#include <fcntl.h>
#include <ctype.h>
#include <unistd.h>
#include <stdbool.h>

#define BUFFER_SIZE 1
char read_one_char(int fd) {
    char buffer[BUFFER_SIZE];
    int bytes_read = read(fd, buffer, BUFFER_SIZE);
    if (bytes_read == 0) {
        return NULL;
    } else {
        return buffer[0];
    }

}
int main(int argc, char* argv[]) {
            printf("input is: \n");
        for (int i = 0; i < argc; i++) {
            printf("%s\n", argv[i]);
        }
    if (argc < 3) {
        printf("input is: \n");
        for (int i = 0; i < argc; i++) {
            printf("%s\n", argv[i]);
        }
        printf("invalid input\n");
        return -1;
    }
    int fd1, fd2;

    fd1 = open(argv[1], O_RDONLY);
    if (fd1 == -1) {
        printf("prob1");
        perror("Error in open");
        return -1;
    }

    fd2 = open(argv[2], O_RDONLY);
    if (fd2 == -1) {
        printf("prob2");
        perror("Error in open");
        close(fd1);
        return -1;
    }
    char file1, file2;
    file1 = read_one_char(fd1);
    file2 = read_one_char(fd2);
    bool same = true, similar = false;
    while (file1 != NULL || file2 != NULL) {
        printf("compare between %c and %c \n",file1, file2);
        if (file1 == file2) {
            file1 = read_one_char(fd1);
            file2 = read_one_char(fd2);
            continue;
        } else if(isspace(file1)) {
            same = false;
            similar = true;
            file1 = read_one_char(fd1);
        } else if(isspace(file2)) {
            same = false;
            similar = true;
            file2 = read_one_char(fd2);
        } else {
            if(tolower(file1) == tolower(file2)) {
                same = false;
                file1 = read_one_char(fd1);
                file2 = read_one_char(fd2);
                continue;
            }
            close(fd1);
            close(fd2);
            printf("they diffrent and i know it in the while");
            return 2;
        }
    }
    if (file1 == file2) {
        if (same) {
            close(fd1);
            close(fd2);
            printf("they same because they end in the same time and same was up");
            return 1;
        } else {
            close(fd1);
            close(fd2);
            printf("they similar because they end at the same time and same was down");
            return 3;
        }
    }
    if (file1 == NULL) {
        while(file2 != NULL) {
            if (!isspace(file2)) {
                close(fd1);
                close(fd2);
                printf("they diffrent because one end while the other still had not space in the end");
                return 2;
            } else {
                same = false;
                file2 = read_one_char(fd2);
            }
        }
        close(fd1);
        close(fd2);
        printf("the similar because un the end of the one there are only spaces");
        return 3;
    }

    if (file2 == NULL) {
        while(file1 != NULL) {
            if (!isspace(file1)) {
                close(fd1);
                close(fd2);
                printf("they diffrent because one end while the other still had not space in the end");
                return 2;
            } else {
                same = false;
                file1 = read_one_char(fd1);
            }
        }
        close(fd1);
        close(fd2);
        printf("the similar because un the end of the one there are only spaces");
        return 3;
    }
    close(fd1);
    close(fd2);
    //cant get here
    return 3;
}