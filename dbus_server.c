#include <dbus/dbus.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// 간단한 에러 출력 함수
void print_dbus_error(const char *msg, DBusError *err) {
    fprintf(stderr, "%s: %s\n", msg, err->message);
    dbus_error_free(err);
}

int main() {
    DBusError err;
    DBusConnection *conn;
    int ret;

    dbus_error_init(&err);

    // 1) 세션 버스에 연결
    conn = dbus_bus_get(DBUS_BUS_SESSION, &err);
    if (dbus_error_is_set(&err)) {
        print_dbus_error("DBus 연결 오류", &err);
        return 1;
    }
    if (conn == NULL) {
        fprintf(stderr, "DBus 세션 버스 연결 실패\n");
        return 1;
    }

    // 2) 서비스 이름 등록
    ret = dbus_bus_request_name(conn,
                                "deu.lab2.NotifyServer",
                                DBUS_NAME_FLAG_REPLACE_EXISTING,
                                &err);
    if (dbus_error_is_set(&err)) {
        print_dbus_error("서비스 이름 등록 오류", &err);
        return 1;
    }
    if (ret != DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER) {
        fprintf(stderr, "서비스 이름을 얻지 못했습니다.\n");
        return 1;
    }

    // 3) 수신할 시그널 필터 등록
    dbus_bus_add_match(conn,
                       "type='signal',interface='deu.lab2.Notifier'",
                       &err);
    dbus_connection_flush(conn);
    if (dbus_error_is_set(&err)) {
        print_dbus_error("매치 규칙 등록 오류", &err);
        return 1;
    }

    printf("DBus 알림 서버 실행 중...\n");
    printf("인터페이스: deu.lab2.Notifier, 시그널: Notify\n");
    printf("클라이언트 알림을 대기합니다.\n");

    while (1) {
        // 메시지 수신 대기 (blocking 아님)
        dbus_connection_read_write(conn, 100); // 100ms
        DBusMessage *msg = dbus_connection_pop_message(conn);

        if (msg == NULL) {
            // 잠깐 쉼
            usleep(100000);
            continue;
        }

        // 우리가 원하는 시그널인지 확인
        if (dbus_message_is_signal(msg,
                                   "deu.lab2.Notifier",
                                   "Notify")) {
            const char *text = NULL;
            dbus_error_init(&err);

            if (dbus_message_get_args(msg, &err,
                                      DBUS_TYPE_STRING, &text,
                                      DBUS_TYPE_INVALID)) {
                printf("알림 수신: %s\n", text);
            } else {
                print_dbus_error("시그널 인자 파싱 실패", &err);
            }
        }

        dbus_message_unref(msg);
    }

    return 0;
}
