# Makefile for TWISTER-384
ALGO_NAME := TWISTER384

# comment out the following line for removement of TWISTER-384 from the build process
HASHES += $(ALGO_NAME)

$(ALGO_NAME)_OBJ      := twister-asm.o twister-big-asm.o twister384.o
$(ALGO_NAME)_TEST_BIN := main-twister384-test.o debug.o uart.o hexdigit_tab.o  \
                         nessie_hash_test.o dbz_strings.o nessie_common.o cli.o string-extras.o performance_test.o
$(ALGO_NAME)_NESSIE_TEST      := "nessie"
$(ALGO_NAME)_PERFORMANCE_TEST := "performance"

