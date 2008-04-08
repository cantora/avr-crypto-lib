# Makefile for present
ALGO_NAME := PRESENT

# comment out the following line for removement of present from the build process
BLOCK_CIPHERS += $(ALGO_NAME)


$(ALGO_NAME)_OBJ      := present.o
$(ALGO_NAME)_TEST_BIN := main-present-test.o debug.o uart.o serial-tools.o \
                         present.o nessie_bc_test.o nessie_common.o cli.o
$(ALGO_NAME)_NESSIE_TEST      := "nessie"
$(ALGO_NAME)_PEROFRMANCE_TEST := "performance"

