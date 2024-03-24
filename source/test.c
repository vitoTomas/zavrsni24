#include <stdio.h>

int test(void) __attribute__((section(".test")));

int test(void) {
    printf_P("Hello from the test program!\n\r");
    return 0;
}