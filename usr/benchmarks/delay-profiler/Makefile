ETAOS=$(shell pwd)/../../..

AVRDUDE=/usr/bin/avrdude
OBJCOPY=/usr/bin/avr-objcopy
CPUFREQ=16000000

MCU=atmega328p
PROGRAMMER=arduino
BAUD=115200
PORT=/dev/ttyACM0

MAKEFLAGS += -rR --no-print-directory

all:
	@$(MAKE) -C $(ETAOS) A=$(PWD) ARCH=avr CROSS_COMPILE=avr- app

clean:
	@$(MAKE) -C $(ETAOS) A=$(PWD) ARCH=avr CROSS_COMPILE=avr- clean

hex: all
	@$(OBJCOPY) -R .eeprom -O ihex delay-profiler.img delay-profiler.hex

upload:
	@$(AVRDUDE) -D -q -V -p $(MCU) -c $(PROGRAMMER) -b $(BAUD) -P $(PORT) \
		-C /etc/avrdude.conf -U flash:w:delay-profiler.hex:i
