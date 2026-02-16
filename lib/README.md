# static lib sources

## x86_64:
<!-- todo: fill in the x86_64 sources -->

- [headers/openssl](https://github.com/djp952/prebuilt-libssl/tree/master/linux-x86_64)
- [headers/libxml](https://dl-cdn.alpinelinux.org/v3.18/main/x86_64/libxml2-dev-2.11.8-r3.apk)
- [headers/libftdi1](https://dl-cdn.alpinelinux.org/v3.18/community/x86_64/libftdi1-dev-1.5-r1.apk)

### sha256sum:
<!-- obtained with `for file in "$(find ./lib/x86_64 -type f -name '*.a')"; do sha256sum $file; done` -->

| Library | SHA256 |
|---------|--------|
| libcrypto.a | `057679f1af757bdbf215297f01516b03f639bae9275bed72a748973725cb0d48` |
| libssl.a | `f90ffa440285865fff833f3a035c39dbcac7e60ba03c3f530ad6ce0de9bac756` |
| libelf.a | `1b92f34510e1370c1832ad3ff6b68689193584bdd900d788c69de1bde737110b` |
| liblzma.a | `9fab5d629a12a3d79602d79c07d5c6f3b6f6924ff05b7156353b521d25c13385` |
| libusb-1.0.a | `01e922ca8f1d241e183f9477760468812327df3c7e12e739d9507679c1556728` |
| libxml-2.0.a | `70a859e0cf88c7e0214a584b233a6782f6aa1dd46602f2df9837a2f1d3fefbf3` |
| libz.a | `f81da8056ea0ee821570a6289f2f77b65de24756df2ad68730295ad818266579` |
| libprotobuf-lite.a | `49a83331ec9e1119c508b3bbabe2b420864029304e4d475e8062ca4bc6a39413` |
| libtss2-mu.a | `526ac75b81890376d6185e0960dc2b1bb09bd52facf1d0505560133710d9a144` |
| libtss2-esys.a | `594b6751b80a6a43c9e8b66480d654bedb628ee5c7b06d088d8e28f1f7849474` |
| libtss2-rc.a | `d1b23db3d623472869acee7ccf8b7263aa0716b1d2f874274e3b458c1845bd44` |
| libtss2-sys.a | `b3064cca75f5768b5885f01f3eef05837d0d2168f050a02f514ac56741648d42` |
| libftdi1.a | `a3a3d113b8d3dc38594299ce6ac733995566cda0347b8e340893b38d3194f437` |

## aarch64:
- [libcrypto.a](https://github.com/djp952/prebuilt-libssl/tree/master/linux-aarch64)
- [libssl.a](https://github.com/djp952/prebuilt-libssl/tree/master/linux-aarch64)
- [libelf.a](https://dl-cdn.alpinelinux.org/v3.18/main/aarch64/libelf-static-0.189-r2.apk)
- [liblzma.a](https://dl-cdn.alpinelinux.org/v3.18/main/aarch64/xz-static-5.4.3-r1.apk)
- [libusb-1.0.a](https://dl-cdn.alpinelinux.org/v3.18/main/aarch64/libusb-dev-1.0.26-r2.apk)
- [libxml-2.0.a](https://dl-cdn.alpinelinux.org/v3.18/main/aarch64/libxml2-static-2.11.8-r3.apk)
- [libz.a](https://dl-cdn.alpinelinux.org/v3.18/main/aarch64/zlib-static-1.2.13-r1.apk)
- [libprotobuf-lite.a](https://github.com/Tencent/Hardcoder/blob/master/libapp2sys/prebuilt/arm64-v8a/libprotobuf-lite.a)
- [libtss2-*.a](https://dl-cdn.alpinelinux.org/v3.18/community/aarch64/tpm2-tss-static-4.0.1-r0.apk)
- [libftdi1.a](https://dl-cdn.alpinelinux.org/v3.18/community/aarch64/libftdi1-static-1.5-r1.apk)

- [headers/openssl](https://github.com/djp952/prebuilt-libssl/tree/master/linux-aarch64)
- [headers/libxml](https://dl-cdn.alpinelinux.org/v3.18/main/aarch64/libxml2-dev-2.11.8-r3.apk)
- [headers/libftdi1](https://dl-cdn.alpinelinux.org/v3.18/community/aarch64/libftdi1-dev-1.5-r1.apk)

### sha256sum:
<!-- obtained with `for file in "$(find ./lib/aarch64/ -type f -name '*.a')"; do sha256sum $file; done` -->

| Library | SHA256 |
|---------|--------|
| libcrypto.a | `2cfec342446d56111909ef9e2b5bcbe88c35ce9a6059aa8d52332db781bc219c` |
| libssl.a | `72d1eae3ba3f4e634f9806abb53f98ea9d921ef3cc6d40cdb59e0d508af6e8cc` |
| libelf.a | `5e19d478adf9e9f30256cf31d6ffbae471015cb00d1a6a8ecd1c6ecfbcd23539` |
| liblzma.a | `e0f824f904046087b4e824e27e66a6127bdc2cc9ba4f4f8e1c4642a834206a14` |
| libusb-1.0.a | `43eb30451aca3fa173bc23d479943b173e27cb3ca036470c79e98a50b2b72651` |
| libxml-2.0.a | `738aaabb166ec160e6c1f5a97a4e2fd22115243b0ebb02f64ada6c7ace1dd018` |
| libz.a | `9e574911bdad38dd9df881f620c130e51783659ca979f7db38c6147dfbd1797a` |
| libprotobuf-lite.a | `7bb1711ade56f05a58c44f7b5b577cc3185c669c01f272f8d509d134526bfd48` |
| libtss2-mu.a | `5de01d3e7f5d07993657b676561ec5fdb3fa26bcf1912da9404db287bab2a8a6` |
| libtss2-esys.a | `3eead2c8ae60952a7bff9f4a3861475b98a18c4c9a7e53a1804c5c861e8bd3a1` |
| libtss2-rc.a | `35727b4251bf556398ec100ffec973ea2379b3596e8e0acbb99c320bf528ee5f` |
| libtss2-sys.a | `ce26b7a09f4b3ec717d080200052f6948a15255b47b3624f82d62df0d214c275` |
| libftdi1.a | `97168204bb12098648a6de92d1858cec524f56e2835c219f5aedb0bd09c5b0bc` |