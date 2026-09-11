#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <json-c/json.h>

void walkobj(json_object *);

void
usage(void)
{
	fprintf(stderr, "usage: [options] file ...\n");
	exit(2);
}

void
walkvalue(json_object *jobj)
{
	enum json_type type;

	printf("type: ");
	type = json_object_get_type(jobj);
	switch (type) {
	default:
		printf("json_unknown_type (%d)\n", type);
		break;
	case json_type_null:
		printf("json_type_null\n");
		break;
	case json_type_boolean:
		printf("json_type_boolean (%s)\n", json_object_get_boolean(jobj) ? "true" : "false");
		break;
	case json_type_double:
		printf("json_type_double (%lf)\n", json_object_get_double(jobj));
		break;
	case json_type_int:
		printf("json_type_int (%d)\n", json_object_get_int(jobj));
		break;
	case json_type_string:
		printf("json_type_string (%s)\n", json_object_get_string(jobj));
		break;
	}
}

void
walkarray(json_object *jobj, char *key)
{
	json_object *jarray, *jvalue;
	enum json_type type;
	int arraylen, i;

	jarray = jobj;
	if (key)
		json_object_object_get_ex(jobj, key, &jarray);

	arraylen = json_object_array_length(jarray);
	printf("Array Length: %d\n", arraylen);

	for (i = 0; i < arraylen; i++) {
		jvalue = json_object_array_get_idx(jarray, i);
		type = json_object_get_type(jvalue);
		if (type == json_type_array) {
			walkarray(jvalue, NULL);
		} else if (type != json_type_object) {
			printf("value[%d]: ", i);
			walkvalue(jvalue);
		} else {
			walkobj(jvalue);
		}
	}
}

void
walkobj(json_object *jobj)
{
	enum json_type type;

	if (!jobj)
		return;

	json_object_object_foreach(jobj, key, val)
	{
		type = json_object_get_type(val);
		switch (type) {
		case json_type_null:
		case json_type_boolean:
		case json_type_double:
		case json_type_int:
		case json_type_string:
			walkvalue(val);
			break;
		case json_type_object:
			printf("json_type_object (%s)\n", key);
			walkobj(val);
			break;
		case json_type_array:
			printf("type: json_type_array, ");
			walkarray(jobj, key);
			break;
		}
	}
}

void
parse(const char *name)
{
	json_object *jobj;

	jobj = json_object_from_file(name);
	if (!jobj) {
		fprintf(stderr, "%s: failed to parse json\n", name);
		return;
	}

	walkobj(jobj);
	json_object_to_file("out.json", jobj);
	json_object_put(jobj);
}

int
main(int argc, char *argv[])
{
	int i;

	if (argc < 2)
		usage();

	for (i = 1; i < argc; i++)
		parse(argv[i]);

	return 0;
}
