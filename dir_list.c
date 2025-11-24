#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>

void list_directory(const char *path) {
    DIR *dir;
    struct dirent *entry;
    char fullpath[1024];

    printf("\n[%s]\n", path);

    dir = opendir(path);
    if (dir == NULL) {
        perror("opendir error");
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
        // . 과 .. 무시
        if (strcmp(entry->d_name, ".") == 0 ||
            strcmp(entry->d_name, "..") == 0)
            continue;

        printf("%s\n", entry->d_name);

        // 경로 결합
        snprintf(fullpath, sizeof(fullpath), "%s/%s", path, entry->d_name);

        // 디렉토리인지 확인 후 재귀 호출
        struct stat st;
        if (stat(fullpath, &st) == 0 && S_ISDIR(st.st_mode)) {
            list_directory(fullpath);  // 재귀
        }
    }

    closedir(dir);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("사용법: %s <디렉토리 경로>\n", argv[0]);
        return 1;
    }

    list_directory(argv[1]);

    return 0;
}

