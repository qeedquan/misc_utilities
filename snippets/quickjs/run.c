#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <getopt.h>
#include "quickjs-libc.h"

#define nelem(x) (sizeof(x) / sizeof(x[0]))

enum {
	FALSE,
	TRUE
};

struct timespec start;

int
jsintr(JSRuntime *rt, void *opaque)
{
	struct timespec now;

	clock_gettime(CLOCK_MONOTONIC, &now);
	if (now.tv_sec - start.tv_sec >= 5)
		return 1;
	return 0;
}

JSValue
jsleep(JSContext *ctx, JSValueConst self, int argc, JSValueConst *argv)
{
	const char *s;
	int t;

	if (argc != 1)
		return JS_EXCEPTION;

	s = JS_ToCString(ctx, argv[0]);
	t = atoi(s);
	printf("%s(%d)\n", __func__, t);
	JS_FreeCString(ctx, s);
	return JS_UNDEFINED;
}

JSValue
jsprint(JSContext *ctx, JSValueConst self, int argc, JSValueConst *argv)
{
	const char *str;
	int i;

	printf("%s(", __func__);
	for (i = 0; i < argc; i++) {
		str = JS_ToCString(ctx, argv[i]);
		printf("%s", str);
		if (i + 1 < argc)
			printf(", ");
	}
	printf(")\n");
	return JS_UNDEFINED;
}

void
usage(void)
{
	fprintf(stderr, "usage: [-i inc.js] file.js\n");
	exit(1);
}

int
eval_buf(JSContext *ctx, const void *buf, int buf_len,
         const char *filename, int eval_flags)
{
	JSValue val;
	int ret;

	if ((eval_flags & JS_EVAL_TYPE_MASK) == JS_EVAL_TYPE_MODULE) {
		// for the modules, we compile then run to be able to set import.meta
		val = JS_Eval(ctx, buf, buf_len, filename,
		              eval_flags | JS_EVAL_FLAG_COMPILE_ONLY);
		if (!JS_IsException(val)) {
			js_module_set_import_meta(ctx, val, TRUE, TRUE);
			val = JS_EvalFunction(ctx, val);
		}
	} else {
		val = JS_Eval(ctx, buf, buf_len, filename, eval_flags);
	}
	if (JS_IsException(val)) {
		js_std_dump_error(ctx);
		ret = -1;
	} else {
		ret = 0;
	}
	JS_FreeValue(ctx, val);
	return ret;
}

int
eval_file(JSContext *ctx, const char *filename, int module)
{
	uint8_t *buf;
	int ret, eval_flags;
	size_t buf_len;

	buf = js_load_file(ctx, &buf_len, filename);
	if (!buf) {
		perror(filename);
		exit(1);
	}

	if (module)
		eval_flags = JS_EVAL_TYPE_MODULE;
	else
		eval_flags = JS_EVAL_TYPE_GLOBAL;
	ret = eval_buf(ctx, buf, buf_len, filename, eval_flags);
	js_free(ctx, buf);
	return ret;
}

int
main(int argc, char *argv[])
{
	JSRuntime *rt;
	JSContext *ctx;
	JSValue obj;

	char *includes[32];
	size_t ninclude;

	size_t i;
	int c;

	ninclude = 0;
	while ((c = getopt(argc, argv, "i:")) != -1) {
		switch (c) {
		case 'i':
			if (ninclude < nelem(includes))
				includes[ninclude++] = optarg;

			break;
		default:
			usage();
			break;
		}
	}
	argc -= optind;
	argv += optind;

	if (argc < 1)
		usage();

	rt = JS_NewRuntime();
	assert(rt != NULL);

	ctx = JS_NewContext(rt);
	assert(ctx != NULL);

	JS_SetModuleLoaderFunc(rt, NULL, js_module_loader, NULL);
	js_std_add_helpers(ctx, argc - 1, argv + 1);
	js_init_module_std(ctx, "std");
	js_init_module_os(ctx, "os");

	obj = JS_GetGlobalObject(ctx);
	JS_SetPropertyStr(ctx, obj, "printc", JS_NewCFunction(ctx, jsprint, "printc", 1));
	JS_SetPropertyStr(ctx, obj, "sleepc", JS_NewCFunction(ctx, jsleep, "sleepc", 1));
	JS_FreeValue(ctx, obj);

	clock_gettime(CLOCK_MONOTONIC, &start);
	const char *str = "import * as std from 'std';\n"
	                  "import * as os from 'os';\n"
	                  "globalThis.std = std;\n"
	                  "globalThis.os = os;\n";
	eval_buf(ctx, str, strlen(str), "<input>", JS_EVAL_TYPE_MODULE);

	for (i = 0; i < ninclude; i++) {
		printf("loading include %s\n", includes[i]);
		eval_file(ctx, includes[i], 0);
	}

	JS_SetInterruptHandler(JS_GetRuntime(ctx), jsintr, NULL);
	eval_file(ctx, argv[0], 0);

	JS_FreeContext(ctx);
	JS_FreeRuntime(rt);

	return 0;
}
