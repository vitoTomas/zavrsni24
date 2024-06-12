/*
---------------------------------------------------------------
            MCOS kernel base and system calls
---------------------------------------------------------------
*/

#include <system.h>
#include <programs.h>

/* Core kernel logic */
int main(void) {
    FILE usart_OUT = FDEV_SETUP_STREAM((int (*)(char, FILE *))__usart_send_char, NULL, _FDEV_SETUP_WRITE);
    FILE usart_IN = FDEV_SETUP_STREAM(NULL, __usart_receive_char, _FDEV_SETUP_READ);

    char user[11];

    stdout = &usart_OUT;
    stderr = &usart_OUT;
    stdin = &usart_IN;

    strcpy(user, "SYS");

    /* Startup */
    __usart_init();
    printf_P("MCOS ver. 1.0.0\n\r");
    printf_P("INFO: System ready!\n\r");

    /* Run MicroShell */
    mshell(user);

    return 0;
}

/* Syscall wrapper */
int syscall(uint8_t syscall, const uint8_t *args) {
    switch (syscall) {

    case _SYS_INIT_USART:
        __usart_init();
        return 0;

    case _SYS_PRINT_CHAR:
        __usart_send_char(*args);
        return 0;

    case _SYS_GET_CHAR:
        return __usart_receive_char(NULL);

    case _SYS_CALL_PROGRAM:
        return __call(*args);

    case _SYS_GET_FILE_STAT:
        return __fstat_P(*args, (FILE_P *) (args + 1));

    case _SYS_FIND_FILE:
        return __ffind_P((char *) (args));

    default:
        return -1;
    }
}

