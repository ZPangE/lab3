#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("사용법: %s <디렉토리 경로>\n", argv[0]);
        return 1;
    }

    const char *path = argv[1];
    DIR *dir = opendir(path);
    if (!dir) {
        perror("opendir 실패");
        return 1;
    }

    struct dirent *entry;
    char filepath[1024];

    printf("=== mmap을 이용한 파일 내용 출력 ===\n");
    printf("디렉토리: %s\n\n", path);

    while ((entry = readdir(dir)) != NULL) {
        // . .. 제외
        if (strcmp(entry->d_name, ".") == 0 ||
            strcmp(entry->d_name, "..") == 0)
            continue;

        snprintf(filepath, sizeof(filepath), "%s/%s", path, entry->d_name);

        struct stat st;
        if (stat(filepath, &st) < 0) {
            perror("stat 실패");
            continue;
        }

        // 디렉토리면 패스
        if (S_ISDIR(st.st_mode))
            continue;

        printf("파일: %s (크기: %ld bytes)\n", filepath, st.st_size);

        int fd = open(filepath, O_RDONLY);
        if (fd < 0) {
            perror("open 실패");
            continue;
        }

        if (st.st_size == 0) {
            printf("[빈 파일]\n\n");
            close(fd);
            continue;
        }

        // mmap 매핑
        void *map = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
        if (map == MAP_FAILED) {
            perror("mmap 실패");
            close(fd);
            continue;
        }

        // 파일 내용 출력
        write(STDOUT_FILENO, map, st.st_size);
        printf("\n\n");

        munmap(map, st.st_size);
        close(fd);
    }

    closedir(dir);
    return 0;
}
