ETAOS=$(shell pwd)/../../..
AVRUPLOAD=../../../scripts/avr_upload
OBJCOPY=/usr/bin/avr-objcopy
CPUFREQ=16000000

MCU=atmega2560
PROGRAMMER=stk500v2
BAUD=115200
PORT=/dev/ttyACM0
#PORT=/dev/pts/2

MAKEFLAGS += -rR --no-print-directory

all:
	@$(MAKE) -C $(ETAOS) A=$(PWD) ARCH=avr CROSS_COMPILE=avr- app

clean:
	@$(MAKE) -C $(ETAOS) A=$(PWD) ARCH=avr CROSS_COMPILE=avr- clean

test:
	@$(SIMULAVR) -d atmega328 -f test-app.img -W $(WR),- -R $(RD),- \
		-e $(EX) -F $(CPUFREQ)

hex: all
	@$(OBJCOPY) -R .eeprom -O ihex test-app.img test-app.hex 

upload:
	@$(AVRUPLOAD) -fH test-app.hex -m $(MCU) -p $(PROGRAMMER) -P $(PORT) \
		-b $(BAUD) -c /etc/avrdude.conf

