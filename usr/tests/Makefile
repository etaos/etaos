#
# Tests root Makefile
#

all: mm devtest

MAKEFLAGS += -rR --no-print-directory

mm: FORCE
	@echo [SIMUL AVR] Running MM test
	@$(MAKE) -C mm/ all test
	@echo [SIMUL AVR] MM test ended
	@echo

devtest: FORCE
	@echo [SIMUL AVR] Running Devtest test
	@$(MAKE) -C devtest/ all test
	@echo [SIMUL AVR] Devtest test ended
	@echo

FORCE:

clean: FORCE
	@$(MAKE) -C mm/ clean
	@$(MAKE) -C devtest/ clean
