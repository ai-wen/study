#!/bin/bash

cd ./../lib/openssl-1.1.1d

#AES、MD5、RSA、SHA no-sm3 no-idea no-sm4 no-ec no-dsa no-sm2
if [ $(getconf WORD_BIT) = '32' ] && [ $(getconf LONG_BIT) = '64' ] ; then
	./config no-asm no-aria no-async no-blake2 no-cast no-chacha no-cms no-comp no-ct no-dso no-engine no-err no-md2 no-md4 no-mdc2 no-poly1305 no-rc2 no-rc4 no-rc5 no-ripemd no-seed  no-ts no-srp no-ui  no-siphash no-bf no-camellia no-ocsp  no-cmac  no-hw no-pic no-egd no-zlib  no-sse2  no-rfc3779 no-ssl no-ssl2 no-ssl3 no-tls no-unit-test no-tests no-sock  --prefix=./../lib/linux/x64		
else
	./config no-asm no-aria no-async no-blake2 no-cast no-chacha no-cms no-comp no-ct no-dso no-engine no-err no-md2 no-md4 no-mdc2 no-poly1305 no-rc2 no-rc4 no-rc5 no-ripemd no-seed  no-ts no-srp no-ui  no-siphash no-bf no-camellia no-ocsp  no-cmac  no-hw no-pic no-egd no-zlib  no-sse2  no-rfc3779 no-ssl no-ssl2 no-ssl3 no-tls no-unit-test no-tests no-sock  --prefix=./../lib/linux/x86	
fi

make build_libs
make install
