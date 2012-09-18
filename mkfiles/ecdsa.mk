# Makefile for ECDSA
ALGO_NAME := ECDSA

# comment out the following line for removement of ECDSA from the build process
SIGNATURE += $(ALGO_NAME)

$(ALGO_NAME)_DIR      := ecdsa/
$(ALGO_NAME)_INCDIR   := memxor/ bigint/ sha1/ noekeon/ base64/ hfal/
$(ALGO_NAME)_OBJ      := bigint.o bigint_io.o sha1-asm.o nist_p192.o base64_enc.o
$(ALGO_NAME)_TESTBIN  := main-ecdsa-test.o $(CLI_STD) hfal_sha1.o $(HFAL_STD) \
                         noekeon_asm.o noekeon_prng.o memxor.o 
			 
$(ALGO_NAME)_PERFORMANCE_TEST := performance

