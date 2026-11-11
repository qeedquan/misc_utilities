#!/bin/sh

# Runs a software TPM emulator exposed as a UNIX socket
swtpm socket --tpm2 \
  --tpmstate dir=swtpm \
  --ctrl type=unixio,path=swtpm/swtpm-sock \
  --flags not-need-init,startup-clear
