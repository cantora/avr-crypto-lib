# Makefile for noekeon
ALGO_NAME := UBI_C

# comment out the following line for removement of threefish from the build process
BLOCK_CIPHERS += $(ALGO_NAME)


$(ALGO_NAME)_OBJ      := threefish256_enc.o threefish512_enc.o threefish1024_enc.o \
                         ubi256.o ubi512.o ubi1024.o memxor.o
$(ALGO_NAME)_TEST_BIN := main-ubi-test.o debug.o uart.o hexdigit_tab.o  \
                         dbz_strings.o nessie_common.o cli.o string-extras.o performance_test.o
$(ALGO_NAME)_NESSIE_TEST      := test nessie
$(ALGO_NAME)_PERFORMANCE_TEST := performance

