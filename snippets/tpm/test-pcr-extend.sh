#!/bin/sh

# To reset PCR values, need to reset the TPM in the BIOS
# For software simulator, it is cleared whenever the simulator closes

# Extend PCR 4's SHA1 bank with a hash
tpm2_pcrextend 4:sha1=f1d2d2f924e986ac86fdf7b36c94bcdf32beec15

# Extend PCR 4's SHA1 and SHA256 banks with hashes
tpm2_pcrextend 4:sha1=f1d2d2f924e986ac86fdf7b36c94bcdf32beec15,sha256=b5bb9d8014a0f9b1d61e21e796d78dccdf1352f23cd32812f4850b878ae4944c

# Extend PCR 4's SHA1 and PCR 7's SHA256 bank with hashes
tpm2_pcrextend 4:sha1=f1d2d2f924e986ac86fdf7b36c94bcdf32beec15 7:sha256=b5bb9d8014a0f9b1d61e21e796d78dccdf1352f23cd32812f4850b878ae4944c

# Read the PCR values back
tpm2_pcrread
