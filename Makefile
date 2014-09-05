#
# ETA/OS root Makefile
#

MVN=mvn

.PHONY: build

build: scripts/build/pom.xml
	$(MVN) validate -f scripts/build/pom.xml
	$(MVN) compile -f scripts/build/pom.xml
	$(MVN) verify -f scripts/build/pom.xml
