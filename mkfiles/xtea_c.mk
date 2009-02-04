# Makefile for XTEA
ALGO_NAME := XTEA_C

# comment out the following line for removement of XTEA from the build process
BLOCK_CIPHERS += $(ALGO_NAME)

$(ALGO_NAME)_OBJ      := xtea.o
$(ALGO_NAME)_TEST_BIN := main-xtea-test.o debug.o uart.o serial-tools.o \
                         nessie_bc_test.o nessie_common.o cli.o string-extras.o performance_test.o
$(ALGO_NAME)_NESSIE_TEST      := "nessie"
$(ALGO_NAME)_PERFORMANCE_TEST := "performance"

