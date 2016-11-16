obj-$(CONFIG_LIBETAOS) += etaos.o

etaos-y = kernel.o cpu.o platform.o chardev.o
etaos-$(CONFIG_SRAM:m=y) += sram.o
etaos-$(CONFIG_EEPROM:m=y) += eeprom.o
