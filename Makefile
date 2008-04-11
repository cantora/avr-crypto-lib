# Makefile for the micro-crypt project
# author: Daniel Otte

BLOCK_CIPHERS  := 
STREAM_CIPHERS := 
HASHES         :=
MACS           :=
PRNGS          := 

# we use the gnu make standard library
include gmsl
include avr-makefile.inc
include *.mk

ALGORITHMS = $(BLOCK_CIPHERS) $(STREAM_CIPHERS) $(HASHES) $(PRNGS) $(MACS)
ALGORITHMS_OBJ = $(patsubst %,%_OBJ, $(ALGORITHMS))
ALGORITHMS_OBJ_IMM = $(foreach a, $(ALGORITHMS_OBJ), $($(a)))
ALGORITHMS_TEST_BIN = $(patsubst %,%_TEST_BIN, $(ALGORITHMS))
ALGORITHMS_TEST_BIN_MAIN = $(foreach a, $(ALGORITHMS_TEST_BIN), $(firstword $($(a))))
ALGORITHMS_TEST_BIN_MAIN_ELF = $(patsubst %.o, %.elf, $(ALGORITHMS_TEST_BIN_MAIN))
ALGORITHMS_TEST_BIN_MAIN_HEX = $(patsubst %.o, %.hex, $(ALGORITHMS_TEST_BIN_MAIN))

ALGORITHMS_TEST_BIN_IMM =  $(foreach a, $(ALGORITHMS_TEST_BIN), $($(a)))
ALGORITHMS_NESSIE_TEST = $(patsubst %,%_NESSIE_TEST, $(ALGORITHMS))
ALGORITHMS_PERFORMANCE_TEST = $(patsubst %,%_PERORMANCE_TEST, $(ALGORITHMS))

#ALGORITHMS_LC = #algorithm names in lowercase 
#ALGORITHMS_LC = $(foreach a, $(ALGORITHMS), $$(lc Text))
ALGORITHMS_LC = $(call lc,$(ALGORITHMS))
PRG = remove_me

DEFS	   =
LIBS	   =

define BLA_TEMPLATE2
$(2): $(3)
	@echo "[gcc]: $$@"
#	echo $$^
	@$(CC) $(CFLAGS) $(LDFLAGS)$(patsubst %.elf,%.map,$(2)) -o \
	$(2) \
	$(3) \
	$(LIBS)
endef

$(foreach algo, $(ALGORITHMS), $(eval $(call BLA_TEMPLATE2, $(algo), $(patsubst %.o,%.elf,$(firstword $($(algo)_TEST_BIN))), $($(algo)_TEST_BIN) )))

.PHONY: info
info:
	@echo "infos on micro-crypt:"
	@echo "  block ciphers:"
	@echo "    $(BLOCK_CIPHERS)"
	@echo "  stream ciphers:"
	@echo "    $(STREAM_CIPHERS)"
	@echo "  hash functions:"
	@echo "    $(HASHES)"
	@echo "  MAC functions:"
	@echo "    $(MACS)"
	@echo "  PRNG functions:"
	@echo "    $(PRNGS)"
	@echo "  LC functions:"
	@echo "    $(ALGORITHMS_LC)"
	
#	echo $(ALGORITHMS_TEST_BIN_MAIN)
#	echo $(ALGORITHMS)
#	echo $(firstword $(XTEA_TEST_BIN))
#	echo $(patsubst %.o,%.elf,$(firstword $(XTEA_TEST_BIN)))
#	echo $(ALGORITHMS_OBJ)
#	echo $(ALGORITHMS_OBJ_IMM)
#	echo $(ALGORITHMS_TEST_BIN)
#	echo $(ALGORITHMS_NESSIE_TEST)
#	echo $(ALGORITHMS_PERFORMANCE_TEST)

%.o: %.c
	@echo "[gcc]: $@"
	@$(CC) $(CFLAGS) -c	-o $@ $<

%.o: %.S
	@echo "[as] : $@"
	@$(CC) $(ASFLAGS) -c -o $@ $<


.PHONY: cores
cores: $(ALGORITHMS_OBJ)

.PHONY: blockciphers
blockciphers: $(patsubst %, %_OBJ, $(BLOCK_CIPHERS))

.PHONY: streamciphers
streamciphers: $(patsubst %, %_OBJ, $(STREAM_CIPHERS))

.PHONY:  hashes
hashes: $(patsubst %, %_OBJ, $(HASHES))

.PHONY:  macs
macs: $(patsubst %, %_OBJ, $(MACS))

prngs: $(patsubst %, %_OBJ, $(PRNGS))

tests: $(ALGORITHMS_TEST_BIN) \
       $(ALGORITHMS_TEST_BIN_MAIN_ELF) \
       $(ALGORITHMS_TEST_BIN_MAIN_HEX)


define OBJ_TEMPLATE
$(1)_OBJ: $(2)
#	@echo " ALGO: $(1)"
#	@echo " REQ:  $(2)"
endef

$(foreach algo, $(ALGORITHMS), $(eval $(call OBJ_TEMPLATE, $(algo), $($(algo)_OBJ))))


$(BLOCK_CIPHERS_OBJ): $(patsubst %,%_OBJ, $(BLOCK_CIPHERS)) 
$(STREAM_CIPHERS_OBJ): $(patsubst %,%_OBJ, $(STREAM_CIPHERS))
$(HASHES_OBJ): $(patsubst %,%_OBJ, $(HASHES))
$(PRNGS_OBJ): $(patsubst %,%_OBJ, $(PRNGS))
$(MACS_OBJ): $(patsubst %,%_OBJ, $(MACS))

$(ALGORITHMS_TEST_BIN): $(ALGORITHMS_TEST_BIN_IMM)


define SIZE_TEMPLATE
$(1)_size.txt: $(2)
	@echo " ALGO: $(1)"
	@echo " REQ:  $(2)"
	$(SIZE) $(2) > $(1)_size.txt
endef

$(foreach algo, $(ALGORITHMS), $(eval $(call SIZE_TEMPLATE, $(call lc,$(algo)), $($(algo)_OBJ))))


	
.PHONY: all
all: $(PRG).elf lst text eeprom


.PHONY: clean
clean:
	rm -rf *.o *.elf *.eps *.png *.pdf *.bak 
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

#%_size.txt: %.o
#	$(SIZE)  $< > $@
	
	
	
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
