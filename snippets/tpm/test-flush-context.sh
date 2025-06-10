#!/bin/sh

# Typically, when using the TPM, the interactions occur through a resource manager, like tpm2-abrmd(8).
# When the process exits, transient object handles are flushed.
# Thus, flushing transient objects through the command line is not required.
# However, when interacting with the TPM directly, this scenario is possible.
# The below example assumes direct TPM access not brokered by a resource manager.
# Specifically we will use the simulator.

# the TPM only has a fix number of object it can hold at once.
# tpm2_createprimary allocates an object every time, quickly leading to out of objects.
# this shows how to free it

flush_transient() {
	# shows all handles allocated
	tpm2_getcap -T handles-transient

	# give the id gotten from tpm2_getcap command to delete it
	# tpm2_flushcontext 0x80000000

	# this deletes all transient objects 
	tpm2_flushcontext -t
}

flush_session() {
	tpm2_startauthsession -S session.dat
	tpm2_flushcontext session.dat
}
