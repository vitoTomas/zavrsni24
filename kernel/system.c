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

    stdout = &usart_OUT;
    stdin = &usart_IN;

    /* Startup */
    __usart_init();
    printf_P("MCOS ver. 1.0.0\n\r");
    printf_P("INFO: System ready!\n\r");

    /* Run MicroShell */
    mshell();

    return 0;
}

/* Syscall wrapper */
void * syscall(uint8_t syscall, const uint8_t *args) {

    void *return_value;
    uint8_t temp;

    switch (syscall) {
    case 1:
        __usart_init();
        return NULL;
    case 2:
        __usart_send_char(*args);
        return NULL;
    case 3:
        temp = (uint8_t) __usart_receive_char(NULL);
        return_value = (void * ) &temp;
        return return_value;
    default:
        return NULL;
    }
}

