# Makefile for TWISTER-256
ALGO_NAME := TWISTER256_C

# comment out the following line for removement of TWISTER-256 from the build process
HASHES += $(ALGO_NAME)

$(ALGO_NAME)_OBJ      := twister.o twister-small.o memxor.o gf256mul.o
$(ALGO_NAME)_TEST_BIN := main-twister256-test.o debug.o uart.o serial-tools.o \
                         nessie_hash_test.o nessie_common.o cli.o performance_test.o
$(ALGO_NAME)_NESSIE_TEST      := "nessie"
$(ALGO_NAME)_PERFORMANCE_TEST := "performance"

