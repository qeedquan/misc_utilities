#!/bin/sh

# Generates a XML description of the C AST
castxml --castxml-cc-gnu-c gcc pi.c --castxml-output=1 -o pi.xml
