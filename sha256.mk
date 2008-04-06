# Makefile for SHA256
ALGO_NAME := SHA256

# comment out the following line for removement of SHA256 from the build process
HASHES += $(ALGO_NAME)

$(ALGO_NAME)_OBJ      := sha256-asm.o
$(ALGO_NAME)_TEST_BIN := main.o debug.o uart.o serial-tools.o sha256-asm.o \
                         xtea-asm.o arcfour-asm.o prng.o cast5.o
$(ALGO_NAME)_NESSIE_TEST      := "nessie"
$(ALGO_NAME)_PEROFRMANCE_TEST := "performance"

