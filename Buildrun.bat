@echo off

rem Define a variable to store the list of kernel source files
set KERNEL_SOURCE_FILES=.\kernel\system.c .\kernel\syscall.c

rem Define a variable to store the list of various source files
set SOURCE_FILES=.\source\mshell.c .\source\stdioe.c .\source\test.c

rem Define a variable to store the path to the linker file
set LINKER_FILE=.\linkers\kernel_linker.ld

rem Define a variable to store the path to your custom headers directory
set HEADER_DIR=.\include

rem Compile source files using the specified linker file
avr-gcc -mmcu=atmega328p -o output.elf -Wl,--defsym=__heap_end=0x80ffff -T %LINKER_FILE% -I %HEADER_DIR% %KERNEL_SOURCE_FILES% %SOURCE_FILES%
if errorlevel 1 (
    echo Compilation failed.
    exit /b 1
)

rem Convert ELF to Intel Hex format
avr-objcopy -O ihex -R .eeprom output.elf output.hex
if errorlevel 1 (
    echo Conversion to hex failed.
    exit /b 1
)

rem Generate a memory map file
avr-objdump -h -S output.elf > memory_map.txt
if errorlevel 1 (
    echo Memory map generation failed.
    exit /b 1
)

rem Execute mega_uploader.bat with the generated hex file
.\mega_uploader.bat output.hex
if errorlevel 1 (
    echo Uploading failed.
    exit /b 1
)

rem Start putty
putty.exe -serial COM3 -sercfg 9600
