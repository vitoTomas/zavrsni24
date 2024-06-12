/*
 ---------------------------------------------------------------
            Syscall and file handle header file.
 ---------------------------------------------------------------
*/
#include <stdint.h>
#include <stdio.h>

#ifndef _SYSCALL_H_

/* Syscalls */
#define _SYS_INIT_USART     1
#define _SYS_PRINT_CHAR     2
#define _SYS_GET_CHAR       3
#define _SYS_CREATE_FILE    4 /* DEPRICATED */
#define _SYS_GET_FILE_STAT  5
#define _SYS_FIND_FILE      6
#define _SYS_LIST_DIR       7
#define _SYS_CALL_PROGRAM   8

/* Memory segments */
#define __KERNEL_START_ADDR     0x0000
#define __PAGE_START_ADDR       0x2800
#define __RESERVED_START_ADDR   0x7800
#define __PAGE_SIZE             128

#define __PEPROM_SIZE           0x0400

/* EEPROM/External memory file handler for executable code organisation.
   Possible future expansion to 32 B and support for external and non-executable
   memory.

    + [page_loc]    -   page number of file
    + [type]        -   file or directory
    + [prog_star]   -   program entry point
    + [file_id]     -   unique file identifier
    + [parent_id]   -   unique file parent identifier
    + [size]        -   page defined file size
    + [name]        -   11 character file name
    + [reserved]    -   for future expansion
*/
struct __pgm_file {
    uint8_t page_loc;   /* Start page location */
    uint8_t type;       /* Type of file */
#define F_PXE 0         /* Program file */
#define F_RED 1         /* Read only file */
#define F_DIR 2         /* Directory */       
    uint8_t file_id;    /* ID of a file */
    uint8_t parent_id;  /* ID of file parent */
    uint8_t size;       /* Number of pages file takes */
#define F_PAGE 256
    uint8_t name[11];   /* Name of the file */
};

typedef struct __pgm_file FILE_P;

#define __SIZE_FILE_P       16

#define __OFFSET_PAGE_LOC   0
#define __OFFSET_TYPE       1
#define __OFFSET_FILE_ID    2
#define __OFFSET_PARENT_ID  3
#define __OFFSET_SIZE       4
#define __OFFSET__NAME      5

int __usart_send_char(uint8_t c);
int __usart_receive_char(FILE * stream);
void __usart_init();
/* **DEPRICATED** */
uint8_t __fcreate_P(const char * name, uint8_t type, uint8_t p_id, uint8_t size);
int __fstat_P(uint8_t f_id, FILE_P * file);
int __ffind_P(char * path);
int __flist_P(char *path);
int __call(uint8_t file_id);

#endif