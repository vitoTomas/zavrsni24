/*
 ---------------------------------------------------------------
 Kernel accesible system calls built in the Kernel block. User
 accessible from syscall wrapper function. To be expanded...
 ---------------------------------------------------------------
*/

#include <system.h>
#include <stdlib.h>
#include <string.h>

/*
    USART initialization function. Called by the Kernel during
    boot. Function is system wide effective. Available as a
    system call for possible further expansions.
*/
void __usart_init() {
    uint32_t ubrr = ((uint32_t)F_CPU / (16UL * (uint32_t)USART_BAUD)) - 1;

    UBRR0H = (uint8_t)(ubrr >> 8);
    UBRR0L = (uint8_t)(ubrr);

    UCSR0B = (1 << TXEN0) | (1 << RXEN0);

    return;
}

/*
    USART I/O function for sending characters to the serial
    port. It is not compatible with the standard stream
    function definition because of its limited functionality.
*/
int __usart_send_char(uint8_t c) {
    while (!(UCSR0A & (1<<UDRE0)));
    UDR0 = c;

    return 0;
}

/*
    USART I/O function for receiving characters from the serial
    port. It is not compatible with the standard stream
    function definition because of its limited functionality.
*/
int __usart_receive_char(FILE * stream) {
    while (!(UCSR0A & (1 << RXC0)))
        ;
    return UDR0;
}

/*
    **DEPRICATED**

    File handling function for creating a file i.e. reserving memory.
    File create EEPROM/External currently supports only Program memory
    files with file descriptors stored in the built in EEPROM. Possible
    future expansion for external memory and writeable files.

    IN:
        @ [name]    -   name of the file (maximum of 11 characters)
        @ [type]    -   type of the file (check out FILE_P definition)
        @ [p_id]    -   ID of the parent file (directory)
        @ [size]    -   size defined by the number of pages the file takes
    RETURN:
        File ID     -   if 0 no file created, otherwise it is the ID of the
                        new file
*/
uint8_t __fcreate_P(PGM_P name, uint8_t type, uint8_t p_id, uint8_t size) {
    FILE_P file;
    uint8_t id_counter = 1;
    uint8_t * address = 0;
    uint8_t * write_address = 0;
    uint8_t f_id = 0xFF;

    if(type == F_DIR && size != 0) {
        printf_P("ERROR: Cannot create directory of size greater than 0!");
        return 0;
    }

    if(size > 100) {
        printf_P("ERROR: Cannot create space of more than a 100 pages!");
        return 0;
    }

    /* Find free memory for file descriptor */
    for(address = 0; (uintptr_t) address < __PEPROM_SIZE; address += __SIZE_FILE_P) {
        f_id = eeprom_read_byte(address + __OFFSET_FILE_ID);
        if(f_id == 0) break;
    }

    if(f_id != 0) return 0;
    write_address = address;

    /* Find first free file id */
    address = 0;

    do{
        f_id = eeprom_read_byte(address + __OFFSET_FILE_ID);
        address += __SIZE_FILE_P;
        if(f_id == id_counter) {
            id_counter++;
            address = 0;
        }
        if((uintptr_t) address > 1023) break;
    }while(id_counter <= 100);

    file.page_loc = ((uintptr_t) write_address) / F_PAGE;
    file.type = type;
    file.file_id = id_counter;
    file.parent_id = p_id;
    file.size = size;
    strcpy_P((char * )&file.name, name);

    eeprom_write_block(&file, address, __SIZE_FILE_P);

    return id_counter;
}

/*
    File handling function for retriving the status of a file.
    File status PROGRAM currently supports only Program memory
    files with file descriptors stored in the built in PROG MEM. Possible
    future expansion for external memory and writeable files.

    IN:
        @ [name]    -   name of the file (maximum of 11 characters)
        @ [type]    -   type of the file (check out FILE_P definition)
        @ [p_id]    -   ID of the parent file (directory)
        @ [size]    -   size defined by the number of pages the file takes

    RETURN:
        Result      -   success status of execution
                        0 -> success
                        -1 -> file not found
                        -2 -> passed NULL as FILE_P pointer 
*/
int __fstat_P(uint8_t f_id, FILE_P * file) {
    uint16_t address =  __RESERVED_START_ADDR;
    uint16_t offset = 0;
    uint8_t curr_id = 0xFF;
    uint8_t *ptr = (uint8_t *) file;
    uint8_t i;

    if(file == NULL) return -2;
    
    /* Find the file with the identifier */
    for(offset; offset < __PEPROM_SIZE; offset += __SIZE_FILE_P) {
        curr_id = pgm_read_byte(address + offset + __OFFSET_FILE_ID);
        if(curr_id == f_id) break;
    }

    if(curr_id != f_id) return -1;

    for(i = 0; i < sizeof(FILE_P); i++) {
        ptr[i] = pgm_read_byte(address + offset + i);
    }

    return 0;
}

/*
    File handling function made to search for a file based on its
    path and returns its file ID.

    IN:
        @ [path]        -   Pointer to the path string

    RETURN:
        Result          -   success status of execution or file ID
                            >= 0 -> success
                            -1 -> file not found
*/
int __ffind_P(char * path) {
    uint16_t address =  __RESERVED_START_ADDR;
    uint16_t offset = 0;
    uint8_t parent = 0, id;
    uint8_t item_found = 0;
    char string[100], *token = NULL, name[12], delim[] = "/";
    int i = 0, found = -1;

    strcpy(string, path);

    /* Detect root directory */
    if (!strcmp(path, delim)) return 0;

    token = strtok(string, delim);
    while(token != NULL) {
        offset = 0;
        item_found = 0;

        if (strlen(token) == 0) {
            token = strtok(NULL, delim);
            continue;
        }

        for(offset; offset < __PEPROM_SIZE; offset += __SIZE_FILE_P) {
            id = pgm_read_byte(address + offset + __OFFSET_PARENT_ID);
            
            for(i = 0; i < 11; i++) {
                name[i] = pgm_read_byte(address + offset + __OFFSET__NAME + i);
            }

            name[11] = '\0';

            if(!strcmp(token, name)) {
                if(id == parent) {
                    found = pgm_read_byte(address + offset + __OFFSET_FILE_ID);
                    parent = found;
                    item_found = 1;
                    break;
                }
            }
        }

        if (item_found == 0) {
            found = -1;
            break;
        }

        token = strtok(NULL, delim);
    }

    return found;
}

/*
    File handling function made to list a directory specified
    by its path.

    IN:
        @ [path]        -   Pointer to the path string

    RETURN:
        Result          -   success status of execution or file ID
                             0 -> success
                            -1 -> file not found
                            -2 -> file is not a directory (not implemented)
*/
int __flist_P(char *path) {
    FILE_P file;
    uint16_t address =  __RESERVED_START_ADDR, offset = 0;
    uint8_t size, type;
    char name[12];
    int i, item_no = 2, cid, id, parent = __ffind_P(path);

    /* File (directory) does not exist */
    if (parent < 0) return -1;

    /* File is not a directory (FIXME) */
    /* ... */

    printf_P("type\tsize\tname\n\r");
    printf_P("----------------------------------------\n\r");
    printf_P(" -\t%4d\t.\n\r", 0);
    printf_P(" -\t%4d\t..\n\r", 0);

    for(offset; offset < __PEPROM_SIZE; offset += __SIZE_FILE_P) {
        id = pgm_read_byte(address + offset + __OFFSET_PARENT_ID);
        cid = pgm_read_byte(address + offset + __OFFSET_FILE_ID);
    
        if (id == parent && cid != parent) {
            item_no++;
            size = pgm_read_byte(address + offset + __OFFSET_SIZE);
            type = pgm_read_byte(address + offset + __OFFSET_TYPE);

            for(i = 0; i < 11; i++) {
                name[i] = pgm_read_byte(address + offset + __OFFSET__NAME + i);
            }

            name[11] = '\0';

            switch (type)
            {
            case F_PXE:
                printf_P(" e\t");
                break;
            case F_RED:
                printf_P(" r\t");
                break;
            case F_DIR:
                printf_P(" d\t");
                break;
            default:
                printf_P(" -\t");
                break;
            }

            printf_P("%4d\t%s\n\r", (int) size * __PAGE_SIZE, name);
        }

    }

    printf_P("----------------------------------------\n\r");
    printf_P("Items: %2d\n\r", item_no);

    return 0;
}

/*
    The call function is a call to the kernel subroutine to indicate
    that a program wishes to start an another program.

    IN:
        @ [file_id]     -   ID of the file to execute
    RETURN:
                        -   success status of execution
                            -1 -> file not found
*/
int __call(uint8_t file_id) {
    typedef int (* Program)(void);
    Program run = NULL;
    FILE_P file;
    uint16_t address;
    int ret = 0;
    
    /* Retrive file information */
    ret = __fstat_P(file_id, &file);
    if(ret) {
        return -1;
    }

    /* Calculate the program start address */
    address = __PAGE_START_ADDR + __PAGE_SIZE * file.page_loc; 

    /* Run the program at address   */
    /* Call jump locations are byte */
    /* aligned hence the need to    */
    /* divide the address by 2      */
    run = (Program)(address / 2);
    return run();
}