# Makefile for BigInt
ALGO_NAME := BIGINT

# comment out the following line for removement of base64 from the build process
ENCODINGS += $(ALGO_NAME)

$(ALGO_NAME)_DIR      := bigint/
$(ALGO_NAME)_OBJ      := bigint.o bigint_io.o bigint_add_u.o 
$(ALGO_NAME)_TEST_BIN := main-bigint-test.o $(CLI_STD)  \
                         performance_test.o noekeon_asm.o noekeon_prng.o memxor.o
			 
$(ALGO_NAME)_PERFORMANCE_TEST := performance

