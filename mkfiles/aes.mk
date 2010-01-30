# Makefile for AES
ALGO_NAME := AES

# comment out the following line for removement of AES from the build process
BLOCK_CIPHERS += $(ALGO_NAME)

$(ALGO_NAME)_DIR      := aes/
$(ALGO_NAME)_OBJ      := aes_enc-asm.o aes_dec-asm.o aes_sbox-asm.o aes_invsbox-asm.o  \
                         aes_keyschedule-asm.o 
$(ALGO_NAME)_TEST_BIN := main-aes-test.o $(CLI_STD)  \
                         nessie_bc_test.o nessie_common.o performance_test.o memxor.o \
                         bcal_aes128.o bcal_aes192.o bcal_aes256.o bcal-basic.o bcal-cbc.o \
                         keysize_descriptor.o dump-asm.o dump-decl.o
$(ALGO_NAME)_NESSIE_TEST      := test nessie
$(ALGO_NAME)_PERFORMANCE_TEST := performance

