# Makefile for SHABEA
ALGO_NAME := SHABEA

# comment out the following line for removement of SHABEA from the build process
BLOCK_CIPHERS := $(BLOCK_CIPHERS) $(ALGO_NAME)

$(ALGO_NAME)_OBJ      := shabea.o sha256-asm.o
$(ALGO_NAME)_TEST_BIN := main-shabea-test.o debug.o uart.o serial-tools.o \
                         shabea.o sha256-asm.o
$(ALGO_NAME)_NESSIE_TEST      := "nessie"
$(ALGO_NAME)_PEROFRMANCE_TEST := "performance"

