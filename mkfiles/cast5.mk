# Makefile for CAST5
ALGO_NAME := CAST5

# comment out the following line for removement of CAST5 from the build process
BLOCK_CIPHERS += $(ALGO_NAME)

$(ALGO_NAME)_OBJ      := cast5.o
$(ALGO_NAME)_TEST_BIN := main-cast5-test.o debug.o uart.o hexdigit_tab.o  cli.o string-extras.o\
                         nessie_bc_test.o dbz_strings.o nessie_common.o performance_test.o
$(ALGO_NAME)_NESSIE_TEST      := "nessie"
$(ALGO_NAME)_PERFORMANCE_TEST := "performance"

