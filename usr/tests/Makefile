#
# Tests root Makefile
#

all: mm devtest timetest

MAKEFLAGS += -rR --no-print-directory

PHONY := mm
mm: FORCE
	@echo [SIMUL AVR] Running MM test
	@$(MAKE) -C mm/ all test
	@echo [SIMUL AVR] MM test ended
	@echo

PHONY += timetest
timetest: FORCE
	@echo [SIMUL AVR] Running TMTEST test
	@$(MAKE) -C tmtest/ test
	@echo [SIMUL AVR] TMTEST test ended
	@echo

PHONY += devtest
devtest: FORCE
	@echo [SIMUL AVR] Running Devtest test
	@$(MAKE) -C devtest/ all test
	@echo [SIMUL AVR] Devtest test ended
	@echo

PHONY += schedtest
schedtest: FORCE
	@echo [SIMUL AVR] Running schedtest test
	@$(MAKE) -C schedtest/ all test
	@echo [SIMUL AVR] schedtest test ended
	@echo

PHONY += clean
clean: FORCE
	@$(MAKE) -C mm/ clean
	@$(MAKE) -C tmtest/ clean
	@$(MAKE) -C devtest/ clean
	@$(MAKE) -C schedtest/ clean

PHONY += FORCE
FORCE:

.PHONY: $(PHONY)
