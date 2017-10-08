ETAOS=$(shell pwd)/../../..
AVRUPLOAD=../../../scripts/avr_upload
OBJCOPY=/usr/bin/avr-objcopy

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
	@$(OBJCOPY) -R .eeprom -O ihex test-app.img test-app.hex 

upload:
	@$(AVRUPLOAD) -fH test-app.hex -m $(MCU) -p $(PROGRAMMER) -P $(PORT) \
		-b $(BAUD) -c /etc/avrdude.conf

