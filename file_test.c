#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int main() {
    int fd;
    char *filename = "sample.txt";
    char *text = "Hello Linux File!\n";
    char buffer[100];

    // 1) 파일 생성 및 쓰기
    fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        perror("open error");
        return 1;
    }

    write(fd, text, strlen(text));
    close(fd);
    printf("1) 파일 생성 및 쓰기 완료\n");

    // 2) 파일 읽기
    fd = open(filename, O_RDONLY);
    if (fd < 0) {
        perror("open error");
        return 1;
    }

    int bytes = read(fd, buffer, sizeof(buffer) - 1);
    buffer[bytes] = '\0';
    close(fd);

    printf("2) 파일 읽기 결과: %s\n", buffer);

    // 3) 파일 삭제
    if (remove(filename) == 0) {
        printf("3) 파일 삭제 완료\n");
    } else {
        perror("remove error");
    }

    return 0;
}
