# Makefile for UBI
ALGO_NAME := UBI

# comment out the following line for removement of ubi from the build process
HASHES += $(ALGO_NAME)


$(ALGO_NAME)_OBJ      := threefish_mix.o threefish256_enc_asm.o ubi256_asm.o threefish512_enc.o threefish1024_enc.o \
                         threefish_mix_4c.o ubi512.o ubi1024.o memxor.o
$(ALGO_NAME)_TEST_BIN := main-ubi-test.o debug.o uart.o hexdigit_tab.o  \
                         dbz_strings.o nessie_common.o cli.o string-extras.o performance_test.o
$(ALGO_NAME)_NESSIE_TEST      := test nessie
$(ALGO_NAME)_PERFORMANCE_TEST := performance

