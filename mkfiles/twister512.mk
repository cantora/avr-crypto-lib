# Makefile for TWISTER-512
ALGO_NAME := TWISTER512

# comment out the following line for removement of TWISTER-512 from the build process
HASHES += $(ALGO_NAME)

$(ALGO_NAME)_OBJ      := twister-asm.o twister-big-asm.o twister512.o
$(ALGO_NAME)_TEST_BIN := main-twister512-test.o debug.o uart.o hexdigit_tab.o  \
                         nessie_hash_test.o dbz_strings.o nessie_common.o cli.o string-extras.o performance_test.o
$(ALGO_NAME)_NESSIE_TEST      := "nessie"
$(ALGO_NAME)_PERFORMANCE_TEST := "performance"

