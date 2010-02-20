# Makefile for present
ALGO_NAME := PRESENT

# comment out the following line for removement of present from the build process
BLOCK_CIPHERS += $(ALGO_NAME)

$(ALGO_NAME)_DIR      := present/
$(ALGO_NAME)_OBJ      := present.o
$(ALGO_NAME)_TEST_BIN := main-present-test.o bcal_present.o $(CLI_STD) $(BCAL_STD)
$(ALGO_NAME)_NESSIE_TEST      := "nessie"
$(ALGO_NAME)_PERFORMANCE_TEST := "performance"

