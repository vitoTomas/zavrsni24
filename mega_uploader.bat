@echo off
REM Usage: upload_avrdude.bat HEX_FILE_PATH

REM Check if the HEX file path was provided as an argument
if "%1"=="" (
    echo Please provide the HEX file path as an argument.
    exit /b
)

REM Set your avrdude command with the appropriate programmer, COM port, and baud rate
set PROGRAMMER=arduino
set PORT=COM3
set BAUDRATE=115200
set BAUDRATE_EEPROM=9600

REM Run avrdude with the provided HEX file path
avrdude -c %PROGRAMMER% -p m328p -P %PORT% -b %BAUDRATE% -U flash:w:%1:i


REM Check the avrdude exit code to determine success or failure
if %errorlevel% neq 0 (
    echo avrdude encountered an error.
) else (
    echo avrdude completed successfully.
)
