# Makefile for Keccak
ALGO_NAME := KECCAK_ASM

# comment out the following line for removement of Keccak from the build process
HASHES += $(ALGO_NAME)

$(ALGO_NAME)_DIR      := keccak/
$(ALGO_NAME)_INCDIR   := memxor/ hfal/
$(ALGO_NAME)_OBJ      := keccak-asm.o
$(ALGO_NAME)_TESTBIN  := main-keccak-test.o hfal_keccak.o  $(CLI_STD) $(HFAL_STD)
$(ALGO_NAME)_NESSIE_TEST      := test nessie
$(ALGO_NAME)_PERFORMANCE_TEST := performance

