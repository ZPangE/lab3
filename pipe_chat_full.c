#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <stdlib.h>

#define MSG_SIZE 100

int main() {
    int p2c[2]; // 부모 → 자식
    int c2p[2]; // 자식 → 부모

    pipe(p2c);
    pipe(c2p);

    pid_t pid = fork();

    if (pid < 0) {
        perror("fork 오류");
        exit(1);
    }

    if (pid == 0) {
        // 자식 프로세스
        close(p2c[1]); // 쓰기 닫음
        close(c2p[0]); // 읽기 닫음

        char recv_msg[MSG_SIZE];
        char send_msg[MSG_SIZE];

        while (1) {
            // 부모 메시지 수신
            read(p2c[0], recv_msg, MSG_SIZE);
            printf("부모 → 자식: %s\n", recv_msg);

            // 종료 조건
            if (strcmp(recv_msg, "quit") == 0) {
                printf("자식 종료.\n");
                break;
            }

            // 자식 메시지 입력
            printf("자식 입력: ");
            fgets(send_msg, MSG_SIZE, stdin);
            send_msg[strcspn(send_msg, "\n")] = 0;

            write(c2p[1], send_msg, MSG_SIZE);

            if (strcmp(send_msg, "quit") == 0) {
                printf("자식 종료.\n");
                break;
            }
        }

        close(p2c[0]);
        close(c2p[1]);
        exit(0);
    }
    else {
        // 부모 프로세스
        close(p2c[0]);
        close(c2p[1]);

        char send_msg[MSG_SIZE];
        char recv_msg[MSG_SIZE];

        while (1) {
            // 부모 메시지 입력
            printf("부모 입력: ");
            fgets(send_msg, MSG_SIZE, stdin);
            send_msg[strcspn(send_msg, "\n")] = 0;

            write(p2c[1], send_msg, MSG_SIZE);

            if (strcmp(send_msg, "quit") == 0) {
                printf("부모 종료.\n");
                break;
            }

            // 자식 메시지 대기
            read(c2p[0], recv_msg, MSG_SIZE);
            printf("자식 → 부모: %s\n", recv_msg);

            if (strcmp(recv_msg, "quit") == 0) {
                printf("부모 종료.\n");
                break;
            }
        }

        close(p2c[1]);
        close(c2p[0]);

        wait(NULL);
    }

    return 0;
}
