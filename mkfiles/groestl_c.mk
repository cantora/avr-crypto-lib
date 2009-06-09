# Makefile for Blake
ALGO_NAME := GROESTL_C

# comment out the following line for removement of Grøestl from the build process
HASHES += $(ALGO_NAME)


$(ALGO_NAME)_OBJ      := groestl_small.o memxor.o aes_sbox.o gf256mul.o
$(ALGO_NAME)_TEST_BIN := main-groestl-test.o hfal_groestl_small.o $(CLI_STD) $(HFAL_STD)
$(ALGO_NAME)_NESSIE_TEST      := test nessie
$(ALGO_NAME)_PERFORMANCE_TEST := performance

