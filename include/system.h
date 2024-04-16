/*
 ---------------------------------------------------------------
        Kernel handlers header file. For Kernel use only!
 ---------------------------------------------------------------
*/

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <string.h>
#include "syscall.h"

#ifndef _SYSTEM_H_

#define F_CPU 16000000UL
#define USART_BAUD 9600

/* Memory organised section */
int main(void) __attribute__((section(".kernel_main")));
int syscall(uint8_t syscall, const uint8_t *args) __attribute__((section(".core_syscalls")));

#endif
