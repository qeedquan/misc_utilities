// http://net-snmp.sourceforge.net/wiki/index.php/TUT:Simple_Application

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <string.h>

static const char *argv0;

// authentication password for server, must be correct to access
static const char *passphrase = "The Net-SNMP Demo Password";

char *
xstrdup(const char *s)
{
	char *p;

	p = strdup(s);
	if (!p)
		abort();
	return p;
}

void
snmpfatal(const char *fmt, ...)
{
	char str[128];
	va_list ap;

	va_start(ap, fmt);
	vsnprintf(str, sizeof(str), fmt, ap);
	va_end(ap);
	snmp_perror(argv0);
	snmp_log(LOG_ERR, "%s\n", str);
	exit(1);
}

int
main(int argc, char *argv[])
{
	netsnmp_session s, *ss;
	netsnmp_pdu *p, *r;
	netsnmp_variable_list *v;
	oid o[MAX_OID_LEN];
	size_t no;
	int i, rv;

	argv0 = argv[0];

	init_snmp("snmpdemoapp");
	snmp_sess_init(&s);

	// use SNMPv3 to talk to experimental server
	s.peername = xstrdup("test.net-snmp.org");
	s.version = SNMP_VERSION_3;

	// set SNMPv3 user name
	s.securityName = xstrdup("MD5User");
	s.securityNameLen = strlen(s.securityName);

	// set security level to authenticated, but not encrypted
	s.securityLevel = SNMP_SEC_LEVEL_AUTHNOPRIV;

	// use authentication method MD5
	s.securityAuthProto = usmHMACMD5AuthProtocol;
	s.securityAuthProtoLen = sizeof(usmHMACMD5AuthProtocol) / sizeof(oid);
	s.securityAuthKeyLen = USM_AUTH_KU_LEN;

	// set authentication key to MD5 hashed version of passphrase
	if (generate_Ku(s.securityAuthProto,
	                s.securityAuthProtoLen,
	                (u_char *)passphrase, strlen(passphrase),
	                s.securityAuthKey,
	                &s.securityAuthKeyLen) != SNMPERR_SUCCESS)
		snmpfatal("Error generating Ku from authentication pass phrase");

	SOCK_STARTUP;
	ss = snmp_open(&s);
	if (!ss)
		snmpfatal("Failed to open SNMP session");

	// make a get request
	p = snmp_pdu_create(SNMP_MSG_GET);
	no = MAX_OID_LEN;
	if (!snmp_parse_oid(".1.3.6.1.2.1.1.1.0", o, &no))
		snmpfatal("failed to parse OID");
	// make same request 10 times
	for (i = 0; i < 10; i++)
		snmp_add_null_var(p, o, no);

	// send request out
	rv = snmp_synch_response(ss, p, &r);
	if (rv != STAT_SUCCESS) {
		snmp_sess_perror("snmpget", ss);
		exit(1);
	}
	if (r->errstat != SNMP_ERR_NOERROR)
		snmpfatal("Error in packet! Reason: %s", snmp_errstring(r->errstat));

	for (v = r->variables; v; v = v->next_variable)
		print_variable(v->name, v->name_length, v);

	snmp_free_pdu(r);
	snmp_close(ss);
	SOCK_CLEANUP;

	return 0;
}
