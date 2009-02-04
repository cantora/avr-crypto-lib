# Makefile for Grain
ALGO_NAME := GRAIN

# comment out the following line for removement of Grain from the build process
STREAM_CIPHERS += $(ALGO_NAME)

$(ALGO_NAME)_OBJ      := grain.o
$(ALGO_NAME)_TEST_BIN := main-grain-test.o debug.o uart.o serial-tools.o \
                         nessie_stream_test.o nessie_common.o cli.o string-extras.o \
			             performance_test.o
$(ALGO_NAME)_NESSIE_TEST      := "nessie"
$(ALGO_NAME)_PERFORMANCE_TEST := "performance"

