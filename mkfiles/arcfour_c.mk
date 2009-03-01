# Makefile for ARCFOUR (RC4 compatible)
ALGO_NAME := ARCFOUR_C

# comment out the following line for removement of ARCFOUR from the build process
STREAM_CIPHERS += $(ALGO_NAME)

$(ALGO_NAME)_OBJ      := arcfour.o
$(ALGO_NAME)_TEST_BIN := main-arcfour-test.o debug.o uart.o hexdigit_tab.o serial-tools.o \
                         nessie_stream_test.o dbz_strings.o nessie_common.o cli.o string-extras.o \
                         performance_test.o
$(ALGO_NAME)_NESSIE_TEST      := "nessie"
$(ALGO_NAME)_PERFORMANCE_TEST := "performance"

