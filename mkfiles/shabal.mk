# Makefile for Skein
ALGO_NAME := SHABAL

# comment out the following line for removement of Skein from the build process
HASHES += $(ALGO_NAME)


$(ALGO_NAME)_OBJ      := shabal-asm.o shabal192-asm.o shabal224-asm.o \
                         shabal256-asm.o shabal384-asm.o shabal512-asm.o
$(ALGO_NAME)_TEST_BIN := main-shabal-test.o debug.o uart.o hexdigit_tab.o  \
                         dbz_strings.o nessie_common.o cli.o string-extras.o performance_test.o \
			 nessie_hash_test.o hfal-basic.o hfal_shabal.o shavs.o \
			 hfal-nessie.o hfal-performance.o hfal-test.o
$(ALGO_NAME)_NESSIE_TEST      := test nessie
$(ALGO_NAME)_PERFORMANCE_TEST := performance

