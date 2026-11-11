#!/bin/sh

# the contents of the mail have from/to field, but those can be whatever, the options passed via curl is the one that matters

# sending mail using ssl
curl --url 'smtp://smtp.example.com:587' --ssl-reqd \
  --mail-from 'sender@example.com' --mail-rcpt 'recipient@example.com' \
  --upload-file mail.txt --user 'username:password'

# send regular mail
curl --url 'smtp://localhost:25' --mail-from 'fake@dino.com' --mail-rcpt 'user@localhost' --upload-file mail.txt

# send regular mail without having to specify the domain for the user
curl --url 'smtp://localhost:25' --mail-from 'fake@dino.com' --mail-rcpt 'user' --upload-file mail.txt

# get imap inbox using password authentication
# servers need to allow plain text authentication for this to work
curl -v 'imap://user:pass@example.com/INBOX?NEW'

# fetch mail
curl -v 'imap://user:pass@example.com/INBOX;UID=1'
# fetching mail addressing scheme using the above address scheme couples the user to the domain, email servers usually do this
# but it is possible to have domain name in a user name, to be able to address it, need to use the following syntax
curl -v imap://example.com -u user@another.domain:password

# read email from imaps/pop3s, ignore certificates
curl -k -n 'imaps://user:pass@host' -X 'STATUS INBOX (MESSAGES)'
curl -k -n 'pop3s://user:pass@host'
# read email message #1 and #2 in pop3s 
curl -k -n 'pop3s://user:pass@host/1'
curl -k -n 'pop3s://user:pass@host/2'
# delete email message #1 and 2 in pop3s
curl -k --request DELE 'pop3s://user:pass@host/1'
curl -k --request DELE 'pop3s://user:pass@host/2'


