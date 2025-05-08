#!/bin/sh

# we can load an external AES key, this bypasses the key hierarchy and allows the TPM to be used as a regular engine to encrypt/decrypt
dd if=/dev/urandom of=key.bin bs=1 count=16
tpm2_loadexternal -C n -G aes128cfb -r key.bin -c key.ctx

echo "secret data!!!!" > secret.dat
tpm2_encryptdecrypt -c key.ctx -o secret.enc secret.dat
tpm2_encryptdecrypt -d -c key.ctx -o secret.dec secret.enc

tpm2_flushcontext -t

# test if we can decrypt it
openssl enc -aes-128-cfb -K $(xxd -p key.bin) -nosalt -nopad -iv "00000000000000000000000000000000" -in secret.enc -out secret_openssl.dec

