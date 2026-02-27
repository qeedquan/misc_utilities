#!/bin/sh

# dump keys from the whitelist/blacklist database

if [ $# -ne 1 ]
then
	echo "Usage: <directory>"
	exit 1
fi

KEYDIR=$1

mkdir -p $KEYDIR
cd $KEYDIR

for var in PK KEK db dbx
do
	efi-readvar -v $var -o $var.esl
	sig-list-to-certs $var.esl $var
done

for der in *.der
do
	openssl x509 -in $der -inform der -noout -text > $der.txt
done
