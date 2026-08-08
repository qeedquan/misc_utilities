#!/bin/sh

# this prompts for a password to use to encrypt/decrypt the file

echo hello > secrets.txt
openssl aes-128-cfb -a -salt -pbkdf2 -in secrets.txt -out secrets.enc
openssl aes-128-cfb -d -a -pbkdf2 -in secrets.enc -out secrets.dec
