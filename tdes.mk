# Makefile for triple-DES
ALGO_NAME := TDES

# comment out the following line for removement of triple-DES from the build process
BLOCK_CIPHERS += $(ALGO_NAME)

$(ALGO_NAME)_OBJ      := des.o
$(ALGO_NAME)_TEST_BIN := main-tdes-test.o debug.o uart.o serial-tools.o des.o
$(ALGO_NAME)_NESSIE_TEST      := "nessie"
$(ALGO_NAME)_PEROFRMANCE_TEST := "performance"

