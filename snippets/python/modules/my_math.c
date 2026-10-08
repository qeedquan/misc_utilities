#define PY_SSIZE_T_CLEAN
#include <Python.h>

static PyObject *
my_math_add(PyObject *self, PyObject *args)
{
	long a, b;

	if (!PyArg_ParseTuple(args, "ll", &a, &b))
		return NULL;
	return PyLong_FromLong(a + b);
}

static PyMethodDef MyMathMethods[] = {
	{ "add", my_math_add, METH_VARARGS, "Adds two integers together." },
	{ NULL, NULL, 0, NULL }
};

static struct PyModuleDef my_math_module = {
	PyModuleDef_HEAD_INIT,
	"my_math",
	"A minimal C extension module for basic math.",
	-1,
	MyMathMethods
};

PyMODINIT_FUNC
PyInit_my_math(void)
{
	return PyModule_Create(&my_math_module);
}
