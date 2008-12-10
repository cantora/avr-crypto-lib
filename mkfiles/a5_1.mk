# Makefile for ARCFOUR (RC4 compatible)
ALGO_NAME := A51

# comment out the following line for removement of ARCFOUR from the build process
STREAM_CIPHERS += $(ALGO_NAME)

$(ALGO_NAME)_OBJ      := A5_1.o
$(ALGO_NAME)_TEST_BIN := main-a5_1-test.o debug.o uart.o serial-tools.o \
                         nessie_stream_test.o nessie_common.o
$(ALGO_NAME)_NESSIE_TEST      := "nessie"
$(ALGO_NAME)_PERFORMANCE_TEST := "performance"

