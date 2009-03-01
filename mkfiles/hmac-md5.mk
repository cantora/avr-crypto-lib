# Makefile for HMAC-SHA256
ALGO_NAME := HMAC-MD5

# comment out the following line for removement of HMAC-MD5 from the build process
MACS += $(ALGO_NAME)

$(ALGO_NAME)_OBJ      := hmac-md5.o md5-asm.o
$(ALGO_NAME)_TEST_BIN := main-hmac-md5-test.o debug.o uart.o hexdigit_tab.o serial-tools.o cli.o string-extras.o \
                         nessie_mac_test.o dbz_strings.o nessie_common.o base64_enc.o base64_dec.o
$(ALGO_NAME)_NESSIE_TEST      := "nessie"
$(ALGO_NAME)_PERFORMANCE_TEST := "performance"

