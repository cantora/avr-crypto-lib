# Makefile for ARCFOUR (RC4 compatible)
ALGO_NAME := ARCFOUR

# comment out the following line for removement of ARCFOUR from the build process
STREAM_CIPHERS += $(ALGO_NAME)

$(ALGO_NAME)_OBJ      := arcfour-asm.o
$(ALGO_NAME)_TEST_BIN := main-arcfour-test.o debug.o uart.o serial-tools.o \
                         nessie_stream_test.o nessie_common.o cli.o \
                         performance_test.o
$(ALGO_NAME)_NESSIE_TEST      := "nessie"
$(ALGO_NAME)_PEROFRMANCE_TEST := "performance"

