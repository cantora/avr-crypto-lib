# Makefile for CAST6 (a.k.a. CAST-256)
ALGO_NAME := CAST6

# comment out the following line for removement of CAST6 from the build process
BLOCK_CIPHERS += $(ALGO_NAME)

$(ALGO_NAME)_DIR      := cast6/
$(ALGO_NAME)_OBJ      := cast6.o
$(ALGO_NAME)_TEST_BIN := main-cast6-test.o $(CLI_STD) \
                         nessie_bc_test.o nessie_common.o performance_test.o
$(ALGO_NAME)_NESSIE_TEST      := test nessie
$(ALGO_NAME)_PERFORMANCE_TEST := performance

