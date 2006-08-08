PRG	    = rc6
# cryptotest
OBJ	    = main-rc6-test.o debug.o uart.o serial-tools.o rc6.o
# main-md5-test.o debug.o uart.o serial-tools.o md5.o
# main-cast5-test.o debug.o uart.o serial-tools.o cast5.o
# main.o debug.o uart.o serial-tools.o sha256-asm.o xtea-asm.o arcfour-asm.o prng.o cast5.o
MCU_TARGET     = atmega32
OPTIMIZE       = -Os

DEFS	   =
LIBS	   =

# You should not have to change anything below here.

CC	     = avr-gcc

# Override is only needed by avr-lib build system.

override CFLAGS	= -Wall -Wstrict-prototypes  $(OPTIMIZE) -mmcu=$(MCU_TARGET) 
$(DEFS)
override LDFLAGS       = -Wl,-Map,$(PRG).map
override ASFLAGS	   = -mmcu=$(MCU_TARGET)

OBJCOPY	= avr-objcopy
OBJDUMP	= avr-objdump

all: $(PRG).elf lst text eeprom

$(PRG).elf: $(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

clean:
	rm -rf *.o $(PRG).elf *.eps *.png *.pdf *.bak 
	rm -rf *.lst *.map $(EXTRA_CLEAN_FILES)

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
