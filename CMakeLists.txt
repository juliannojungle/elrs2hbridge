cmake_minimum_required(VERSION 3.13)
project(elrs2hbridge C)

set(MCU attiny84)
set(F_CPU 16000000UL)

set(SRC
    src/main.c
    src/usi_uart.c
)

add_executable(elrs2hbridge.elf ${SRC})

set_target_properties(elrs2hbridge.elf PROPERTIES
    COMPILE_FLAGS "-mmcu=${MCU} -DF_CPU=${F_CPU} -Os -Wall"
    LINK_FLAGS "-mmcu=${MCU}"
)

add_custom_command(OUTPUT elrs2hbridge.hex
    COMMAND avr-objcopy -O ihex -R .eeprom elrs2hbridge.elf elrs2hbridge.hex
    DEPENDS elrs2hbridge.elf
)

add_custom_target(hex ALL DEPENDS elrs2hbridge.hex)

add_custom_target(flash
    COMMAND avrdude -c usbasp -p ${MCU} -U flash:w:elrs2hbridge.hex:i
    DEPENDS elrs2hbridge.hex
)

add_custom_target(fuses
    COMMAND avrdude -c usbasp -p ${MCU}
            -U lfuse:w:0xFF:m
            -U hfuse:w:0xDF:m
            -U efuse:w:0xFF:m
)
