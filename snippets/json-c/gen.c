#include <stdio.h>
#include <json-c/json.h>

#define nelem(x) (sizeof(x) / sizeof(x[0]))

int
main(void)
{
	json_object *jobj = json_object_new_object();
	json_object *jarray[3];
	json_object *jstring[3][128];
	for (size_t i = 0; i < nelem(jstring); i++) {
		jarray[i] = json_object_new_array();
		for (size_t j = 0; j < nelem(jstring[i]); j++) {
			char buf[256];
			size_t val;

			val = 1;
			for (size_t k = 0; k <= i; k++)
				val *= j;

			snprintf(buf, sizeof(buf), "%zu %zu", j, val);
			jstring[i][j] = json_object_new_string(buf);
			json_object_array_put_idx(jarray[i], j, jstring[i][j]);
		}
	}
	json_object_object_add(jobj, "Line", jarray[0]);
	json_object_object_add(jobj, "Square", jarray[1]);
	json_object_object_add(jobj, "Cube", jarray[2]);
	int flags = JSON_C_TO_STRING_SPACED | JSON_C_TO_STRING_PRETTY;
	printf("%s\n", json_object_to_json_string_ext(jobj, flags));
	json_object_put(jobj);

	return 0;
}
