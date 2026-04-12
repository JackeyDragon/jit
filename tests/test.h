#ifndef TEST_H
#define TEST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TEST_ASSERT(condition, msg) do { \
    if (!(condition)) { \
        printf("FAIL: %s\n", msg); \
        exit(1); \
    } \
} while(0)

#define TEST_ASSERT_EQ(actual, expected, msg) do { \
    if ((actual) != (expected)) { \
        printf("FAIL: %s - expected %d, got %d\n", msg, (expected), (actual)); \
        exit(1); \
    } \
} while(0)

#define TEST_ASSERT_STR_EQ(actual, expected, msg) do { \
    if (strcmp((actual), (expected)) != 0) { \
        printf("FAIL: %s - expected '%s', got '%s'\n", msg, (expected), (actual)); \
        exit(1); \
    } \
} while(0)

#define TEST_RUN(name) do { \
    printf("Running %s... ", #name); \
    fflush(stdout); \
    name(); \
    printf("PASS\n"); \
} while(0)

#endif