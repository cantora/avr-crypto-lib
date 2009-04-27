# Makefile for Skein
ALGO_NAME := SHABAL_C

# comment out the following line for removement of Skein from the build process
HASHES += $(ALGO_NAME)


$(ALGO_NAME)_OBJ      := shabal.o shabal192.o shabal224.o shabal256.o shabal384.o shabal512.o
$(ALGO_NAME)_TEST_BIN := main-shabal-test.o debug.o uart.o hexdigit_tab.o  \
                         dbz_strings.o nessie_common.o cli.o string-extras.o performance_test.o \
			 nessie_hash_test.o hfal-basic.o hfal_shabal.o shavs.o
$(ALGO_NAME)_NESSIE_TEST      := test nessie
$(ALGO_NAME)_PERFORMANCE_TEST := performance

