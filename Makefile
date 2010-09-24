# Makefile for the AVR-Crypto-Lib project
#
#    This file is part of the AVR-Crypto-Lib.
#    Copyright (C) 2010 Daniel Otte (daniel.otte@rub.de)
#
#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.

SHELL = sh

BLOCK_CIPHERS  :=
STREAM_CIPHERS :=
HASHES         :=
MACS           :=
PRNGS          :=
ENCODINGS      :=
SIGNATURE      :=
PK_CIPHERS     :=
AUX            :=


# we use the gnu make standard library
include gmsl
include avr-makefile.inc


GLOBAL_INCDIR := ./ $(TESTSRC_DIR)

#-------------------------------------------------------------------------------
# inclusion of make stubs
include mkfiles/*.mk

#-------------------------------------------------------------------------------
ALGORITHMS = $(BLOCK_CIPHERS) $(STREAM_CIPHERS) $(HASHES) $(PRNGS) $(MACS) \
			 $(ENCODINGS) $(SIGNATURE) $(PK_CIPHERS) $(AUX)
ALGORITHMS_OBJ = $(patsubst %,%_OBJ, $(ALGORITHMS))
ALGORITHMS_TEST_BIN = $(patsubst %,%_TEST_BIN, $(ALGORITHMS))

#-------------------------------------------------------------------------------
# define binary object in $(BIN_DIR)$(ALGO)/<obj>
define Assert_Template
$(1) = $(2)
endef

$(foreach a, $(ALGORITHMS), $(eval $(call Assert_Template, \
    $(a)_BINOBJ, \
    $(addprefix $(BIN_DIR)$(call lc,$(a))/,$($(a)_OBJ)) \
)))

$(foreach a, $(ALGORITHMS), $(eval $(call Assert_Template, \
    $(a)_TESTBINOBJ, \
    $(addprefix $(BIN_DIR)$(call lc,$(a))/$(TEST_DIR),$($(a)_TEST_BIN)) \
)))



#-------------------------------------------------------------------------------

define TargetSource_Template
$(1): $(2)
	@echo "[cc]: $(1) <-- $(2)"
	@mkdir -p $(dir $(1))
	@$(CC) $(CFLAGS_A) -I./$(strip $(3)) -c -o $(1) $(2)
endef

# ----------------------------------------------------------------------------
# Function:  find_source_file
# Arguments: 1: name of the binary file (.o extension) to search
#            2: list of directorys to search for file
# Returns:   Returns paths to source file (mathing the pattern in 
#            $(SOURCE_PATTERN)
# ----------------------------------------------------------------------------
SOURCE_PATTERN := %.S %.c 
find_source_file = $(firstword $(foreach d, $(2), \
                     $(filter $(SOURCE_PATTERN),  \
                       $(wildcard $(d)$(notdir $(patsubst %.o,%,$1)).*) \
                     ) \
                   ) )
              
              
$(foreach a, $(ALGORITHMS), \
  $(foreach b, $($(a)_OBJ), \
    $(eval $(call TargetSource_Template, \
      $(BIN_DIR)$(call lc, $(a))/$(b), \
       $(call find_source_file, $(b), $($(a)_DIR) $($(a)_INCDIR) $(GLOBAL_INCDIR) ),\
      $($(a)_DIR) \
    )) \
  ) \
)

$(foreach a, $(ALGORITHMS), \
  $(foreach b, $($(a)_TEST_BIN), \
    $(eval $(call TargetSource_Template, \
      $(BIN_DIR)$(call lc, $(a))/$(TEST_DIR)$(b), \
       $(call find_source_file, $(b), $($(a)_DIR) $($(a)_INCDIR) $(GLOBAL_INCDIR) ),\
      $($(a)_DIR) \
    )) \
  ) \
)

#-------------------------------------------------------------------------------

define MainTestElf_Template
$(1): $(2) $(3)
	@echo "[ld]: $(1)"
	@$(CC) $(CFLAGS_A) $(LDFLAGS)$(patsubst %.elf,%.map,$(1)) -o \
	$(1) \
	$(2) $(3) \
	$(LIBS)
endef

$(foreach a, $(ALGORITHMS), \
    $(eval $(call MainTestElf_Template,  \
        $(BIN_DIR)$(call lc, $(a))/$(TEST_DIR)main-$(call lc, $(a))-test.elf, \
        $($(a)_BINOBJ), \
        $($(a)_TESTBINOBJ) \
	)) \
)

#-------------------------------------------------------------------------------

all: $(foreach algo, $(ALGORITHMS), $($(algo)_BINOBJ))

#-------------------------------------------------------------------------------

define TestBin_TEMPLATE
$(1)_TEST_BIN: $(2)
endef

$(foreach algo, $(ALGORITHMS), $(eval $(call TestBin_TEMPLATE, \
    $(algo), \
    $(BIN_DIR)$(call lc, $(algo))/$(TEST_DIR)main-$(call lc, $(algo))-test.elf \
)))

#-------------------------------------------------------------------------------

%.hex: %.elf
	@echo "[objcopy]: $@"
	@$(OBJCOPY) -j .text -j .data -O ihex $< $@

#-------------------------------------------------------------------------------

define Flash_Template
$(1)_FLASH: $(2)
	@echo "[flash]: $(2)"
	@$(FLASHCMD)$(call first,$(2))
endef

$(foreach algo, $(ALGORITHMS), $(eval $(call Flash_Template, \
    $(algo), \
    $(BIN_DIR)$(call lc, $(algo))/$(TEST_DIR)main-$(call lc, $(algo))-test.hex \
)))

#-------------------------------------------------------------------------------

define Speed_Template
$(1)_SPEED:  $(1)_FLASH
	@$(RUBY) $(SPEEDTOOL) -c $(SPEEDCMD) -t $(SPEEDLOG_DIR) -a $(call lc, $(1))
endef

$(foreach algo, $(ALGORITHMS), $(eval $(call Speed_Template, \
    $(algo), $(algo) \
)))

.PHONY: hash_speed
hash_speed: $(foreach algo, $(HASHES), $(algo)_SPEED)

.PHONY: blockcipher_speed
blockcipher_speed: $(foreach algo, $(BLOCK_CIPHERS), $(algo)_SPEED)
#-------------------------------------------------------------------------------


define Size_Template
$(1)_SIZE:  $(2)
	@echo "[size] $(1)"
	$(SIZE) $(2) > $(strip $(SIZE_DIR))$(strip $(call lc, $(1))).size
endef

$(foreach algo, $(ALGORITHMS), $(eval $(call Size_Template, \
    $(strip $(algo)), $($(algo)_BINOBJ) \
)))

.PHONY: hash_size
hash_size: $(foreach algo, $(HASHES), $(algo)_SIZE)

.PHONY: blockcipher_size
blockcipher_size: $(foreach algo, $(BLOCK_CIPHERS), $(algo)_SIZE)

#-------------------------------------------------------------------------------

.PHONY: tests
tests: $(foreach a, $(ALGORITHMS), $(a)_TEST_BIN)

#-------------------------------------------------------------------------------

define TestRun_Template
$(1)_TESTRUN: $(1)_FLASH
	@echo "[test]: $(1)"
	$(RUBY) $(GET_TEST) $(TESTPORT) $(TESTPORTBAUDR) 8 1 nessie $(TESTLOG_DIR)$(TESTPREFIX) $(2)
endef

$(foreach algo, $(ALGORITHMS),$(eval $(call TestRun_Template, $(algo), $(call lc,$(algo)) )))

all_testrun: $(foreach algo, $(ALGORITHMS), $(algo)_TESTRUN)

#-------------------------------------------------------------------------------

define Obj_Template
$(1)_OBJ: $(2)
endef

$(foreach algo, $(ALGORITHMS), \
    $(eval $(call Obj_Template, \
        $(algo), \
        $($(algo)_BINOBJ)\
	))\
)

.PHONY: cores
cores: $(foreach algo, $(ALGORITHMS), $(algo)_OBJ)

.PHONY: blockchiphers
blockciphers: $(foreach algo, $(BLOCK_CIPHERS), $(algo)_OBJ)

.PHONY: streamchiphers
streamciphers: $(foreach algo, $(STREAM_CIPHERS), $(algo)_OBJ)

.PHONY: hashes
hashes: $(foreach algo, $(HASHES), $(algo)_OBJ)

.PHONY: macs
macs: $(foreach algo, $(MACS), $(algo)_OBJ)

.PHONY: prngs
prngs: $(foreach algo, $(PRNGS), $(algo)_OBJ)

.PHONY: encodings
encodings: $(foreach algo, $(ENCODINGS), $(algo)_OBJ)

.PHONY: aux
aux: $(foreach algo, $(AUX), $(algo)_OBJ)


#-------------------------------------------------------------------------------


.PHONY: help
help: info
.PHONY: info
info:
	@echo "infos on AVR-Crypto-lib:"
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
	@echo "  signature functions:"
	@echo "    $(SIGNATURE)"
	@echo "  public key ciphers:"
	@echo "    $(PK_CIPHERS)"
	@echo "  encodings:"
	@echo "    $(ENCODINGS)"
	@echo "  auxiliary functions:"
	@echo "    $(AUX)"
	@echo " targets:"
	@echo "  all                - all algorithm cores"
	@echo "  cores              - all algorithm cores"
	@echo "  listings           - all algorithm core listings"
	@echo "  tests              - all algorithm test programs"
	@echo "  stats              - all algorithm size statistics"
	@echo "  blockciphers       - all blockcipher cores"
	@echo "  streamciphers      - all streamcipher cores"
	@echo "  hashes             - all hash cores"
	@echo "  macs               - all MAC cores"
	@echo "  prngs              - all PRNG cores"
	@echo "  all_testrun        - testrun all algorithms"
	@echo "  hash_size          - measure size of all hash functions"
	@echo "  hash_speed         - measure performance of all hash functions"
	@echo "  blockcipher_size   - measure size of all blockciphers"
	@echo "  blockcipher_speed  - measure performance of all blockciphers"
	@echo "  docu               - build doxygen documentation"
	@echo "  clean              - remove a lot of builded files"
	@echo "  depclean           - also remove dependency files"
	@echo "  *_TEST_BIN         - build test program"
	@echo "  *_TESTRUN          - run nessie test"
	@echo "  *_OBJ              - build algorithm core"
	@echo "  *_FLASH            - flash test program"
	@echo "  *_LIST             - build assembler listing"


#-------------------------------------------------------------------------------

.PHONY: clean
clean:
	rm -rf $(BIN_DIR)*

.PHONY: depclean
depclean: clean
	rm -f $(DEP_DIR)*.d

#-------------------------------------------------------------------------------
# dependency inclusion
#

DEPS := $(wildcard $(DEP_DIR)*.d)

ifneq ($(DEPS),)
include $(DEPS)
endif

