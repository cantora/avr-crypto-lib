# Makefile for SHA1
ALGO_NAME := SHA1

# comment out the following line for removement of SHA1 from the build process
HASHES += $(ALGO_NAME)

$(ALGO_NAME)_OBJ      := sha1-asm.o
$(ALGO_NAME)_TEST_BIN := main-sha1-test.o debug.o uart.o serial-tools.o \
                         sha1-asm.o nessie_hash_test.o nessie_common.o cli.o 
$(ALGO_NAME)_NESSIE_TEST      := "nessie"
$(ALGO_NAME)_PEROFRMANCE_TEST := "performance"

