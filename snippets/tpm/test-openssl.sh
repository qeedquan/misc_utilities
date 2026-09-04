#!/bin/sh

test_openssl() {
	# list the default dynamic engine loader
	openssl engine -t -tt -vvvv -c dynamic
	
	if [ -z $OPENSSL_TPM_SO ];
	then
		echo "\$OPENSSL_TPM_SO not set"
		exit 1
	fi

	# to load an engine from shared lib
	openssl engine -t -tt -vvvv -c $OPENSSL_TPM_SO

	# generate some random number
	openssl rand -engine $OPENSSL_TPM_SO -hex 128
}

test_openssl
