#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <string.h>
#include <errno.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "사용법: %s <원본 파일> <복사할 파일>\n", argv[0]);
        return 1;
    }

    const char *src_path = argv[1];
    const char *dst_path = argv[2];
    const char *shm_name = "/filecopy_shm";

    int src_fd = -1, dst_fd = -1, shm_fd = -1;
    struct stat st;
    void *src_map = NULL;
    void *shm_map = NULL;
    void *dst_map = NULL;

    // 1) 원본 파일 열기
    src_fd = open(src_path, O_RDONLY);
    if (src_fd < 0) {
        perror("원본 파일 open 실패");
        goto error;
    }

    if (fstat(src_fd, &st) < 0) {
        perror("fstat 실패");
        goto error;
    }

    if (st.st_size == 0) {
        fprintf(stderr, "원본 파일 크기가 0입니다.\n");
        goto error;
    }

    size_t filesize = st.st_size;
    printf("원본 파일 크기: %zu 바이트\n", filesize);

    // 2) mmap으로 원본 파일 매핑 (읽기 전용)
    src_map = mmap(NULL, filesize, PROT_READ, MAP_SHARED, src_fd, 0);
    if (src_map == MAP_FAILED) {
        perror("mmap(src) 실패");
        goto error;
    }

    // 3) 공유 메모리 객체 생성 및 크기 설정
    shm_fd = shm_open(shm_name, O_CREAT | O_RDWR, 0600);
    if (shm_fd < 0) {
        perror("shm_open 실패");
        goto error;
    }

    if (ftruncate(shm_fd, filesize) < 0) {
        perror("shm ftruncate 실패");
        goto error;
    }

    // 4) 공유 메모리 매핑 (읽기/쓰기)
    shm_map = mmap(NULL, filesize, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shm_map == MAP_FAILED) {
        perror("mmap(shm) 실패");
        goto error;
    }

    // 5) 원본 파일 내용 → 공유 메모리로 복사
    memcpy(shm_map, src_map, filesize);
    printf("원본 파일 내용을 공유 메모리에 복사 완료\n");

    // 6) 대상 파일 생성 및 크기 설정
    dst_fd = open(dst_path, O_CREAT | O_RDWR | O_TRUNC, 0644);
    if (dst_fd < 0) {
        perror("대상 파일 open 실패");
        goto error;
    }

    if (ftruncate(dst_fd, filesize) < 0) {
        perror("대상 파일 ftruncate 실패");
        goto error;
    }

    // 7) 대상 파일 mmap (읽기/쓰기)
    dst_map = mmap(NULL, filesize, PROT_READ | PROT_WRITE, MAP_SHARED, dst_fd, 0);
    if (dst_map == MAP_FAILED) {
        perror("mmap(dst) 실패");
        goto error;
    }

    // 8) 공유 메모리 내용 → 대상 파일로 복사
    memcpy(dst_map, shm_map, filesize);
    printf("공유 메모리 내용을 대상 파일로 복사 완료\n");

    // 9) 동기화
    if (msync(dst_map, filesize, MS_SYNC) < 0) {
        perror("msync 실패");
        goto error;
    }

    printf("파일 복사가 완료되었습니다: %s -> %s\n", src_path, dst_path);

    // 정리
    munmap(src_map, filesize);
    munmap(shm_map, filesize);
    munmap(dst_map, filesize);

    close(src_fd);
    close(dst_fd);
    close(shm_fd);

    shm_unlink(shm_name);

    return 0;

error:
    if (src_map && src_map != MAP_FAILED) munmap(src_map, st.st_size);
    if (shm_map && shm_map != MAP_FAILED) munmap(shm_map, st.st_size);
    if (dst_map && dst_map != MAP_FAILED) munmap(dst_map, st.st_size);

    if (src_fd >= 0) close(src_fd);
    if (dst_fd >= 0) close(dst_fd);
    if (shm_fd >= 0) {
        close(shm_fd);
        shm_unlink(shm_name);
    }

    return 1;
}
