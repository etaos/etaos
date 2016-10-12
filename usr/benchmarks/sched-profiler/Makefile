ETAOS=$(shell pwd)/../../..

AVRDUDE=/usr/bin/avrdude
OBJCOPY=/usr/bin/avr-objcopy
CPUFREQ=16000000

MCU=atmega2560
PROGRAMMER=stk500v2
BAUD=115200
PORT=/dev/ttyACM0

MAKEFLAGS += -rR --no-print-directory

all:
	@$(MAKE) -C $(ETAOS) A=$(PWD) ARCH=avr CROSS_COMPILE=avr- app

clean:
	@$(MAKE) -C $(ETAOS) A=$(PWD) ARCH=avr CROSS_COMPILE=avr- clean

hex: all
	@$(OBJCOPY) -R .eeprom -O ihex sched-profiler.img sched-profiler.hex

upload:
	@$(AVRDUDE) -D -q -V -p $(MCU) -c $(PROGRAMMER) -b $(BAUD) -P $(PORT) \
		-C /etc/avrdude.conf -U flash:w:sched-profiler.hex:i
