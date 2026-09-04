#!/bin/bash

set -x
set -e

# create a primary key
tpm2_createprimary -c primary.ctx
# use the primary key to generate children key
tpm2_create -C primary.ctx -Grsa2048 -u key.pub -r key.priv
# load object into the tpm
tpm2_load -C primary.ctx -u key.pub -r key.priv -c key.ctx

# perform encryption on the data
echo "secret data" > secret.dat
tpm2_rsaencrypt -c key.ctx -o secret.enc secret.dat
tpm2_rsadecrypt -c key.ctx -o secret.dec secret.enc
