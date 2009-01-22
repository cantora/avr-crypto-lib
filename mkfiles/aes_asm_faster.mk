# Makefile for AES
ALGO_NAME := AES_FASTER

# comment out the following line for removement of AES from the build process
BLOCK_CIPHERS += $(ALGO_NAME)


$(ALGO_NAME)_OBJ      := aes_enc-asm.o aes_dec-asm_faster.o aes_sboxes-asm.o  \
                         aes_keyschedule-asm.o  
$(ALGO_NAME)_TEST_BIN := main-aes-test.o debug.o uart.o serial-tools.o \
                         nessie_bc_test.o nessie_common.o cli.o performance_test.o
$(ALGO_NAME)_NESSIE_TEST      := test nessie
$(ALGO_NAME)_PERFORMANCE_TEST := performance

