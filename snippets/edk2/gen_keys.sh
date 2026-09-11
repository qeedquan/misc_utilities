#!/bin/sh

# gen keys for the whitelist database

if [ $# -ne 1 ]
then
	echo "Usage: <directory>"
	exit 1
fi

KEYDIR=$1

mkdir -p $KEYDIR
cd $KEYDIR

for key in PK KEK dsk1
do
	openssl req -new -x509 -newkey rsa:2048 -subj "/CN=Custom PK/" -keyout $key.key -out $key.crt -days 3650 -nodes -sha256
	#openssl req -out $key.csr -key $key.key -new
	openssl x509 -outform der -in $key.crt -out $key.cer
done
