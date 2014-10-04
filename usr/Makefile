#
# usr/ Makefile
#

MAKEFLAGS += -rR --no-print-directory

PHONY := all tests clean FORCE

all:

tests: FORCE
	@$(MAKE) -C tests/

clean:
	@$(MAKE) -C tests/ clean

FORCE:

.PHONY = $(PHONY)
