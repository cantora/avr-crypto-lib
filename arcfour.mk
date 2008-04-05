# Makefile for ARCFOUR (RC4 compatible)
ALGO_NAME := ARCFOUR

# comment out the following line for removement of ARCFOUR from the build process
BLOCK_CIPHERS := $(BLOCK_CIPHERS) $(ALGO_NAME)

$(ALGO_NAME)_OBJ      := arcfour-asm.o
$(ALGO_NAME)_TEST_BIN := main.o debug.o uart.o serial-tools.o sha256-asm.o \
                         xtea-asm.o arcfour-asm.o prng.o cast5.o
$(ALGO_NAME)_NESSIE_TEST      := "nessie"
$(ALGO_NAME)_PEROFRMANCE_TEST := "performance"

