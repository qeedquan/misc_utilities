// cc -o basic-agent basic-agent.c `net-snmp-config --agent-libs` -Wall -Wextra -pedantic
// If running as subagent, snmpd must be running, for writes need to set the community password
// or else it snmpd will ignore it and return no such instance
// for traps, need snmptrapd to run to catch traps
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#include <net-snmp/library/tools.h>
#include <string.h>
#include <signal.h>
#include <stdbool.h>

#define nelem(x) (sizeof(x) / sizeof(x[0]))

typedef struct Conf Conf;
typedef struct Vars Vars;

struct Conf {
	bool agentx_subagent;
	bool syslog;
	bool background;
};

struct Vars {
	long longs[2];
	u_long ints[1];
	char strs[2][256];
	u_long delay_time;
	int alarm_count;
	u_int hr_alarm_id;
};

volatile sig_atomic_t running;
Conf conf;
Vars vars;

const char *
mode_string(int mode)
{
	switch (mode) {
	case MODE_GET:
		return "MODE_GET";
	case MODE_GETNEXT:
		return "MODE_GETNEXT";
	case MODE_GETBULK:
		return "MODE_GETBULK";
	case MODE_SET_BEGIN:
		return "MODE_SET_BEGIN";
	case MODE_SET_RESERVE1:
		return "MODE_SET_RESERVE1";
	case MODE_SET_RESERVE2:
		return "MODE_SET_RESERVE2";
	case MODE_SET_ACTION:
		return "MODE_SET_ACTION";
	case MODE_SET_UNDO:
		return "MODE_SET_UNDO";
	case MODE_SET_COMMIT:
		return "MODE_SET_COMMIT";
	case MODE_SET_FREE:
		return "MODE_SET_FREE";
	}
	return "MODE_UNKNOWN";
}

RETSIGTYPE
stop_server(int u)
{
	running = 0;
	(void)u;
}

int
log_watched_handler(netsnmp_mib_handler *handler,
                    netsnmp_handler_registration *reginfo,
                    netsnmp_agent_request_info *reqinfo,
                    netsnmp_request_info *requests)
{
	snmp_log(LOG_INFO, "handling watched variable %s\n", reginfo->handlerName);

	(void)handler;
	(void)reqinfo;
	(void)requests;

	return SNMP_ERR_NOERROR;
}

int
inc_int_handler(netsnmp_mib_handler *handler,
                netsnmp_handler_registration *reginfo,
                netsnmp_agent_request_info *reqinfo,
                netsnmp_request_info *requests)
{
	int *p;

	// watched variables are handled before the handler is called so the user will see the increment
	// on the next request
	snmp_log(LOG_INFO, "incrementing watched variable %s\n", reginfo->handlerName);
	p = reginfo->my_reg_void;
	p[0]++;

	(void)handler;
	(void)reqinfo;
	(void)requests;

	return SNMP_ERR_NOERROR;
}

int
gen_rand_handler(netsnmp_mib_handler *handler,
                 netsnmp_handler_registration *reginfo,
                 netsnmp_agent_request_info *reqinfo,
                 netsnmp_request_info *requests)
{
	char sval[32];
	u_long ival;
	size_t i, n;

	snmp_log(LOG_INFO, "handling %s\n", reginfo->handlerName);
	if (strstr(reginfo->handlerName, "String")) {
		n = rand() % nelem(sval);
		for (i = 0; i < n; i++)
			sval[i] = 'a' + rand() % 27;
		sval[n] = '\0';
		snmp_set_var_typed_value(requests->requestvb, ASN_OCTET_STR, sval, n);
	} else if (strstr(reginfo->handlerName, "Integer")) {
		ival = rand();
		snmp_set_var_typed_value(requests->requestvb, ASN_INTEGER, &ival, sizeof(ival));
	}
	(void)handler;
	(void)reqinfo;
	return SNMP_ERR_NOERROR;
}

void
return_delayed_response(unsigned int clientreg, void *clientarg)
{
	netsnmp_delegated_cache *cache;
	netsnmp_request_info *requests;
	netsnmp_agent_request_info *reqinfo;
	u_long *delay_time_cache;

	cache = netsnmp_handler_check_cache(clientarg);
	if (!cache) {
		snmp_log(LOG_ERR, "illegal call to return delayed response\n");
		return;
	}

	// re-establish the previous pointers we are used to having
	reqinfo = cache->reqinfo;
	requests = cache->requests;
	snmp_log(LOG_INFO, "called %s mode %s\n", __func__, mode_string(reqinfo->mode));

	// mention that it's no longer delegated, and we've now answered
	// the query (which we'll do down below).
	requests->delegated = 0;

	switch (reqinfo->mode) {
	case MODE_GET:
	case MODE_GETNEXT:
		snmp_set_var_typed_value(requests->requestvb, ASN_INTEGER, &vars.delay_time, sizeof(vars.delay_time));
		break;

	// BEGIN, RESERVE1, RESERVE2, ACTION, UNDO, COMMIT is part of a state machine
	// multiple states in the transaction, each state can actually be called multiple times
	// before moving on to the next state
	case MODE_SET_BEGIN:
		break;

	case MODE_SET_RESERVE1:
		if (requests->requestvb->type != ASN_INTEGER) {
			netsnmp_set_request_error(reqinfo, requests, SNMP_ERR_WRONGTYPE);
			netsnmp_free_delegated_cache(cache);
			return;
		}
		break;

	case MODE_SET_RESERVE2:
		// store old value for UNDO support in the future.
		delay_time_cache = netsnmp_memdup(&vars.delay_time, sizeof(vars.delay_time));

		// malloc failed
		if (delay_time_cache == NULL) {
			snmp_log(LOG_ERR, "malloc failed\n");
			netsnmp_set_request_error(reqinfo, requests, SNMP_ERR_RESOURCEUNAVAILABLE);
			netsnmp_free_delegated_cache(cache);
			return;
		}

		// Add our temporary information to the request itself.
		// This is then retrivable later.  The free function
		// passed auto-frees it when the request is later
		// deleted.
		netsnmp_request_add_list_data(requests, netsnmp_create_data_list("test_delayed", delay_time_cache, free));
		break;

	case MODE_SET_ACTION:
		// update current value
		vars.delay_time = *(requests->requestvb->val.integer);
		break;

	case MODE_SET_UNDO:
		// ack, something somewhere failed.  We reset back to the
		// previously old value by extracting the previosuly
		// stored information back out of the request
		vars.delay_time = *((u_long *)netsnmp_request_get_list_data(requests, "test_delayed"));
		break;

	case MODE_SET_COMMIT:
	case MODE_SET_FREE:
		// the only thing to do here is free the old memdup'ed
		// value, but it's auto-freed by the datalist recovery, so
		// we don't have anything to actually do here
		break;
	}

	// free the information cache
	netsnmp_free_delegated_cache(cache);

	(void)clientreg;
}

int
delayed_instance_handler(netsnmp_mib_handler *handler,
                         netsnmp_handler_registration *reginfo,
                         netsnmp_agent_request_info *reqinfo,
                         netsnmp_request_info *requests)
{
	netsnmp_delegated_cache *cache;

	snmp_log(LOG_NOTICE, "handler %s mode %s\n", reginfo->handlerName, mode_string(reqinfo->mode));

	requests->delegated = 1;
	cache = netsnmp_create_delegated_cache(handler, reginfo, reqinfo, requests, NULL);
	snmp_alarm_register(vars.delay_time, 0, return_delayed_response, cache);
	return SNMP_ERR_NOERROR;
}

/*
[07 e3 02 17 01 28 00 00 2d 08 00]

field  octets  contents                  range
-----  ------  --------                  -----
1      1-2   year*                     0..65536
2       3    month                     1..12
3       4    day                       1..31
4       5    hour                      0..23
5       6    minutes                   0..59
6       7    seconds                   0..60
             (use 60 for leap-second)
7       8    deci-seconds              0..9
8       9    direction from UTC        '+' / '-'
9      10    hours from UTC*           0..13
10     11    minutes from UTC          0..59
*/
int
datetime0_handler(netsnmp_mib_handler *handler,
                  netsnmp_handler_registration *reginfo,
                  netsnmp_agent_request_info *reqinfo,
                  netsnmp_request_info *requests)
{
	char str[128];
	int off;
	u_char *buf;
	size_t i, len;
	time_t now;

	time(&now);
	buf = date_n_time(&now, &len);
	snmp_set_var_typed_value(requests->requestvb, ASN_OCTET_STR, buf, len);

	off = sprintf(str, "%s len = %zu buf = [", __func__, len);
	for (i = 0; i < len; i++) {
		off += sprintf(str + off, "%02x", buf[i]);
		if (i + 1 < len)
			off += sprintf(str + off, " ");
	}
	off += sprintf(str + off, "]\n");
	snmp_log(LOG_NOTICE, "%s", str);

	(void)handler;
	(void)reginfo;
	(void)reqinfo;
	return SNMP_ERR_NOERROR;
}

int
datetime1_handler(netsnmp_mib_handler *handler,
                  netsnmp_handler_registration *reginfo,
                  netsnmp_agent_request_info *reqinfo,
                  netsnmp_request_info *requests)
{
	u_char buf[16];
	size_t len;
	time_t now;
	struct tm tm;

	time(&now);
	gmtime_r(&now, &tm);

	len = sizeof(buf);
	netsnmp_dateandtime_set_buf_from_vars(buf, &len,
	                                      tm.tm_year + 1900, tm.tm_mon + 1,
	                                      tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec,
	                                      0, 0, 0, 0);
	snmp_set_var_typed_value(requests->requestvb, ASN_OCTET_STR, buf, len);

	snmp_log(LOG_NOTICE, "%s len = %zu buf = [%02x %02x %02x %02x %02x %02x %02x %02x]\n", reginfo->handlerName,
	         len, buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6], buf[7]);

	(void)handler;
	(void)reqinfo;
	return SNMP_ERR_NOERROR;
}

int
rand_datetime0_handler(netsnmp_mib_handler *handler,
                       netsnmp_handler_registration *reginfo,
                       netsnmp_agent_request_info *reqinfo,
                       netsnmp_request_info *requests)
{
	u_char buf[8];
	size_t len;
	int val;

	len = sizeof(buf);
	val = rand() % 65536;
	buf[0] = (val >> 8) & 0xff;
	buf[1] = val & 0xff;
	buf[2] = 1 + (rand() % 12);
	buf[3] = 1 + (rand() % 31);
	buf[4] = rand() % 24;
	buf[5] = rand() % 60;
	buf[6] = rand() % 61;
	buf[7] = rand() % 10;

	snmp_set_var_typed_value(requests->requestvb, ASN_OCTET_STR, buf, len);
	snmp_log(LOG_NOTICE, "%s len = %zu buf = [%02x %02x %02x %02x %02x %02x %02x %02x]\n", reginfo->handlerName,
	         len, buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6], buf[7]);

	(void)handler;
	(void)reqinfo;
	return SNMP_ERR_NOERROR;
}

void
heartbeat_notification(unsigned int clientreg, void *clientarg)
{
	snmp_log(LOG_INFO, "heartbeat notification\n");
	(void)clientreg;
	(void)clientarg;
}

void
alarm_notification(unsigned int clientreg, void *clientarg)
{
	char buf[80], *c;
	time_t tim;
	int *val;

	tim = time(NULL);
	ctime_r(&tim, buf);
	if ((c = strchr(buf, '\n')))
		*c = '\0';

	val = clientarg;
	if (*val < 5) {
		*val += 1;
		snmp_log(LOG_INFO, "alarm notification at %s, starting again in %d seconds\n", buf, *val);
		snmp_alarm_register(*val, 0, alarm_notification, clientarg);
	} else
		snmp_log(LOG_INFO, "alarm notification ending at %s\n", buf);

	(void)clientreg;
}

long
microseconds(void)
{
	struct timeval t;
	gettimeofday(&t, NULL);
	return t.tv_sec * 1e6 + t.tv_usec;
}

void
hr_alarm_notification(unsigned int clientreg, void *clientarg)
{
	static int count;
	u_int id;

	if (count++ >= 10) {
		id = *(u_int *)clientarg;
		snmp_alarm_unregister(id);
	}
	printf("hr_alarm: %ld (ms)\n", microseconds());

	(void)clientreg;
}

void
send_trap_notification(unsigned int clientreg, void *clientarg)
{
	static const oid notification_oid[] = {1, 3, 6, 1, 4, 1, 8072, 2, 3, 0, 1};
	static const oid objid_snmptrap[] = {1, 3, 6, 1, 6, 3, 1, 1, 4, 1, 0};
	static const oid hbeat_rate_oid[] = {1, 3, 6, 1, 4, 1, 8072, 2, 3, 2, 1, 0};
	static const oid hbeat_name_oid[] = {1, 3, 6, 1, 4, 1, 8072, 2, 3, 2, 2, 0};

	size_t notification_oid_len;
	size_t objid_snmptrap_len;
	size_t hbeat_rate_oid_len;
	size_t hbeat_name_oid_len;

	netsnmp_variable_list *notification_vars;
	int heartbeat_rate;
	const char *heartbeat_name;

	snmp_log(LOG_INFO, "notification trap\n");

	notification_vars = NULL;
	heartbeat_name = "A girl named Maria";
	heartbeat_rate = rand() % 60;

	notification_oid_len = OID_LENGTH(notification_oid);
	objid_snmptrap_len = OID_LENGTH(objid_snmptrap);
	hbeat_rate_oid_len = OID_LENGTH(hbeat_rate_oid);
	hbeat_name_oid_len = OID_LENGTH(hbeat_name_oid);

	snmp_varlist_add_variable(&notification_vars,
	                          objid_snmptrap,
	                          objid_snmptrap_len,
	                          ASN_OBJECT_ID,
	                          notification_oid,
	                          notification_oid_len * sizeof(oid));

	snmp_varlist_add_variable(&notification_vars,
	                          hbeat_rate_oid, hbeat_rate_oid_len,
	                          ASN_INTEGER,
	                          (u_char *)&heartbeat_rate,
	                          sizeof(heartbeat_rate));

	if (heartbeat_rate < 30) {
		snmp_varlist_add_variable(&notification_vars,
		                          hbeat_name_oid, hbeat_name_oid_len,
		                          ASN_OCTET_STR,
		                          heartbeat_name, strlen(heartbeat_name));
	}
	send_v2trap(notification_vars);
	snmp_free_varbind(notification_vars);

	(void)clientreg;
	(void)clientarg;
}

void
init_conf(Conf *c)
{
	c->agentx_subagent = true;
	c->background = false;
	c->syslog = false;
}

void
init_vars(Vars *v)
{
	v->longs[0] = 42;
	v->longs[1] = 23;
	v->ints[0] = 0;
	v->delay_time = 1;
	v->alarm_count = 1;
	snprintf(v->strs[0], sizeof(v->strs[0]), "hello, world!");
	snprintf(v->strs[1], sizeof(v->strs[1]), "snmp testing...1, 2, 3!!111");
}

void
init_mibs_wgtable(void)
{
	static const oid wgtab_oid[] = {1, 3, 6, 1, 4, 1, 8072, 2, 2, 1};

	netsnmp_table_data_set *table_set;
	netsnmp_table_row *row;
	netsnmp_handler_registration *reginfo;

	table_set = netsnmp_create_table_data_set("netSnmpIETFWGTable");
	table_set->allow_creation = 1;

	netsnmp_table_dataset_add_index(table_set, ASN_OCTET_STR);
	netsnmp_table_set_multi_add_default_row(table_set,
	                                        2, ASN_OCTET_STR, 1, NULL, 0,
	                                        3, ASN_OCTET_STR, 1, NULL, 0,
	                                        0);

	reginfo = netsnmp_create_handler_registration("netSnmpIETFWGTable", NULL, wgtab_oid, OID_LENGTH(wgtab_oid), HANDLER_CAN_RWRITE);
	netsnmp_register_table_data_set(reginfo, table_set, NULL);

	row = netsnmp_create_table_data_row();

	netsnmp_table_row_add_index(row, ASN_OCTET_STR, "snmpv3", strlen("snmpv3"));
	netsnmp_set_row_column(row, 2, ASN_OCTET_STR, "Russ Mundy", strlen("Russ Mundy"));
	netsnmp_mark_row_column_writable(row, 2, 1);

	netsnmp_set_row_column(row, 3, ASN_OCTET_STR, "David Harrington", strlen("David Harrington"));
	netsnmp_mark_row_column_writable(row, 3, 1);
	netsnmp_table_dataset_add_row(table_set, row);

	row = netsnmp_create_table_data_row();
	netsnmp_table_row_add_index(row, ASN_OCTET_STR, "snmpconf", strlen("snmpconf"));
	netsnmp_set_row_column(row, 2, ASN_OCTET_STR, "David Partain", strlen("David Partain"));
	netsnmp_mark_row_column_writable(row, 2, 1);
	netsnmp_set_row_column(row, 3, ASN_OCTET_STR, "Jon Saperia", strlen("Jon Saperia"));
	netsnmp_mark_row_column_writable(row, 3, 1);
	netsnmp_table_dataset_add_row(table_set, row);

	netsnmp_register_auto_data_table(table_set, NULL);
}

void
init_mibs(void)
{
	static const oid int0_oid[] = {1, 3, 6, 1, 4, 1, 8072, 2, 1, 1, 0};
	static const oid int1_oid[] = {1, 3, 6, 1, 4, 1, 8072, 2, 1, 2, 0};
	static const oid str0_oid[] = {1, 3, 6, 1, 4, 1, 8072, 2, 1, 3, 0};
	static const oid str1_oid[] = {1, 3, 6, 1, 4, 1, 8072, 2, 1, 4, 0};
	static const oid int2_oid[] = {1, 3, 6, 1, 4, 1, 8072, 2, 1, 5, 0};
	static const oid rstr0_oid[] = {1, 3, 6, 1, 4, 1, 8072, 2, 1, 6, 0};
	static const oid rint0_oid[] = {1, 3, 6, 1, 4, 1, 8072, 2, 1, 7, 0};
	static const oid sleeper_oid[] = {1, 3, 6, 1, 4, 1, 8072, 2, 1, 8, 0};
	static const oid datetime0_oid[] = {1, 3, 6, 1, 4, 1, 8072, 2, 1, 9};
	static const oid datetime1_oid[] = {1, 3, 6, 1, 4, 1, 8072, 2, 1, 10};
	static const oid rdatetime0_oid[] = {1, 3, 6, 1, 4, 1, 8072, 2, 1, 11, 0};

	static netsnmp_watcher_info watcher_info[3];
	netsnmp_handler_registration *reginfo;
	struct timeval tv;

	netsnmp_register_long_instance("basicInteger0", int0_oid, OID_LENGTH(int0_oid), &vars.longs[0], NULL);
	netsnmp_register_long_instance("basicInteger1", int1_oid, OID_LENGTH(int1_oid), &vars.longs[1], NULL);

	reginfo = netsnmp_create_handler_registration("basicString0", log_watched_handler, str0_oid, OID_LENGTH(str0_oid), HANDLER_CAN_RWRITE);
	netsnmp_init_watcher_info6(&watcher_info[0], vars.strs[0], strlen(vars.strs[0]), ASN_OCTET_STR, WATCHER_MAX_SIZE, sizeof(vars.strs[0]), NULL);
	netsnmp_register_watched_instance(reginfo, &watcher_info[0]);

	reginfo = netsnmp_create_handler_registration("basicString1", log_watched_handler, str1_oid, OID_LENGTH(str1_oid), HANDLER_CAN_RONLY);
	netsnmp_init_watcher_info6(&watcher_info[1], vars.strs[1], strlen(vars.strs[1]), ASN_OCTET_STR, WATCHER_MAX_SIZE, sizeof(vars.strs[1]), NULL);
	netsnmp_register_watched_instance(reginfo, &watcher_info[1]);

	reginfo = netsnmp_create_handler_registration("basicInteger2", inc_int_handler, int2_oid, OID_LENGTH(int2_oid), HANDLER_CAN_RONLY);
	reginfo->my_reg_void = &vars.ints[0];
	netsnmp_init_watcher_info(&watcher_info[2], &vars.ints[0], sizeof(vars.ints[0]), ASN_INTEGER, WATCHER_FIXED_SIZE);
	netsnmp_register_watched_instance(reginfo, &watcher_info[2]);

	reginfo = netsnmp_create_handler_registration("randomString0", gen_rand_handler, rstr0_oid, OID_LENGTH(rstr0_oid), HANDLER_CAN_RONLY);
	netsnmp_register_instance(reginfo);

	reginfo = netsnmp_create_handler_registration("randomInteger0", gen_rand_handler, rint0_oid, OID_LENGTH(rint0_oid), HANDLER_CAN_RONLY);
	netsnmp_register_instance(reginfo);

	reginfo = netsnmp_create_handler_registration("basicSleeper", delayed_instance_handler, sleeper_oid, OID_LENGTH(sleeper_oid), HANDLER_CAN_RWRITE);
	netsnmp_register_instance(reginfo);

	// scalar instance automatically handles the ending oid value .0 for us
	reginfo = netsnmp_create_handler_registration("basicDateTime0", datetime0_handler, datetime0_oid, OID_LENGTH(datetime0_oid), HANDLER_CAN_RONLY);
	netsnmp_register_read_only_scalar(reginfo);

	reginfo = netsnmp_create_handler_registration("basicDateTime1", datetime1_handler, datetime1_oid, OID_LENGTH(datetime1_oid), HANDLER_CAN_RONLY);
	netsnmp_register_read_only_scalar(reginfo);

	reginfo = netsnmp_create_handler_registration("randomDateTime0", rand_datetime0_handler, rdatetime0_oid, OID_LENGTH(rdatetime0_oid), HANDLER_CAN_RONLY);
	netsnmp_register_instance(reginfo);

	init_mibs_wgtable();

	snmp_alarm_register(5, SA_REPEAT, heartbeat_notification, NULL);
	snmp_alarm_register(10, SA_REPEAT, send_trap_notification, NULL);
	snmp_alarm_register(vars.alarm_count, 0, alarm_notification, &vars.alarm_count);

	tv.tv_sec = 0;
	tv.tv_usec = 100000;
	vars.hr_alarm_id = snmp_alarm_register_hr(tv, SA_REPEAT, hr_alarm_notification, &vars.hr_alarm_id);
}

void
init_snmp_lib(void)
{
	if (conf.syslog)
		snmp_enable_calllog();
	snmp_enable_stderrlog();

	if (conf.agentx_subagent)
		netsnmp_ds_set_boolean(NETSNMP_DS_APPLICATION_ID, NETSNMP_DS_AGENT_ROLE, 1);

	if (conf.background && netsnmp_daemonize(1, !conf.syslog))
		exit(1);

	SOCK_STARTUP;
	init_agent("basic-agent");
	init_mibs();
	if (!conf.agentx_subagent) {
		init_vacm();
		init_usm();
	}
	init_snmp("basic-agent");
	if (!conf.agentx_subagent)
		init_master_agent();

	running = 1;
	signal(SIGTERM, stop_server);
	signal(SIGINT, stop_server);
}

void
close_snmp_lib(void)
{
	snmp_log(LOG_INFO, "shutting down SNMP\n");
	snmp_shutdown("basic-agent");
	SOCK_CLEANUP;
}

int
main(void)
{
	srand(time(NULL));
	init_conf(&conf);
	init_vars(&vars);
	init_snmp_lib();
	while (running)
		agent_check_and_process(1);
	close_snmp_lib();
	return 0;
}
