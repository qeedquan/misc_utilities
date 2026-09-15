# To set the variable before building:
# FOO=1 make
ifdef FOO
    VAR := $(FOO)
else
    VAR := default_value
endif

# BAR=abc make
BAZ = default_baz
ifeq ($(BAR), abc)
    BAZ = custom_baz
-include variables.def
endif

all:
	@echo "Variable is: $(VAR)"
	@echo "Variable is: $(BAZ)"
	@echo "Variable is: $(CON)"

