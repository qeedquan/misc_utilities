#!/bin/sh

# the contents of the mail have from/to field, but those can be whatever, the options passed via curl is the one that matters

# sending mail using ssl
curl --url 'smtp://smtp.example.com:587' --ssl-reqd \
  --mail-from 'sender@example.com' --mail-rcpt 'recipient@example.com' \
  --upload-file mail.txt --user 'username:password'

# send regular mail
curl --url 'smtp://localhost:25' --mail-from 'fake@dino.com' --mail-rcpt 'user@localhost' --upload-file mail.txt
