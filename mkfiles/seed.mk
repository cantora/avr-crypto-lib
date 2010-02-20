# Makefile for SEED
ALGO_NAME := SEED

# comment out the following line for removement of SEED from the build process
BLOCK_CIPHERS += $(ALGO_NAME)

$(ALGO_NAME)_DIR      := seed/
$(ALGO_NAME)_OBJ      := seed-asm.o
$(ALGO_NAME)_TEST_BIN := main-seed-test.o bcal_seed.o $(CLI_STD) $(BCAL_STD) 
$(ALGO_NAME)_NESSIE_TEST      := "nessie"
$(ALGO_NAME)_PERFORMANCE_TEST := "performance"

