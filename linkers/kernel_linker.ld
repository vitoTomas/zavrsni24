TARGET(binary)
INPUT (".\linkers\files")
OUTPUT_FORMAT("elf32-avr", "elf32-avr", "elf32-avr")
OUTPUT_ARCH(avr:5)

MEMORY
{
    FLASH (rx) : ORIGIN = 0x000000, LENGTH = 32K
    RAM (rw!x) : ORIGIN = 0x800100, LENGTH = 2K
}

SECTIONS
{
    .text :
    {
        *(.data)
        *(.text)
    } > FLASH

    .core_syscalls :
    {
        *(.core_syscalls)
    } > FLASH

    .test 0x2800:
    {
        *(.test)
    } > FLASH

    .mshell :
    {
        *(.mshell)
    } > FLASH

    /*
    .files 0x07800:
    {
        ".\linkers\files"
    } > FLASH
    */

    .data 0x801100:
    {
        __data_load_start = LOADADDR(.data);
        __data_start = .;
        *(.data)
        __data_end = .;
    } > RAM

    .bss :
    {
        __bss_start = .;
        *(.bss)
        __bss_end = .;
    } > RAM

    .heap :
    {
        __heap_start = .;
    } > RAM

    /DISCARD/ :
    {
        *(.comment)
        *(.gnu*)
    }
}