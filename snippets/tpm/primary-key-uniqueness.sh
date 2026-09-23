#!/bin/sh

# demonstrates the primary key we generate are deterministic
# each tpm will have a unique seed for each key hierarchy that allows
# one to recreate the primary key without any extra data

# the context values will be based on the algorithms chosen however so that can change the values

# these context will be the same
tpm2_createprimary -C o -g sha256 -G ecc -c unique1.ctx
tpm2_createprimary -C o -g sha256 -G ecc -c unique2.ctx
tpm2_flushcontext -t

# these context will be the same
tpm2_createprimary -C o -g sha1 -G rsa -c unique3.ctx
tpm2_createprimary -C o -g sha1 -G rsa -c unique4.ctx
tpm2_flushcontext -t

# a checksum will show that the context file are different, that is due to the fact that there are
# other meta-data and internal naming of the keys are different, but the public key properties will be the same
sha1sum *.ctx

tpm2_readpublic -c unique1.ctx
tpm2_readpublic -c unique2.ctx
tpm2_flushcontext -t

echo ""

tpm2_readpublic -c unique3.ctx
tpm2_readpublic -c unique4.ctx
tpm2_flushcontext -t

# however it is possible to change the underlying seed which can cause
# the primary key hierarchy to be different, note that not all TPM supports this
# if this seed is changed, any key that was previously generated are rendered worthless
tpm2_changepps
