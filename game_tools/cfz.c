// used for font character arrays
// resize it by just copying the pixels
// n-fold where n is an integer number >= 1
// to zoom by
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <err.h>
#include <getopt.h>

#include <clang-c/Index.h>

#define nelem(x) (sizeof(x) / sizeof(x[0]))

typedef struct Ctx Ctx;
typedef struct Var Var;

struct Ctx {
	CXTranslationUnit tu;
	Var *var;
	size_t nvar;
};

struct Var {
	char *name;
	CXType type;
	long long *val;
	size_t len;
};

char *
xstrdup(const char *s)
{
	char *p = strdup(s);
	if (s == NULL)
		err(1, "strdup");
	return p;
}

void *
xcalloc(size_t nmemb, size_t size)
{
	if (nmemb == 0)
		nmemb = 1;
	if (size == 0)
		size = 1;
	void *p = calloc(nmemb, size);
	if (p == NULL)
		err(1, "calloc");
	return p;
}

#define MUL_NO_OVERFLOW (1UL << (sizeof(size_t) * 4))

void *
reallocarray(void *optr, size_t nmemb, size_t size)
{
	if ((nmemb >= MUL_NO_OVERFLOW || size >= MUL_NO_OVERFLOW) &&
	    nmemb > 0 && SIZE_MAX / nmemb < size) {
		errno = ENOMEM;
		return NULL;
	}
	return realloc(optr, size * nmemb);
}

void *
xreallocarray(void *optr, size_t nmemb, size_t size)
{
	void *p = reallocarray(optr, nmemb, size);
	if (p == NULL)
		err(1, "realloc");
	return p;
}

FILE *
xopen_memstream(char **str, size_t *len)
{
	FILE *fp = open_memstream(str, len);
	if (fp == NULL)
		err(1, "open_memstream");
	return fp;
}

char *
kindstr(enum CXCursorKind kind)
{
	CXString name = clang_getCursorKindSpelling(kind);
	char *str = xstrdup(clang_getCString(name));
	clang_disposeString(name);
	return str;
}

char *
typestr(CXType type)
{
	CXString name = clang_getTypeSpelling(type);
	char *str = xstrdup(clang_getCString(name));
	clang_disposeString(name);
	return str;
}

char *
cxcstr(CXCursor cursor, Ctx *ctx)
{
	CXSourceRange range = clang_getCursorExtent(cursor);
	CXToken *tokens = NULL;
	unsigned int ntokens = 0;

	char *str = NULL;
	size_t len;
	FILE *fp = xopen_memstream(&str, &len);

	clang_tokenize(ctx->tu, range, &tokens, &ntokens);
	for (unsigned int i = 0; i < ntokens; i++) {
		CXString spelling = clang_getTokenSpelling(ctx->tu, tokens[i]);
		fprintf(fp, "%s", clang_getCString(spelling));
		clang_disposeString(spelling);
	}
	clang_disposeTokens(ctx->tu, tokens, ntokens);
	fclose(fp);

	if (str == NULL)
		err(1, "open_memstream");

	return str;
}

long long
eval(const char *str)
{
	const char *p = str;
	long long v = 0;
	for (; *p;) {
		if ('a' <= *p && *p <= 'z') {
			v = *p - 'a';
			p++;
		} else if ('A' <= *p && *p <= 'Z') {
			v = *p - 'Z';
			p++;
		} else {
			char *endptr = NULL;
			v = strtoll(p, &endptr, 0);
			if (endptr != NULL)
				p = endptr;
			else
				p++;
		}
	}
	return v;
}

enum CXChildVisitResult
xvarinitlist(CXCursor cursor, CXCursor parent, CXClientData data)
{
	enum CXCursorKind kind = clang_getCursorKind(cursor);
	if (kind == CXCursor_UnexposedExpr)
		return CXChildVisit_Recurse;

	Ctx *ctx = data;
	Var *var = &ctx->var[ctx->nvar - 1];
	switch (kind) {
	case CXCursor_IntegerLiteral:
	case CXCursor_CharacterLiteral: {
		char *str = cxcstr(cursor, data);
		long long val = eval(str);
		free(str);

		var->val[var->len++] = val;
		break;
	}

	default:
		errx(1, "unsupported expression type for constant: %s", kindstr(kind));
	}

	return CXChildVisit_Continue;
}

enum CXChildVisitResult
xvardecl(CXCursor cursor, CXCursor parent, CXClientData data)
{
	enum CXCursorKind kind;

	kind = clang_getCursorKind(cursor);
	if (kind == CXCursor_InitListExpr)
		clang_visitChildren(cursor, xvarinitlist, data);

	return CXChildVisit_Continue;
}

enum CXChildVisitResult
visitor(CXCursor cursor, CXCursor parent, CXClientData data)
{
	CXSourceLocation location = clang_getCursorLocation(cursor);
	if (clang_Location_isFromMainFile(location) == 0)
		return CXChildVisit_Continue;

	Ctx *ctx = data;
	enum CXCursorKind kind = clang_getCursorKind(cursor);
	if (kind == CXCursor_VarDecl) {
		CXType type = clang_getCursorType(cursor);

		long long asize = clang_getArraySize(type);
		if (asize <= 0)
			return CXChildVisit_Continue;
		CXType atype = clang_getArrayElementType(type);

		switch (atype.kind) {
		case CXType_UChar:
			break;
		default:
			return CXChildVisit_Continue;
		}

		CXString spelling = clang_getCursorSpelling(cursor);

		ctx->var = xreallocarray(ctx->var, sizeof(*ctx->var), ++ctx->nvar);
		Var *var = &ctx->var[ctx->nvar - 1];
		memset(var, 0, sizeof(*var));
		var->name = xstrdup(clang_getCString(spelling));
		var->type = type;
		var->val = xcalloc(asize, sizeof(*var->val));
		clang_disposeString(spelling);

		clang_visitChildren(cursor, xvardecl, data);
	}

	return CXChildVisit_Continue;
}

void
usage(void)
{
	fprintf(stderr, "usage: [options] file\n");
	fprintf(stderr, "  -l lsb first   (default: msb)\n");
	fprintf(stderr, "  -z zoom factor (default: 2)\n");
	exit(2);
}

long long *
resize(long long *val, size_t len, int zoom, int msb)
{
	long long *p = xcalloc(sizeof(*p), len * zoom * zoom);
	long long *q = p;
	size_t x = 0;

	for (size_t n = 0; n < len; n++) {
		for (int a = 0; a < zoom; a++) {
			for (size_t b = 0; b < 8; b++) {
				unsigned int bit;
				if (msb)
					bit = (val[n] >> (7 - b)) & 0x1;
				else
					bit = (val[n] >> b) & 0x1;

				for (int c = 0; c < zoom; c++) {
					*q |= bit;

					if (++x == 8) {
						x = 0;
						q++;
					} else
						*q <<= 1;
				}
			}
		}
	}
	return p;
}

void
freectx(Ctx *ctx)
{
	for (size_t i = 0; i < ctx->nvar; i++) {
		Var *v = &ctx->var[i];
		free(v->val);
		free(v->name);
	}
	free(ctx->var);
}

int
main(int argc, char *argv[])
{
	int msb = 1;
	int zoom = 2;
	int c;
	while ((c = getopt(argc, argv, "mz:")) != -1) {
		switch (c) {
		case 'l':
			msb = 0;
			break;
		case 'z':
			zoom = atoi(optarg);
			break;
		}
	}

	argc -= optind;
	argv += optind;

	if (argc < 1)
		usage();
	if (zoom < 1 || zoom > 128)
		errx(1, "invalid zoom factor");

	CXIndex index = clang_createIndex(0, 1);
	CXTranslationUnit tu = clang_parseTranslationUnit(index, argv[0], NULL, 0, NULL, 0, CXTranslationUnit_None);
	if (tu == NULL)
		errx(1, "%s: failed to parse translation unit!", argv[1]);

	Ctx ctx = {.tu = tu};
	CXCursor root = clang_getTranslationUnitCursor(tu);
	clang_visitChildren(root, visitor, &ctx);

	clang_disposeTranslationUnit(tu);
	clang_disposeIndex(index);

	printf("/* Automatically generated by cfz */\n");
	for (size_t i = 0; i < ctx.nvar; i++) {
		Var *v = &ctx.var[i];
		CXType atype = clang_getArrayElementType(v->type);
		size_t n = 0;

		long long *val = resize(v->val, v->len, zoom, msb);
		char *type = typestr(atype);

		printf("\n");
		printf("%s %s_z%d[%zu] = {\n", type, v->name, zoom, v->len * zoom * zoom);
		for (size_t j = 0; j < v->len * zoom; j++) {
			if (n == 0)
				printf("    ");
			printf("0x%02llx,", val[j]);
			if (++n == 8) {
				n = 0;
				printf("\n");
			}
		}
		printf("};\n");

		free(type);
		free(val);
	}

	freectx(&ctx);

	return 0;
}
