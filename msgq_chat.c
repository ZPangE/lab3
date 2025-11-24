#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

struct msgbuf {
    long mtype;
    char mtext[100];
};

int main() {
    key_t key = ftok(".", 65);  
    int msgid = msgget(key, 0666 | IPC_CREAT);

    pid_t pid = fork();

    if (pid == 0) {
        // 자식 프로세스
        struct msgbuf msg;

        // 부모 메시지 받기 (type = 1)
        msgrcv(msgid, &msg, sizeof(msg.mtext), 1, 0);
        printf("자식: 부모 메시지 → %s\n", msg.mtext);

        // 부모에게 응답 전송 (type = 2)
        msg.mtype = 2;
        strcpy(msg.mtext, "메시지 잘 받았어!");
        msgsnd(msgid, &msg, sizeof(msg.mtext), 0);

        exit(0);
    }
    else {
        // 부모 프로세스
        struct msgbuf msg;

        // 부모 → 자식 (type = 1)
        msg.mtype = 1;
        strcpy(msg.mtext, "안녕 자식아(메시지큐)!");
        msgsnd(msgid, &msg, sizeof(msg.mtext), 0);

        // 자식 → 부모 (type = 2)
        msgrcv(msgid, &msg, sizeof(msg.mtext), 2, 0);
        printf("부모: 자식 메시지 → %s\n", msg.mtext);

        wait(NULL);

        // 메시지 큐 삭제
        msgctl(msgid, IPC_RMID, NULL);
    }

    return 0;
}
