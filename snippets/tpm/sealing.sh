#!/bin/sh

# tests sealing
# where we save a snapshot of the pcr values, make a policy based on it
# and seal it against the pcr values, the seal context can't be unsealed without the right pcr values

# there are alot of flushcontext calls since this assumes we're not running a daemon that handles slot management

tpm2_flushcontext -t
tpm2_flushcontext -l
tpm2_createprimary -c primary.ctx -Q

tpm2_pcrread -Q -o pcr.bin sha256:0,1,2,3

tpm2_flushcontext -t
tpm2_flushcontext -l
tpm2_createpolicy -Q --policy-pcr -l sha256:0,1,2,3 -f pcr.bin -L pcr.policy

echo 'secret' | tpm2_create -C primary.ctx -L pcr.policy -i - -u seal.pub -r seal.priv -c seal.ctx -Q

tpm2_unseal -c seal.ctx -p pcr:sha256:0,1,2,3
tpm2_flushcontext -t

