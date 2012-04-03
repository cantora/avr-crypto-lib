# Makefile for RSA
ALGO_NAME := RSA_PKCS15

# comment out the following line for removement of RSA from the build process
SIGNATURE += $(ALGO_NAME)

$(ALGO_NAME)_DIR      := rsa/
$(ALGO_NAME)_INCDIR   := memxor/ bigint/ noekeon/
$(ALGO_NAME)_OBJ      := bigint.o bigint_io.o rsa_basic.o rsa_pkcs15.o 
$(ALGO_NAME)_TESTBIN := main-rsa_pkcs15-test.o $(CLI_STD) random_dummy.o \
                         noekeon.o noekeon_prng.o memxor.o 
			 
$(ALGO_NAME)_PERFORMANCE_TEST := performance

