#!/bin/sh

# show certificates from a site
# get-cert.sh google.com:443

echo 'exit' | openssl s_client --showcerts -connect $@
