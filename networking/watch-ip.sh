#!/bin/sh

# Periodically check what the IP is using public internet servers

if [ "$HOST" = "" ]; then
	HOST="o-o.myaddr.l.google.com"
fi

if [ "$NAME" = "" ]; then
	NAME="@ns1.google.com"
fi

while true; do
	echo "$(date) | $(dig TXT +short $HOST $NAME)"
	sleep 3600
done
