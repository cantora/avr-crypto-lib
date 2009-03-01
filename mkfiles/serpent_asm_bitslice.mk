# Makefile for serpent
ALGO_NAME := SERPENT_ASM_BITSLICE

# comment out the following line for removement of serpent from the build process
BLOCK_CIPHERS += $(ALGO_NAME)


$(ALGO_NAME)_OBJ      := serpent-sboxes-bitslice-asm.o serpent-asm.o memxor.o
$(ALGO_NAME)_TEST_BIN := main-serpent-test.o debug.o uart.o hexdigit_tab.o serial-tools.o \
                         nessie_bc_test.o dbz_strings.o nessie_common.o cli.o string-extras.o performance_test.o
$(ALGO_NAME)_NESSIE_TEST      := "nessie"
$(ALGO_NAME)_PERFORMANCE_TEST := "performance"

