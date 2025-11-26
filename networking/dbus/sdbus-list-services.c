#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <systemd/sd-bus.h>

sd_bus *sys, *usr;

void
list_services(sd_bus *bus, const char *type)
{
	char **acquired, **activatable;
	size_t i, j;

	sd_bus_list_names(bus, &acquired, &activatable);
	for (i = 0; acquired[i]; i++) {
		for (j = 0; activatable[j]; j++) {
			if (!strcmp(acquired[i], activatable[j])) {
				break;
			}
		}

		printf("%-8s %-40s %-12s\n", type, acquired[i], (activatable[j]) ? "(activatable)" : "");
	}

	for (i = 0; acquired[i]; i++)
		free(acquired[i]);
	free(acquired);

	for (i = 0; activatable[i]; i++)
		free(activatable[i]);
	free(activatable);
}

void
list_machine_info(void)
{
	struct {
		sd_bus *bus;
		const char *type;
	} bustab[] = {
		{ sys, "System" },
		{ usr, "User" },
		{ NULL, NULL },
	};

	sd_bus *bus;
	sd_id128_t mid, bid, iid, rid;
	char s[SD_ID128_STRING_MAX];
	const char *type, *sender, *scope;
	pid_t tid;
	size_t i;

	sd_id128_get_machine(&mid);
	sd_id128_get_boot(&bid);
	sd_id128_get_invocation(&iid);
	sd_id128_randomize(&rid);

	printf("Machine    ID: %s\n", sd_id128_to_string(mid, s));
	printf("Boot       ID: %s\n", sd_id128_to_string(bid, s));
	printf("Invocation ID: %s\n", sd_id128_to_string(iid, s));
	printf("Random     ID: %s\n", sd_id128_to_string(rid, s));
	printf("\n");

	for (i = 0; bustab[i].bus; i++) {
		bus = bustab[i].bus;
		type = bustab[i].type;

		printf("%s Interactive Auth    %d\n", type, sd_bus_get_allow_interactive_authorization(bus));
		printf("%s Client              %d\n", type, sd_bus_is_bus_client(bus));
		printf("%s Server              %d\n", type, sd_bus_is_server(bus));
		printf("%s Anonymous           %d\n", type, sd_bus_is_anonymous(bus));
		printf("%s Trusted             %d\n", type, sd_bus_is_trusted(bus));
		printf("%s Monitor             %d\n", type, sd_bus_is_monitor(bus));
		printf("%s Exit on Disconnect  %d\n", type, sd_bus_get_exit_on_disconnect(bus));
		printf("%s Close on Exit       %d\n", type, sd_bus_get_close_on_exit(bus));
		printf("%s Connected Signal    %d\n", type, sd_bus_get_connected_signal(bus));
		if (sd_bus_get_sender(bus, &sender) >= 0)
			printf("%s Get Sender          '%s'\n", type, sender);
		if (sd_bus_get_scope(bus, &scope) >= 0)
			printf("%s Scope               '%s'\n", type, scope);
		if (sd_bus_get_tid(bus, &tid) >= 0)
			printf("%s TID                 %ju\n", type, (uintmax_t)tid);
		printf("\n");
	}
}

int
main(void)
{
	int r;

	r = sd_bus_default_system(&sys);
	assert(r >= 0);

	r = sd_bus_default(&usr);
	assert(r >= 0);

	list_machine_info();
	list_services(sys, "system");
	list_services(usr, "user");

	sd_bus_unref(sys);
	sd_bus_unref(usr);

	return 0;
}
