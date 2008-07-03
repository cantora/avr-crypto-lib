
OBJ = $(SERPENT_OBJ)
MCU_TARGET     = atmega644
OPTIMIZE       = -Os
DEFS           = -D$(call uc, $(MCU_TARGET))
FLASHCMD       = avrdude -p $(MCU_TARGET) -P /dev/ttyUSB0 -c avr911 -U flash:w:# no space at the end
DEP_DIR        = deps/
BIN_DIR        = bin/
TESTBIN_DIR    = test_bin/
#uisp -dprog=bsd -dlpt=/dev/parport1 --upload if=$(PRG).hex
ERASECMD       = 

CC	     = avr-gcc

override CFLAGS	= -MMD -MF$(DEP_DIR)$(patsubst %.c,%.d,$^) -pedantic -std=c99 -Wall -Wstrict-prototypes  $(OPTIMIZE) -mmcu=$(MCU_TARGET) $(DEFS)

override LDFLAGS       = -Wl,-Map,
override ASFLAGS	   = -mmcu=$(MCU_TARGET)

SIZESTAT_FILE = sizestats.txt

OBJCOPY	= avr-objcopy
OBJDUMP	= avr-objdump
SIZE = avr-size
RUBY = ruby
