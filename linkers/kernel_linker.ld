OUTPUT_FORMAT("elf32-avr", "elf32-avr", "elf32-avr")
OUTPUT_ARCH(avr:5)

MEMORY
{
    FLASH (rx) : ORIGIN = 0x000000, LENGTH = 0x008000
    RAM (rwx) : ORIGIN = 0x800100, LENGTH = 0x0007FF
}

SECTIONS
{
    .text :
    {
        *(.text)
    } > FLASH

    .core_syscalls :
    {
        *(.core_syscalls)
    } > FLASH

    .test 0x1400 :
    {
        *(.test)
    } > FLASH

    .mshell 0x1800 :
    {
        *(.mshell)
    } > FLASH

    .data :
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

    /DISCARD/ :
    {
        *(.comment)
        *(.gnu*)
    }
}
