# Makefile for Keccak
ALGO_NAME := KECCAK_C

# comment out the following line for removement of Keccak from the build process
HASHES += $(ALGO_NAME)

$(ALGO_NAME)_DIR      := keccak/
$(ALGO_NAME)_OBJ      := keccak.o memxor.o
$(ALGO_NAME)_TEST_BIN := main-keccak-test.o hfal_keccak.o  $(CLI_STD) $(HFAL_STD)
$(ALGO_NAME)_NESSIE_TEST      := test nessie
$(ALGO_NAME)_PERFORMANCE_TEST := performance

