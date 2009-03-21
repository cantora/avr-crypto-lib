# Makefile for Skein
ALGO_NAME := SKEIN

# comment out the following line for removement of Skein from the build process
HASHES += $(ALGO_NAME)


$(ALGO_NAME)_OBJ      := threefish_mix.o threefish256_enc_asm.o ubi256_asm.o skein256_asm.o \
                         threefish_mix_4c.o threefish512_enc.o threefish1024_enc.o \
                         ubi512.o ubi1024.o memxor.o skein512.o skein1024.o
$(ALGO_NAME)_TEST_BIN := main-skein-test.o debug.o uart.o hexdigit_tab.o  \
                         dbz_strings.o nessie_common.o cli.o string-extras.o performance_test.o \
                         hfal-basic.o hfal_skein256.o hfal_skein512.o hfal_skein1024.o shavs.o
$(ALGO_NAME)_NESSIE_TEST      := test nessie
$(ALGO_NAME)_PERFORMANCE_TEST := performance

