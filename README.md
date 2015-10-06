# pam-password-vusb Data Storage

### Building the Enviroinment for Virtual-USB (V-USB)

With the few modifications this Code can be used for different versions of AVR Atmega Chips, This repo built and tested with the Atmega8 8-Bit Microcontroller building the Low-USB Speed Device Data Storage

#### Install the AVR GCC Toolchain Components using Package Manager 

- gcc-avr   : The actual C/C++ Compiler 
- binutils  : A collection of tools, including the assembler, linker and some other tools to manipulate the generated                binary files.
- libc-avr  : A subset of the standard C Library with some additional AVR specific functions. The libc-avr package also                includes the AVR specific header files. 
- uisp      : Requierd for avr 
- gdb       : The debugger. See the Debugging section for more information about this. 
- avrdude   : A Programm to download/upload/manipulate the ROM and EEPROM of an AVR MCU.

### To install avrdude
              sudo apt-get install avrdude

### To Install above toolchain 
              sudo apt-get install gcc-avr binutils-avr gdb-avr avr-libc avrdude

### Flashing the Fuse Bits for the Atmega8 using avrdude
              sudo avrdude -c usbasp -p m8 -U lfuse:w:0x9f:m -U hfuse:w:0xc9:m

### Burning the Firmware to the AVR Chip using avrdude 

For Building the firmware first clone this git repo to your desktop using ,

              git clone https://github.com/suryasundarraj/pam-password-vusb.git
              cd pam-password-vsb
              cd firmware
              make hex
              sudo avrdude -c usbasp -p m8 -U flash:w:main.hex

### Building the Commandline Tool to Read and Write to the EEPROM
              cd pam-password-vusb
              cd commanline
              make
#### Commands to Read and Write data using Commandline tool

              sudo ./pamtool read 
                    : retrives the data stored in the EEPROM
              sudo ./pamtool write 0x52 0x61 0x64
                    : writes the data to the EEPROM which can be retrived later 
  

