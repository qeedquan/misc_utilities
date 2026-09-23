#!/bin/sh

set -x

# With the password set all the TPM commands that define objects/nvindices/etc (tpm2_createprimary, tpm2_nvdefine, etc)
# needs a password authentication to use otherwise the command will fail
test_password() {
	# Set the password on the owner hierarchy
	tpm2_changeauth -c owner mypass

	# Update the password
	tpm2_changeauth -c o -p mypass mynewpass

	# Rechange it to be an empty password
	tpm2_changeauth -c o -p mynewpass

	# Rechange the password
	tpm2_changeauth -c owner onemorepass

	# If the password is not known, we can use clear to reset the password
	tpm2_clear -c platform
}

test_password
