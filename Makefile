# === CONFIGURAÇÕES DO PROJETO ===
PROJECT = elrs2hbridge
MCU = attiny84
# F_CPU = 16000000UL # para cristal de 16 MHz
F_CPU = 20000000UL # para cristal de 20 MHz
PROGRAMMER = usbasp

SRC = src/main.c src/usi_uart.c

# === TOOLS ===
CC = avr-gcc
OBJCOPY = avr-objcopy
CFLAGS = -mmcu=$(MCU) -DF_CPU=$(F_CPU) -Os -Wall
LDFLAGS = -mmcu=$(MCU)

# === REGRAS ===
all: $(PROJECT).hex

$(PROJECT).elf: $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $@

$(PROJECT).hex: $(PROJECT).elf
	$(OBJCOPY) -O ihex -R .eeprom $< $@

flash: $(PROJECT).hex
	avrdude -c $(PROGRAMMER) -p $(MCU) -U flash:w:$<:i

fuses:
	# Low fuse: 0xFF -> cristal externo 16 MHz ou 20 MHz, startup 16K CK + 65ms
	# High fuse: 0xDF -> habilita reset, preserva EEPROM
	# Extended fuse: 0xFF -> BOD desativado (pode ajustar se quiser)
	avrdude -c $(PROGRAMMER) -p $(MCU) -U lfuse:w:0xFF:m -U hfuse:w:0xDF:m -U efuse:w:0xFF:m

clean:
	rm -f $(PROJECT).elf $(PROJECT).hex
