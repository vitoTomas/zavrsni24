/*
 ---------------------------------------------------------------
 Kernel accesible system calls built in the Kernel block. User
 accessible from syscall wrapper function. To be expanded...
 ---------------------------------------------------------------
*/

#include <system.h>

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
    File handling function for creating a file i.e. reserving memory.
    File create EEPROM/External currently supports only Program memory
    files with file descriptors stored in the built in EEPROM. Possible
    future expansion for external memory and writeable files.

    IN:
        @ [name]    -   name of the file (maximum of 11 characters)
        @ [type]    -   type of the file (check out FILE_E definition)
        @ [p_id]    -   ID of the parent file (directory)
        @ [size]    -   size defined by the number of pages the file takes
    RETURN:
        File ID     -   if 0 no file created, otherwise it is the ID of the
                        new file
*/
uint8_t __fcreate_E(PGM_P name, uint8_t type, uint8_t p_id, uint8_t size) {
    FILE_E file;
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
    for(address = 0; (uintptr_t) address < __EEPROM_SIZE; address += __SIZE_FILE_E) {
        f_id = eeprom_read_byte(address + __OFFSET_FILE_ID);
        if(f_id == 0) break;
    }

    if(f_id != 0) return 0;
    write_address = address;

    /* Find first free file id */
    address = 0;

    do{
        f_id = eeprom_read_byte(address + __OFFSET_FILE_ID);
        address += __SIZE_FILE_E;
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

    eeprom_write_block(&file, address, __SIZE_FILE_E);

    return id_counter;
}

/*
    File handling function for retriving the status of a file.
    File status EEPROM/External currently supports only Program memory
    files with file descriptors stored in the built in EEPROM. Possible
    future expansion for external memory and writeable files.

    IN:
        @ [name]    -   name of the file (maximum of 11 characters)
        @ [type]    -   type of the file (check out FILE_E definition)
        @ [p_id]    -   ID of the parent file (directory)
        @ [size]    -   size defined by the number of pages the file takes
    RETURN:
        Result      -   success status of execution
                        0 -> success
                        -1 -> file not found
                        -2 -> passed NULL as FILE_E pointer 
*/
int __fstat_E(uint8_t f_id, FILE_E * file) {
    uint8_t * address = (uint8_t *) 0;
    uint8_t offset = 0;
    uint8_t curr_id = 0xFF;

    if(file == NULL) return -2;
    
    /* Find the file with the identifier */
    for(offset; offset < __EEPROM_SIZE; offset += __SIZE_FILE_E) {
        curr_id = eeprom_read_byte(address + offset + __OFFSET_FILE_ID);
        if(curr_id == f_id) break;
    }

    if(curr_id != f_id) return -1;

    eeprom_read_block(file, address + offset, __SIZE_FILE_E);

    return 0;
}

/*
    The call function is a call to the kernel subroutine to indicate
    that a program wishes to start an another program.

    IN:
        @ [file_id]     -   ID of the file to execute
    RETURN:
                        -   success status of execution
*/

int __call(uint8_t file_id) {
    typedef int (* Program)(void);
    Program run = NULL;
    FILE_E file;
    uint16_t address;
    int ret = 0;
    
    /* Retrive file information */
    ret = __fstat_E(file_id, &file);
    if(ret) {
        fprintf(stderr, "ERROR: Unable to locate the file.");
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