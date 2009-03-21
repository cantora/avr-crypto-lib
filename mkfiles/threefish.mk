# Makefile for threefish
ALGO_NAME := THREEFISH

# comment out the following line for removement of threefish from the build process
BLOCK_CIPHERS += $(ALGO_NAME)


$(ALGO_NAME)_OBJ      := threefish256_enc_asm.o threefish512_enc.o threefish1024_enc.o\
                         threefish_mix.o threefish_mix_4c.o
$(ALGO_NAME)_TEST_BIN := main-threefish-test.o debug.o uart.o hexdigit_tab.o  \
                         nessie_bc_test.o dbz_strings.o nessie_common.o cli.o string-extras.o performance_test.o
$(ALGO_NAME)_NESSIE_TEST      := test nessie
$(ALGO_NAME)_PERFORMANCE_TEST := performance

