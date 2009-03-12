# Makefile for TWISTER-224
ALGO_NAME := TWISTER224_C

# comment out the following line for removement of TWISTER-224 from the build process
HASHES += $(ALGO_NAME)

$(ALGO_NAME)_OBJ      := twister.o twister-small.o memxor.o gf256mul.o
$(ALGO_NAME)_TEST_BIN := main-twister224-test.o debug.o uart.o hexdigit_tab.o  \
                         nessie_hash_test.o dbz_strings.o nessie_common.o cli.o string-extras.o performance_test.o
$(ALGO_NAME)_NESSIE_TEST      := "nessie"
$(ALGO_NAME)_PERFORMANCE_TEST := "performance"

