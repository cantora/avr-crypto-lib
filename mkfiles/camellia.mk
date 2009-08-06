# Makefile for camellia
ALGO_NAME := CAMELLIA

# comment out the following line for removement of Camellia from the build process
BLOCK_CIPHERS += $(ALGO_NAME)

$(ALGO_NAME)_DIR      := camellia/
$(ALGO_NAME)_OBJ      := camellia128-stub.o camellia-asm.o
$(ALGO_NAME)_TEST_BIN := main-camellia-test.o $(CLI_STD) nessie_bc_test.o \
			 nessie_common.o performance_test.o
$(ALGO_NAME)_NESSIE_TEST      := "nessie"
$(ALGO_NAME)_PERFORMANCE_TEST := "performance"

