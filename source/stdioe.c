#include <stdio.h>

#include <syscall.h>
#include <stdioe.h>

#define BUFFER 100

int escanf_P(const char * __fmt, void * data) {
    char buffer[BUFFER];
    unsigned offset = 0;

    do {
        buffer[offset] = (char) __usart_receive_char(stdin);
        if(buffer[offset] == 13) break;
        putchar(buffer[offset]);
        offset++;
    } while(offset < BUFFER);

    return sscanf_P(buffer, __fmt, data);
}