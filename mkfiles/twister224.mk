# Makefile for TWISTER-224
ALGO_NAME := TWISTER224

# comment out the following line for removement of TWISTER-224 from the build process
HASHES += $(ALGO_NAME)

$(ALGO_NAME)_OBJ      := twister-small-asm.o twister-asm.o twister224.o 
$(ALGO_NAME)_TEST_BIN := main-twister224-test.o debug.o uart.o serial-tools.o \
                         nessie_hash_test.o nessie_common.o cli.o string-extras.o performance_test.o
$(ALGO_NAME)_NESSIE_TEST      := "nessie"
$(ALGO_NAME)_PERFORMANCE_TEST := "performance"

