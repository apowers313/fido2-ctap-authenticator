all:
	gcc -dynamiclib -fPIC -o lib/ctap-auth.dylib src/ctap-auth.c 