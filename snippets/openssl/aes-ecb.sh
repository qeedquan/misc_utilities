#!/bin/sh

dd if=/dev/urandom of=key.bin bs=1 count=16
KEY=$(xxd -p key.bin)
echo "key=$KEY" > key.txt

echo "secrets!!!!!!!!" > secrets.txt
openssl aes-128-ecb -K $KEY -nopad -in secrets.txt -out secrets.enc
openssl aes-128-ecb -d -K $KEY -nopad -in secrets.enc -out secrets.dec
sha1sum secrets.dec secrets.txt
