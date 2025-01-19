#!/bin/sh

tpm2_flushcontext -t
tpm2_createprimary -C e -c primary.ctx
tpm2_create -G rsa -u rsa.pub -r rsa.priv -C primary.ctx
tpm2_flushcontext -t
tpm2_load -C primary.ctx -u rsa.pub -r rsa.priv -c rsa.ctx
tpm2_flushcontext -t

# sign the message and verify that the original data works
echo "my message" > message.dat
tpm2_sign -c rsa.ctx -g sha256 -o sig.rssa message.dat
tpm2_verifysignature -c rsa.ctx -g sha256 -s sig.rssa -m message.dat

# tamper the message, should fail now
echo foo >> message.dat
tpm2_verifysignature -c rsa.ctx -g sha256 -s sig.rssa -m message.dat
