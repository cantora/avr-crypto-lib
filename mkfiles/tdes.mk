# Makefile for Triple-DES
ALGO_NAME := TDES

# comment out the following line for removement of Triple-DES from the build process
BLOCK_CIPHERS += $(ALGO_NAME)

$(ALGO_NAME)_DIR      := des/
$(ALGO_NAME)_OBJ      := des.o
$(ALGO_NAME)_TEST_BIN := main-tdes-test.o $(CLI_STD) \
                         nessie_bc_test.o nessie_common.o performance_test.o
$(ALGO_NAME)_NESSIE_TEST      := "nessie"
$(ALGO_NAME)_PERFORMANCE_TEST := "performance"

