# Makefile for noekeon
ALGO_NAME := OMAC_NOEKEON

# comment out the following line for removement of noekeon from the build process
MACS += $(ALGO_NAME)


$(ALGO_NAME)_OBJ      := noekeon_asm.o omac_noekeon.o memxor.o
$(ALGO_NAME)_TEST_BIN := main-omac-noekeon-test.o debug.o uart.o hexdigit_tab.o serial-tools.o \
                         nessie_mac_test.o nessie_common.o cli.o string-extras.o performance_test.o
$(ALGO_NAME)_NESSIE_TEST      := test nessie
$(ALGO_NAME)_PERFORMANCE_TEST := performance

