from setuptools import setup, Extension

"""

1. Run `pip install .`
2. pip install will build and install the module such that you can `import my_math`

"""

my_math_module = Extension(
    'my_math',
    sources=['my_math.c']
)

setup(
    name='my_math',
    version='1.0',
    description='A minimal Python C extension example',
    ext_modules=[my_math_module]
)
