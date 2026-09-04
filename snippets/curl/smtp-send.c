#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <err.h>
#include <curl/curl.h>

typedef struct {
	char url[128];

	char from_addr[128];
	char to_addr[128];
	char cc_addr[128];

	char from_mail[128];
	char to_mail[128];
	char cc_mail[128];
} Flags;

Flags flags = {
    .url = "smtp://localhost",

    .from_addr = "<sender@example.org>",
    .to_addr = "<user@localhost>",
    .cc_addr = "",

    .from_mail = "Sender Person",
    .to_mail = "A receiver",
    .cc_mail = "",
};

void
gen_payload(char *buf)
{
	static const char fmt[] = "Date: Mon, 29 Nov 2010 21:54:29 +1100\r\n"
	                          "To: %s %s\r\n"
	                          "From: %s %s\r\n"
	                          "Cc: %s %s\r\n"
	                          "Message-ID: <dcd7cb36-11db-487a-9f3a-e652a9458efd@"
	                          "rfcpedant.example.org>\r\n"
	                          "Subject: SMTP example message\r\n"
	                          "\r\n" // empty line to divide headers from body, see RFC 5322
	                          "The body of the message starts here.\r\n"
	                          "\r\n"
	                          "It could be a lot of lines, could be MIME encoded, whatever.\r\n"
	                          "Check RFC 5322.\r\n";

	sprintf(buf, fmt,
	        flags.to_mail, flags.to_addr,
	        flags.from_mail, flags.from_addr,
	        flags.cc_mail, flags.cc_addr);
};

struct upload_status {
	size_t bytes_read;
};

static size_t
payload_source(char *ptr, size_t size, size_t nmemb, void *userp)
{
	struct upload_status *upload_ctx = (struct upload_status *)userp;
	char payload[1024];
	const char *data;
	size_t room = size * nmemb;

	if ((size == 0) || (nmemb == 0) || ((size * nmemb) < 1)) {
		return 0;
	}

	gen_payload(payload);
	data = &payload[upload_ctx->bytes_read];

	if (data) {
		size_t len = strlen(data);
		if (room < len)
			len = room;
		memcpy(ptr, data, len);
		upload_ctx->bytes_read += len;

		return len;
	}

	return 0;
}

void
usage(void)
{
	fprintf(stderr, "usage: [options]\n");
	fprintf(stderr, "  -i   specify url (default: %s)\n", flags.url);
	fprintf(stderr, "  -h   show this message\n");
	fprintf(stderr, "  -t   specify to address (default: %s)\n", flags.to_addr);
	exit(2);
}

int
main(int argc, char *argv[])
{
	CURL *curl;
	CURLcode res = CURLE_OK;
	struct curl_slist *recipients = NULL;
	struct upload_status upload_ctx = {0};
	int c;

	while ((c = getopt(argc, argv, "c:i:t:h")) != -1) {
		switch (c) {
		case 'c':
			snprintf(flags.cc_addr, sizeof(flags.cc_addr), "%s", optarg);
			snprintf(flags.cc_mail, sizeof(flags.cc_mail), "CC Guy");
			break;
		case 'i':
			snprintf(flags.url, sizeof(flags.url), "%s", optarg);
			break;
		case 't':
			snprintf(flags.to_addr, sizeof(flags.to_addr), "%s", optarg);
			break;
		case 'h':
			usage();
			break;
		}
	}

	curl = curl_easy_init();
	if (!curl)
		errx(1, "Failed to init CURL");

	// This is the URL for your mailserver
	curl_easy_setopt(curl, CURLOPT_URL, flags.url);

	// Note that this option is not strictly required, omitting it will result
	// in libcurl sending the MAIL FROM command with empty sender data. All
	// autoresponses should have an empty reverse-path, and should be directed
	// to the address in the reverse-path which triggered them. Otherwise,
	// they could cause an endless loop. See RFC 5321 Section 4.5.5 for more details.
	curl_easy_setopt(curl, CURLOPT_MAIL_FROM, flags.from_addr);

	// Add two recipients, in this particular case they correspond to the
	// To: and Cc: addressees in the header, but they could be any kind of recipient.
	recipients = curl_slist_append(recipients, flags.to_addr);
	if (flags.cc_addr[0])
		recipients = curl_slist_append(recipients, flags.cc_addr);
	curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);

	// We are using a callback function to specify the payload (the headers and
	// body of the message). You could just use the CURLOPT_READDATA option to
	// specify a FILE pointer to read from.
	curl_easy_setopt(curl, CURLOPT_READFUNCTION, payload_source);
	curl_easy_setopt(curl, CURLOPT_READDATA, &upload_ctx);
	curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);

	// Send the message
	res = curl_easy_perform(curl);

	// Check for errors
	if (res != CURLE_OK)
		fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));

	// Free the list of recipients
	curl_slist_free_all(recipients);

	// curl will not send the QUIT command until you call cleanup, so you
	// should be able to reuse this connection for additional messages
	// (setting CURLOPT_MAIL_FROM and CURLOPT_MAIL_RCPT as required, and
	// calling curl_easy_perform() again. It may not be a good idea to keep
	// the connection open for a long time though (more than a few minutes may
	// result in the server timing out the connection), and you do want to
	// clean up in the end.
	curl_easy_cleanup(curl);

	return res;
}
