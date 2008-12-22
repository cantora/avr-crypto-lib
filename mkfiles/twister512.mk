# Makefile for TWISTER-512
ALGO_NAME := TWISTER512

# comment out the following line for removement of TWISTER-512 from the build process
HASHES += $(ALGO_NAME)

$(ALGO_NAME)_OBJ      := twister.o twister-big.o memxor.o gf256mul.o
$(ALGO_NAME)_TEST_BIN := main-twister512-test.o debug.o uart.o serial-tools.o \
                         nessie_hash_test.o nessie_common.o cli.o performance_test.o
$(ALGO_NAME)_NESSIE_TEST      := "nessie"
$(ALGO_NAME)_PERFORMANCE_TEST := "performance"

