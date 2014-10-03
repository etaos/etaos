#
# Tests root Makefile
#

all: mm

MAKEFLAGS += -rR --no-print-directory

mm: FORCE
	@echo [SIMUL AVR] Running MM test
	@$(MAKE) -C mm/ all test
	@echo [SIMUL AVR] MM test ended
	@echo

FORCE:

clean: FORCE
	@$(MAKE) -C mm/ clean
