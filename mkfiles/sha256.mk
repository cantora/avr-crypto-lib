# Makefile for SHA256
ALGO_NAME := SHA256

# comment out the following line for removement of SHA256 from the build process
HASHES += $(ALGO_NAME)

$(ALGO_NAME)_OBJ      := sha256-asm.o
$(ALGO_NAME)_TEST_BIN := main-sha256-test.o debug.o uart.o serial-tools.o \
                         nessie_hash_test.o nessie_common.o cli.o string-extras.o performance_test.o \
			 hfal-basic.o hfal_sha256.o shavs.o dump.o
$(ALGO_NAME)_NESSIE_TEST      := "nessie"
$(ALGO_NAME)_PERFORMANCE_TEST := "performance"

