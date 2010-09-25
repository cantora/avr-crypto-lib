# Makefile for RSA
ALGO_NAME := RSA

# comment out the following line for removement of RSA from the build process
PK_CIPHERS += $(ALGO_NAME)

$(ALGO_NAME)_DIR      := rsa/
$(ALGO_NAME)_INCDIR   := bigint/ noekeon/ memxor/
$(ALGO_NAME)_OBJ      := bigint.o bigint_io.o bigint_add_u.o rsa.o rsa_key_blob.o
$(ALGO_NAME)_TEST_BIN := main-rsa-test.o $(CLI_STD)  \
                         noekeon_asm.o noekeon_prng.o memxor.o 
			 
$(ALGO_NAME)_PERFORMANCE_TEST := performance

