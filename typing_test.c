#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

// 현재 시각을 초 단위(실수)로 반환
double now_sec() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec / 1000000.0;
}

int main() {
    const char *sentences[] = {
        "Hello Linux typing test.",
        "Rust and C are system programming languages.",
        "Process and IPC are important in operating systems."
    };
    const int count = sizeof(sentences) / sizeof(sentences[0]);

    char input[256];
    int total_errors = 0;
    int total_chars = 0;
    double total_time = 0.0;

    printf("=== 타자 연습 프로그램 ===\n");
    printf("아래에 출력되는 문장을 그대로 따라 치고 Enter를 누르세요.\n\n");

    for (int i = 0; i < count; i++) {
        printf("문장 %d:\n%s\n", i + 1, sentences[i]);
        printf("입력: ");

        double start = now_sec();

        if (fgets(input, sizeof(input), stdin) == NULL) {
            printf("입력 오류\n");
            return 1;
        }

        double end = now_sec();
        double elapsed = end - start;
        if (elapsed <= 0.0) elapsed = 0.01;  // 0초 방지

        total_time += elapsed;

        // 줄바꿈 제거
        size_t len_ref = strlen(sentences[i]);
        size_t len_in = strlen(input);
        if (len_in > 0 && input[len_in - 1] == '\n') {
            input[len_in - 1] = '\0';
            len_in--;
        }

        int errors = 0;
        size_t min_len = len_ref < len_in ? len_ref : len_in;

        // 문자 단위 비교
        for (size_t j = 0; j < min_len; j++) {
            if (sentences[i][j] != input[j]) {
                errors++;
            }
        }

        // 길이 차이를 추가 오타로 처리
        if (len_in > len_ref) {
            errors += (int)(len_in - len_ref);
        } else if (len_ref > len_in) {
            errors += (int)(len_ref - len_in);
        }

        total_errors += errors;
        total_chars += (int)len_ref;

        printf("→ 해당 문장 오타 수: %d개, 입력 시간: %.2f초\n\n", errors, elapsed);
    }

    if (total_time <= 0.0) total_time = 0.01;

    // 5타 = 1단어 기준 WPM
    double wpm = (total_chars / 5.0) / (total_time / 60.0);

    printf("=== 결과 ===\n");
    printf("총 기준 문자 수: %d자\n", total_chars);
    printf("총 오타 수: %d개\n", total_errors);
    printf("총 입력 시간: %.2f초\n", total_time);
    printf("평균 분당 타수(WPM): %.2f\n", wpm);

    return 0;
}

