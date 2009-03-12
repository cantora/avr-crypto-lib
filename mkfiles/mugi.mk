# Makefile for MUGI
ALGO_NAME := MUGI_C

# comment out the following line for removement of MUGI_C from the build process
STREAM_CIPHERS += $(ALGO_NAME)

$(ALGO_NAME)_OBJ      := mugi.o gf256mul.o aes_sbox.o
$(ALGO_NAME)_TEST_BIN := main-mugi-test.o debug.o uart.o hexdigit_tab.o  \
                         nessie_stream_test.o dbz_strings.o nessie_common.o cli.o string-extras.o \
                         performance_test.o
$(ALGO_NAME)_NESSIE_TEST      := "nessie"
$(ALGO_NAME)_PERFORMANCE_TEST := "performance"

