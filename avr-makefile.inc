
MCU_TARGET     = atmega644
OPTIMIZE       = -Os
PROGRAMMER     = stk500v2 # avr911
DEFS           = -D$(call uc, $(MCU_TARGET))
FLASHCMD       = avrdude -p $(MCU_TARGET) -P /dev/ttyUSB0 -c $(PROGRAMMER) -U flash:w:# no space at the end
#FLASHCMD       = avrdude -p $(MCU_TARGET) -c usbasp -U flash:w:# no space at the end
DEP_DIR        = deps/
BIN_DIR        = bin/
TESTBIN_DIR    = test_bin/
TESTSRC_DIR    = test_src/
#uisp -dprog=bsd -dlpt=/dev/parport1 --upload if=$(PRG).hex
ERASECMD       = 
TESTPORT       = /dev/ttyUSB1
TESTPORTBAUDR  = 38400
TESTLOG_DIR    = testlog/
TESTPREFIX     = nessie-
LIST_DIR       = listings/
STAT_DIR       = stats/
CC	       = avr-gcc
CSTD           = c99

override CFLAGS	= -MMD -MF$(DEP_DIR)$(patsubst %.c,%.d,$(patsubst $(TESTSRC_DIR)%,%,$<)) -I. -gdwarf-2 -pedantic -std=$(CSTD) -Wall -Wstrict-prototypes  $(OPTIMIZE) -mmcu=$(MCU_TARGET) $(DEFS)

override LDFLAGS       = -gdwarf-2 -Wl,-Map,
override ASFLAGS	   = -mmcu=$(MCU_TARGET) -Wa,--gdwarf-2

SIZESTAT_FILE = sizestats.txt

OBJCOPY	= avr-objcopy
OBJDUMP	= avr-objdump
SIZE = avr-size
RUBY = ruby
GET_TEST = host/get_test.rb
MAKE = make
MAKE2GRAPH = ~/bin/make2graph.rb
TWOPI = twopi
