# Makefile for camellia
ALGO_NAME := CAMELLIA

# comment out the following line for removement of serpent from the build process
BLOCK_CIPHERS += $(ALGO_NAME)

# main-camellia-test.o debug.o uart.o serial-tools.o camellia.o camellia-asm.o
$(ALGO_NAME)_OBJ      := camellia.o camellia-asm.o
$(ALGO_NAME)_TEST_BIN := main-camellia-test.o debug.o uart.o serial-tools.o \
                         camellia.o camellia-asm.o
$(ALGO_NAME)_NESSIE_TEST      := "nessie"
$(ALGO_NAME)_PEROFRMANCE_TEST := "performance"

