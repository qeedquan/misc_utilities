#!/bin/sh

# run test-tss program that will populate the TPM with some data
# then we can use these standard tpm2 command line tools to validate it

# to wipe the data, use tpm2_clear
tpm2_readpublic -c 0x81000000
tpm2_unseal -c 0x81000001
echo
