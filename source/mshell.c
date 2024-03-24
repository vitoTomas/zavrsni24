#include <stdio.h>
#include <string.h>

#include <syscall.h>
#include <stdioe.h>

int mshell(void) __attribute__((section(".mshell")));

int mshell(void) {
    char input[100];
    char path[200];

    uint8_t file_id;

    input[0] = '\0';       // Input placeholder
    path[0] = '/';
    path[1] = '\0';

    file_id = 1;

    /* Attempt to run a test program */
    __call(file_id);

    while(1) {
        printf_P("%s$> ", path);
        escanf_P("%s", input);

        strcat(path, input);
        printf_P("\n\r");
    }

    return 0;
}