# Makefile for RC5
ALGO_NAME := RC5

# comment out the following line for removement of RC5 from the build process
BLOCK_CIPHERS += $(ALGO_NAME)


$(ALGO_NAME)_OBJ      := rc5.o
$(ALGO_NAME)_TEST_BIN := main-rc5-test.o debug.o uart.o hexdigit_tab.o serial-tools.o \
                         nessie_bc_test.o \
                         dbz_strings.o nessie_common.o cli.o string-extras.o performance_test.o
$(ALGO_NAME)_NESSIE_TEST      := test nessie
$(ALGO_NAME)_PERFORMANCE_TEST := performance

