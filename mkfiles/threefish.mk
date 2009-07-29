# Makefile for threefish
ALGO_NAME := THREEFISH

# comment out the following line for removement of threefish from the build process
BLOCK_CIPHERS += $(ALGO_NAME)


$(ALGO_NAME)_OBJ      := threefish256_enc_asm.o threefish512_enc_asm.o threefish1024_enc_asm.o\
                         threefish_mix.o threefish_invmix.o \
			 threefish256_dec_asm.o threefish512_dec_asm.o threefish1024_dec_asm.o
$(ALGO_NAME)_TEST_BIN := main-threefish-test.o $(CLI_STD) \
                         nessie_bc_test.o nessie_common.o performance_test.o
$(ALGO_NAME)_NESSIE_TEST      := test nessie
$(ALGO_NAME)_PERFORMANCE_TEST := performance

