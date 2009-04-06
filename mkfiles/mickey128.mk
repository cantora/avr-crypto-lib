# Makefile for Mickey128
ALGO_NAME := MICKEY128

# comment out the following line for removement of Mickey128 from the build process
STREAM_CIPHERS += $(ALGO_NAME)

$(ALGO_NAME)_OBJ      := mickey128.o
$(ALGO_NAME)_TEST_BIN := main-mickey128-test.o debug.o uart.o hexdigit_tab.o serial-tools.o \
                         nessie_stream_test.o nessie_common.o cli.o string-extras.o
$(ALGO_NAME)_NESSIE_TEST      := "nessie"
$(ALGO_NAME)_PERFORMANCE_TEST := "performance"

