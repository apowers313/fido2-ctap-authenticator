INCLS=-Isrc/cbor/include -I/opt/local/include
LIBPATH=-Lsrc/cbor/lib
LIBS=-lcbor -lcrypto

.PHONY: lib/ctap-auth.dylib
all: lib/ctap-auth.dylib

lib/ctap-auth.dylib: src/ctap-auth.c src/cbor/include/cbor.h Makefile
	gcc -dynamiclib -fPIC -o lib/ctap-auth.dylib $(INCLS) $(LIBPATH) $(LIBS) src/ctap-auth.c
	install_name_tool -change libcbor.0.dylib src/cbor/lib/libcbor.0.dylib lib/ctap-auth.dylib

src/cbor/include/cbor.h: Makefile
	cd src/libcbor && \
	cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=../cbor . && \
	make cbor cbor_shared install
