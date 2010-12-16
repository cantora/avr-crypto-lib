# Makefile for Grøstl
ALGO_NAME := JH_SIMPLE_SPEED_C

# comment out the following line for removement of Grøstl from the build process
HASHES += $(ALGO_NAME)

$(ALGO_NAME)_DIR      := jh/
$(ALGO_NAME)_INCDIR   := hfal/ memxor/
$(ALGO_NAME)_OBJ      := jh_simple_speed.o jh_tables.o memxor.o 
$(ALGO_NAME)_TEST_BIN := hfal_jh.o main-jh-test.o $(CLI_STD) $(HFAL_STD)
$(ALGO_NAME)_NESSIE_TEST      := test nessie
$(ALGO_NAME)_PERFORMANCE_TEST := performance

