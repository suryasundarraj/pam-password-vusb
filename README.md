# pam-password-vusb Data Storage

### Building the Enviroinment for Virtual-USB (V-USB)

With the Few Modifications the Code can be used for different versions of AVR Atmega Chips, This repo is run and tested with the Atmega8 8-Bit Microcontroller building the Low-USB Speed Device Data Storage

#### Install the AVR GCC Toolchain Components using Package Manager 

- gcc-avr   : The actual C/C++ Compiler 
- binutils  : A collection of tools, including the assembler, linker and some other tools to manipulate the generated   
              binary files.
- libc-avr  : A subset of the standard C Library with some additional AVR specific functions. The libc-avr package also                includes the AVR specific header files. 
- uisp      : Requierd for avr 
- gdb       : The debugger. See the Debugging section for more information about this. 
- avrdude   : A Programm to download/upload/manipulate the ROM and EEPROM of an AVR MCU.

### To install avrdude
              sudo apt-get install avrdude

### To Install above toolchain 
              sudo apt-get install gcc-avr binutils-avr gdb-avr avr-libc avrdude



  

