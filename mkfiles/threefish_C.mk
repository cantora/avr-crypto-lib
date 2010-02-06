# Makefile for threefish
ALGO_NAME := THREEFISH_C

# comment out the following line for removement of threefish from the build process
BLOCK_CIPHERS += $(ALGO_NAME)

$(ALGO_NAME)_DIR      := skein/
$(ALGO_NAME)_OBJ      := threefish256_enc.o threefish512_enc.o threefish1024_enc.o threefish_mix_c.o \
                         threefish_invmix_c.o threefish256_dec.o threefish512_dec.o threefish1024_dec.o
$(ALGO_NAME)_TEST_BIN := main-threefish-test.o $(CLI_STD) \
                         nessie_bc_test.o nessie_common.o performance_test.o
$(ALGO_NAME)_NESSIE_TEST      := test nessie
$(ALGO_NAME)_PERFORMANCE_TEST := performance

