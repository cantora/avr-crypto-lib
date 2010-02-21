# Makefile for CubeHash 
ALGO_NAME := CUBEHASH_C

# comment out the following line for removement of CubeHash from the build process
HASHES += $(ALGO_NAME)

$(ALGO_NAME)_DIR      := cubehash/
$(ALGO_NAME)_OBJ      := cubehash.o memxor.o
$(ALGO_NAME)_TEST_BIN := main-cubehash-test.o hfal_cubehash.o  $(CLI_STD) $(HFAL_STD)
$(ALGO_NAME)_NESSIE_TEST      := test nessie
$(ALGO_NAME)_PERFORMANCE_TEST := performance

