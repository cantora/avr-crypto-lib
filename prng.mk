# Makefile for PRNG
ALGO_NAME := PRNG

# comment out the following line for removement of PRNG from the build process
PRNGS += $(ALGO_NAME)

$(ALGO_NAME)_OBJ      := prng.o
$(ALGO_NAME)_TEST_BIN := main.o debug.o uart.o serial-tools.o sha256-asm.o \
                         xtea-asm.o arcfour-asm.o prng.o cast5.o
                         
$(ALGO_NAME)_NESSIE_TEST      := "nessie"
$(ALGO_NAME)_PEROFRMANCE_TEST := "performance"

