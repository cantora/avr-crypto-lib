# Makefile for ECDSA
ALGO_NAME := ECDSA

# comment out the following line for removement of ECDSA from the build process
SIGNATURE += $(ALGO_NAME)

$(ALGO_NAME)_DIR      := ecdsa/
$(ALGO_NAME)_INCDIR   := memxor/ bigint/ sha1/ sha2/ noekeon/ base64/ hfal/
$(ALGO_NAME)_OBJ      := bigint.o bigint_io.o nist_p192.o ecc_chudnovsky.o ecdsa_sign.o base64_enc.o \
	                 sha2_small_common.o sha2_large_common.o sha224.o sha256.o sha384.o sha512.o \
			 sha1-asm.o
$(ALGO_NAME)_TESTBIN  := main-ecdsa-test.o $(CLI_STD) $(HFAL_STD) \
                         noekeon_asm.o noekeon_prng.o memxor.o \
			 hfal_sha1.o hfal_sha224.o hfal_sha256.o hfal_sha384.o hfal_sha512.o
			 
$(ALGO_NAME)_PERFORMANCE_TEST := performance

