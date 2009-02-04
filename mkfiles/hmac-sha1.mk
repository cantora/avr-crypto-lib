# Makefile for HMAC-SHA256
ALGO_NAME := HMAC-SHA1

# comment out the following line for removement of HMAC-SHA256 from the build process
MACS += $(ALGO_NAME)

$(ALGO_NAME)_OBJ      := hmac-sha1.o sha1-asm.o
$(ALGO_NAME)_TEST_BIN := main-hmac-sha1-test.o debug.o uart.o serial-tools.o cli.o string-extras.o \
                         nessie_mac_test.o nessie_common.o
$(ALGO_NAME)_NESSIE_TEST      := "nessie"
$(ALGO_NAME)_PERFORMANCE_TEST := "performance"

