PRG        = serpent-test
#PRG	    = tdes-test
# camellia
# cryptotest
SERPENT_OBJ     = main-serpent-test.o debug.o uart.o serial-tools.o serpent.o nessie_bc_test.o
CAMELLIA_OBJ	= main-camellia-test.o debug.o uart.o serial-tools.o camellia.o camellia-asm.o
SKIPJACK_OBJ	= main-skipjack-test.o debug.o uart.o serial-tools.o skipjack.o
SHA1_OBJ		= main-sha1-test.o debug.o uart.o serial-tools.o sha1-asm.o
MD5_OBJ			= main-md5-test.o debug.o uart.o serial-tools.o md5.o
CAST5_OBJ		= main-cast5-test.o debug.o uart.o serial-tools.o cast5.o
RC6_OBJ			= main-rc6-test.o debug.o uart.o serial-tools.o rc6.o
Multi_OBJ		= main.o debug.o uart.o serial-tools.o sha256-asm.o xtea-asm.o arcfour-asm.o prng.o cast5.o
DES_OBJ			= main-des-test.o debug.o uart.o serial-tools.o des.o
TDES_OBJ	    = main-tdes-test.o debug.o uart.o serial-tools.o des.o
SEED_OBJ	    = main-seed-test.o debug.o uart.o serial-tools.o seed.o seed-asm.o
SHABEA_OBJ	    = main-shabea-test.o debug.o uart.o serial-tools.o shabea.o sha256-asm.o

OBJ = $(SERPENT_OBJ)
MCU_TARGET     = atmega32
OPTIMIZE       = -Os

FLASHCMD       = avrdude -p $(MCU_TARGET) -P /dev/ttyUSB0 -c avr911 -U flash:w:$(PRG).hex
#  -U eeprom:w:$(PRG)_eeprom.hex
#uisp -dprog=bsd -dlpt=/dev/parport1 --upload if=$(PRG).hex
ERASECMD       = 

DEFS	   =
LIBS	   =

# You should not have to change anything below here.

CC	     = avr-gcc

# Override is only needed by avr-lib build system.

override CFLAGS	= -pedantic -std=c99 -Wall -Wstrict-prototypes  $(OPTIMIZE) -mmcu=$(MCU_TARGET) 
$(DEFS)
override LDFLAGS       = -Wl,-Map,$(PRG).map
override ASFLAGS	   = -mmcu=$(MCU_TARGET)

OBJCOPY	= avr-objcopy
OBJDUMP	= avr-objdump

all: $(PRG).elf lst text eeprom

$(PRG).elf: $(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

#rc6-test: 

clean:
	rm -rf *.o $(PRG).elf *.eps *.png *.pdf *.bak 
	rm -rf *.lst *.map $(EXTRA_CLEAN_FILES)

flash:
	$(ERASECMD)
	$(FLASHCMD)
	
lst:  $(PRG).lst

%.lst: %.elf
	$(OBJDUMP) -h -S $< > $@

# Rules for building the .text rom images

text: hex bin srec

hex:  $(PRG).hex
bin:  $(PRG).bin
srec: $(PRG).srec

%.hex: %.elf
	$(OBJCOPY) -j .text -j .data -O ihex $< $@

%.srec: %.elf
	$(OBJCOPY) -j .text -j .data -O srec $< $@

%.bin: %.elf
	$(OBJCOPY) -j .text -j .data -O binary $< $@

# Rules for building the .eeprom rom images

eeprom: ehex ebin esrec

ehex:  $(PRG)_eeprom.hex
ebin:  $(PRG)_eeprom.bin
esrec: $(PRG)_eeprom.srec

%_eeprom.hex: %.elf
	$(OBJCOPY) -j .eeprom --change-section-lma .eeprom=0 -O ihex $< $@

%_eeprom.srec: %.elf
	$(OBJCOPY) -j .eeprom --change-section-lma .eeprom=0 -O srec $< $@

%_eeprom.bin: %.elf
	$(OBJCOPY) -j .eeprom --change-section-lma .eeprom=0 -O binary $< $@

# Every thing below here is used by avr-libc's build system and can be ignored
# by the casual user.

FIG2DEV		 = fig2dev
EXTRA_CLEAN_FILES       = *.hex *.bin *.srec

dox: eps png pdf

eps: $(PRG).eps
png: $(PRG).png
pdf: $(PRG).pdf

%.eps: %.fig
	$(FIG2DEV) -L eps $< $@

%.pdf: %.fig
	$(FIG2DEV) -L pdf $< $@

%.png: %.fig
	$(FIG2DEV) -L png $< $@
