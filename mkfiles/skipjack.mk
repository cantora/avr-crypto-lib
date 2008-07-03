# Makefile for SKIPJACK
ALGO_NAME := SKIPJACK

# comment out the following line for removement of skipjack from the build process
BLOCK_CIPHERS += $(ALGO_NAME)

$(ALGO_NAME)_OBJ      := skipjack.o
$(ALGO_NAME)_TEST_BIN := main-skipjack-test.o debug.o uart.o serial-tools.o \
                         skipjack.o
$(ALGO_NAME)_NESSIE_TEST      := "nessie"
$(ALGO_NAME)_PEROFRMANCE_TEST := "performance"

