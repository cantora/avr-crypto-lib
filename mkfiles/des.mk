# Makefile for DES
ALGO_NAME := DES

# comment out the following line for removement of DES from the build process
BLOCK_CIPHERS += $(ALGO_NAME)

$(ALGO_NAME)_OBJ      := des.o
$(ALGO_NAME)_TEST_BIN := main-des-test.o debug.o uart.o serial-tools.o des.o \
                         nessie_bc_test.o nessie_common.o cli.o performance_test.o
$(ALGO_NAME)_NESSIE_TEST      := "nessie"
$(ALGO_NAME)_PEROFRMANCE_TEST := "performance"

