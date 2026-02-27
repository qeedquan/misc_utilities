#!/bin/sh

# generate private key
openssl genrsa -out priv.pem 4096

# private key can be then used to derive a public key
openssl rsa -in priv.pem -pubout > pub.pem
