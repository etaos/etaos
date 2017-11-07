obj-m += etaos.o

etaos-$(CONFIG_LIBETAOS:m=y) = kernel.o cpu.o platform.o chardev.o thread.o
etaos-$(CONFIG_SRAM:m=y) += sram.o
etaos-$(CONFIG_EEPROM:m=y) += eeprom.o
etaos-$(CONFIG_DHT11:m=y) += dht.o
etaos-$(CONFIG_BMP085:m=y) += bmp.o
etaos-y += string.o
