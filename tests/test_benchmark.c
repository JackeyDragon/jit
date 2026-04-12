#define _POSIX_C_SOURCE 200809L

#include "test.h"
#include "../src/tokenizer.h"
#include "../src/ast.h"
#include <time.h>
#include <string.h>
#include <sys/time.h>
#include <stdlib.h>

extern struct token *current;
extern ast *parse_statement();
extern int exec(ast *statement);

double get_time_ms() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000.0 + tv.tv_usec / 1000.0;
}

void run_benchmark(int iterations) {
    double start = get_time_ms();
    for (int i = 0; i < iterations; i++) {
        struct token *tokens = tokinize("var x = 1 + 2 * 3;");
        current = tokens;
        ast *tree = parse_statement();
        exec(tree);
    }
    double end = get_time_ms();
    printf("Benchmark: %d iterations in %.2fms (%.2f us/iter)\n",
           iterations, end - start, (end - start) * 1000 / iterations);
}

int main(int argc, char *argv[]) {
    int iterations = 10000;
    if (argc > 1) {
        iterations = atoi(argv[1]);
    }
    printf("=== Benchmark (%d iterations) ===\n", iterations);
    run_benchmark(iterations);
    return 0;
}