#!/bin/sh

# https://github.com/stefanberger/swtpm/wiki/Using-the-Intel-TSS-with-swtpm
# use a software emulator to make a virtual TPM device that other tools uses

# set this environment variable for other TPM tools to use the virtual device
# export TPM2TOOLS_TCTI="swtpm:port=2321"

# on startup run tpm2_startup -c to clear the TPM state for usage (startup-clear option makes this optional)

# if allocated too much objects with tpm2_createprimary, reset the TPM with
# swtpm_ioctl -i --tcp :2322

# directory where all the TPM state is stored
DIR=swtpm_state

swtpm_serve_socket() {
	swtpm socket --tpmstate dir=$DIR --tpm2 --ctrl type=tcp,port=2322 --server type=tcp,port=2321 --flags not-need-init,startup-clear --log level=20
}

swtpm_serve_device() {
	sudo swtpm chardev --vtpm-proxy --tpmstate dir=$DIR --tpm2 --ctrl type=tcp,port=2322 --flags not-need-init,startup-clear --log level=20
}

mkdir -p $DIR 2>/dev/null
swtpm_serve_socket
#swtpm_serve_device
