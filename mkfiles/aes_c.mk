# Makefile for AES
ALGO_NAME := AES_C

# comment out the following line for removement of AES from the build process
BLOCK_CIPHERS += $(ALGO_NAME)


$(ALGO_NAME)_OBJ      := aes_enc.o aes_dec.o aes_sbox.o aes_invsbox.o \
                         aes_keyschedule.o gf256mul.o \
                         aes128_enc.o aes128_dec.o aes192_enc.o aes192_dec.o \
                         aes256_enc.o aes256_dec.o
$(ALGO_NAME)_TEST_BIN := main-aes-test.o debug.o uart.o serial-tools.o \
                         nessie_bc_test.o nessie_common.o cli.o string-extras.o performance_test.o
$(ALGO_NAME)_NESSIE_TEST      := test nessie
$(ALGO_NAME)_PERFORMANCE_TEST := performance

