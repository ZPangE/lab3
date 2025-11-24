#include <dbus/dbus.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_dbus_error(const char *msg, DBusError *err) {
    fprintf(stderr, "%s: %s\n", msg, err->message);
    dbus_error_free(err);
}

int main(int argc, char *argv[]) {
    DBusError err;
    DBusConnection *conn;
    DBusMessage *msg;
    DBusMessageIter args;
    dbus_uint32_t serial = 0;

    if (argc != 2) {
        fprintf(stderr, "사용법: %s <보낼 알림 메시지>\n", argv[0]);
        return 1;
    }

    const char *text = argv[1];

    dbus_error_init(&err);

    // 1) 세션 버스 연결
    conn = dbus_bus_get(DBUS_BUS_SESSION, &err);
    if (dbus_error_is_set(&err)) {
        print_dbus_error("DBus 연결 오류", &err);
        return 1;
    }
    if (conn == NULL) {
        fprintf(stderr, "DBus 세션 버스 연결 실패\n");
        return 1;
    }

    // 2) 시그널 메시지 생성
    msg = dbus_message_new_signal(
        "/deu/lab2/Notifier",    // object path
        "deu.lab2.Notifier",     // interface
        "Notify"                 // signal name
    );

    if (msg == NULL) {
        fprintf(stderr, "시그널 메시지 생성 실패\n");
        return 1;
    }

    // 3) 문자열 인자 추가
    dbus_message_iter_init_append(msg, &args);
    if (!dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &text)) {
        fprintf(stderr, "메시지 인자 추가 실패\n");
        dbus_message_unref(msg);
        return 1;
    }

    // 4) 버스로 시그널 전송
    if (!dbus_connection_send(conn, msg, &serial)) {
        fprintf(stderr, "시그널 전송 실패\n");
        dbus_message_unref(msg);
        return 1;
    }

    dbus_connection_flush(conn);
    dbus_message_unref(msg);

    printf("알림 메시지 전송 완료: %s\n", text);

    return 0;
}
