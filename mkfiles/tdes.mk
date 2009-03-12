# Makefile for DES
ALGO_NAME := TDES

# comment out the following line for removement of DES from the build process
BLOCK_CIPHERS += $(ALGO_NAME)

$(ALGO_NAME)_OBJ      := des.o
$(ALGO_NAME)_TEST_BIN := main-tdes-test.o debug.o uart.o hexdigit_tab.o  \
                         nessie_bc_test.o dbz_strings.o nessie_common.o cli.o string-extras.o performance_test.o
$(ALGO_NAME)_NESSIE_TEST      := "nessie"
$(ALGO_NAME)_PERFORMANCE_TEST := "performance"

