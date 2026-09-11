#!/bin/sh

# seed random data
echo "entropy" | tpm2_stirrandom

dd if=/dev/urandom bs=1 count=64 > myrandom.bin
tpm2_stirrandom < ./myrandom.bin

# get some random bytes as binary data
tpm2_getrandom 64

# get some random bytes as hex
tpm2_getrandom 20 --hex
