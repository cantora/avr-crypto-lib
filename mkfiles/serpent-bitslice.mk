# Makefile for serpent
ALGO_NAME := SERPENT_BITSLICE

# comment out the following line for removement of serpent from the build process
BLOCK_CIPHERS += $(ALGO_NAME)


$(ALGO_NAME)_OBJ      := serpent-asm.o serpent-sboxes-bitslice-asm.o memxor.o
$(ALGO_NAME)_TEST_BIN := main-serpent-test.o debug.o uart.o serial-tools.o \
                         nessie_bc_test.o nessie_common.o cli.o string-extras.o performance_test.o
$(ALGO_NAME)_NESSIE_TEST      := "nessie"
$(ALGO_NAME)_PERFORMANCE_TEST := "performance"

