#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

// 시그널 핸들러
void handler(int sig) {
    printf("자식 프로세스: 시그널 %d 받음\n", sig);
}

int main() {
    pid_t pid;

    printf("=== 프로세스 & 시그널 실습 ===\n");

    pid = fork();

    if (pid < 0) {
        perror("fork 실패");
        return 1;
    }

    // 자식 프로세스
    if (pid == 0) {
        signal(SIGUSR1, handler);  // 시그널 핸들러 등록
        printf("자식 프로세스 생성됨. PID = %d\n", getpid());
        printf("부모가 시그널 보내기 기다리는 중...\n");

        // 무한 대기
        while (1) {
            sleep(1);
        }
    }

    // 부모 프로세스
    sleep(1); // 자식 준비할 시간

    printf("부모 프로세스: 자식 PID = %d\n", pid);
    printf("부모 -> 자식 SIGUSR1 보내는 중...\n");

    kill(pid, SIGUSR1);   // 자식에게 시그널 보내기

    sleep(1);
    printf("부모: 자식 종료시키기\n");

    kill(pid, SIGKILL);   // 자식 강제 종료
    wait(NULL);           // 자식 종료 대기

    printf("부모: 자식 종료 완료\n");

    return 0;
}
