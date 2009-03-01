# Makefile for AES
ALGO_NAME := AES

# comment out the following line for removement of AES from the build process
BLOCK_CIPHERS += $(ALGO_NAME)


$(ALGO_NAME)_OBJ      := aes_enc-asm.o aes_dec-asm.o aes_sbox-asm.o aes_invsbox-asm.o  \
                         aes_keyschedule-asm.o  
$(ALGO_NAME)_TEST_BIN := main-aes-test.o debug.o uart.o hexdigit_tab.o serial-tools.o \
                         nessie_bc_test.o dbz_strings.o nessie_common.o cli.o string-extras.o performance_test.o
$(ALGO_NAME)_NESSIE_TEST      := test nessie
$(ALGO_NAME)_PERFORMANCE_TEST := performance

