#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

int main() {
    int p2c[2]; // parent → child
    int c2p[2]; // child → parent

    pipe(p2c);
    pipe(c2p);

    pid_t pid = fork();

    if (pid == 0) {
        // 자식 프로세스
        close(p2c[1]); // 부모 → 자식: 쓰기 닫음
        close(c2p[0]); // 자식 → 부모: 읽기 닫음

        char buffer[100];

        // 부모 메시지 수신
        read(p2c[0], buffer, sizeof(buffer));
        printf("자식: 부모로부터 받은 메시지 → %s\n", buffer);

        // 부모에게 응답 전송
        char reply[] = "응답 잘 받았어!";
        write(c2p[1], reply, strlen(reply) + 1);

        close(p2c[0]);
        close(c2p[1]);
    }
    else {
        // 부모 프로세스
        close(p2c[0]); // 부모 → 자식: 읽기 닫음
        close(c2p[1]); // 자식 → 부모: 쓰기 닫음

        // 자식에게 메시지 보내기
        char msg[] = "안녕, 자식 프로세스야!";
        write(p2c[1], msg, strlen(msg) + 1);

        // 자식의 응답 읽기
        char buffer[100];
        read(c2p[0], buffer, sizeof(buffer));
        printf("부모: 자식으로부터 받은 메시지 → %s\n", buffer);

        close(p2c[1]);
        close(c2p[0]);

        wait(NULL); // 자식 종료 대기
    }

    return 0;
}
