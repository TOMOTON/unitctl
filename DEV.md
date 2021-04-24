
## With libcurl

```
	linux-vdso.so.1 (0x00007ffef7ff9000)
	libcurl.so.4 => /usr/lib/x86_64-linux-gnu/libcurl.so.4 (0x00007f8effe52000)
	libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007f8effab3000)
	libnghttp2.so.14 => /usr/lib/x86_64-linux-gnu/libnghttp2.so.14 (0x00007f8eff88d000)
	libidn2.so.0 => /usr/lib/x86_64-linux-gnu/libidn2.so.0 (0x00007f8eff66b000)
	librtmp.so.1 => /usr/lib/x86_64-linux-gnu/librtmp.so.1 (0x00007f8eff44e000)
	libssh2.so.1 => /usr/lib/x86_64-linux-gnu/libssh2.so.1 (0x00007f8eff221000)
	libpsl.so.5 => /usr/lib/x86_64-linux-gnu/libpsl.so.5 (0x00007f8eff013000)
	libssl.so.1.0.2 => /usr/lib/x86_64-linux-gnu/libssl.so.1.0.2 (0x00007f8efedaa000)
	libcrypto.so.1.0.2 => /usr/lib/x86_64-linux-gnu/libcrypto.so.1.0.2 (0x00007f8efe943000)
	libgssapi_krb5.so.2 => /usr/lib/x86_64-linux-gnu/libgssapi_krb5.so.2 (0x00007f8efe6f8000)
	libkrb5.so.3 => /usr/lib/x86_64-linux-gnu/libkrb5.so.3 (0x00007f8efe41e000)
	libk5crypto.so.3 => /usr/lib/x86_64-linux-gnu/libk5crypto.so.3 (0x00007f8efe1eb000)
	libcom_err.so.2 => /lib/x86_64-linux-gnu/libcom_err.so.2 (0x00007f8efdfe7000)
	liblber-2.4.so.2 => /usr/lib/x86_64-linux-gnu/liblber-2.4.so.2 (0x00007f8efddd8000)
	libldap_r-2.4.so.2 => /usr/lib/x86_64-linux-gnu/libldap_r-2.4.so.2 (0x00007f8efdb87000)
	libz.so.1 => /lib/x86_64-linux-gnu/libz.so.1 (0x00007f8efd96d000)
	libpthread.so.0 => /lib/x86_64-linux-gnu/libpthread.so.0 (0x00007f8efd750000)
	/lib64/ld-linux-x86-64.so.2 (0x00007f8f002d9000)
	libunistring.so.0 => /usr/lib/x86_64-linux-gnu/libunistring.so.0 (0x00007f8efd439000)
	libgnutls.so.30 => /usr/lib/x86_64-linux-gnu/libgnutls.so.30 (0x00007f8efd0a0000)
	libhogweed.so.4 => /usr/lib/x86_64-linux-gnu/libhogweed.so.4 (0x00007f8efce6b000)
	libnettle.so.6 => /usr/lib/x86_64-linux-gnu/libnettle.so.6 (0x00007f8efcc34000)
	libgmp.so.10 => /usr/lib/x86_64-linux-gnu/libgmp.so.10 (0x00007f8efc9b1000)
	libgcrypt.so.20 => /lib/x86_64-linux-gnu/libgcrypt.so.20 (0x00007f8efc6a1000)
	libdl.so.2 => /lib/x86_64-linux-gnu/libdl.so.2 (0x00007f8efc49d000)
	libkrb5support.so.0 => /usr/lib/x86_64-linux-gnu/libkrb5support.so.0 (0x00007f8efc291000)
	libkeyutils.so.1 => /lib/x86_64-linux-gnu/libkeyutils.so.1 (0x00007f8efc08d000)
	libresolv.so.2 => /lib/x86_64-linux-gnu/libresolv.so.2 (0x00007f8efbe76000)
	libsasl2.so.2 => /usr/lib/x86_64-linux-gnu/libsasl2.so.2 (0x00007f8efbc5b000)
	libp11-kit.so.0 => /usr/lib/x86_64-linux-gnu/libp11-kit.so.0 (0x00007f8efb9f6000)
	libidn.so.11 => /lib/x86_64-linux-gnu/libidn.so.11 (0x00007f8efb7c2000)
	libtasn1.so.6 => /usr/lib/x86_64-linux-gnu/libtasn1.so.6 (0x00007f8efb5af000)
	libgpg-error.so.0 => /lib/x86_64-linux-gnu/libgpg-error.so.0 (0x00007f8efb39b000)
	libffi.so.6 => /usr/lib/x86_64-linux-gnu/libffi.so.6 (0x00007f8efb192000)
```

## Without libcurl

```
    linux-vdso.so.1 (0x00007ffcfd7b5000)
    libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007f3415290000)
    /lib64/ld-linux-x86-64.so.2 (0x00007f3415834000)
```

## Dynamic size

```
total 133
-rwxrwxrwx 1 vagrant vagrant      0 Apr 18 08:22 .gitkeep
-rwxrwxrwx 1 vagrant vagrant 135304 Apr 24 08:13 unitctl
```

## Static size

```
total 955
-rwxrwxrwx 1 vagrant vagrant      0 Apr 18 08:22 .gitkeep
-rwxrwxrwx 1 vagrant vagrant 977768 Apr 24 08:18 unitctl
```

Static has no added value: Unit already requires the same libraries:

```
    linux-vdso.so.1 (0x00007ffdfb3b5000)
    libm.so.6 => /lib/x86_64-linux-gnu/libm.so.6 (0x00007fd37f9c0000)
    librt.so.1 => /lib/x86_64-linux-gnu/librt.so.1 (0x00007fd37f7b8000)
    libdl.so.2 => /lib/x86_64-linux-gnu/libdl.so.2 (0x00007fd37f5b4000)
    libpthread.so.0 => /lib/x86_64-linux-gnu/libpthread.so.0 (0x00007fd37f397000)
    libssl.so.1.1 => /usr/lib/x86_64-linux-gnu/libssl.so.1.1 (0x00007fd37f12b000)
    libcrypto.so.1.1 => /usr/lib/x86_64-linux-gnu/libcrypto.so.1.1 (0x00007fd37ec90000)
    libpcre.so.3 => /lib/x86_64-linux-gnu/libpcre.so.3 (0x00007fd37ea1d000)
    libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007fd37e67e000)
    /lib64/ld-linux-x86-64.so.2 (0x00007fd37ff31000)
```