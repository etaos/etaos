obj-m += etaos.o

etaos-$(CONFIG_LIBETAOS) = kernel.o cpu.o platform.o chardev.o thread.o
etaos-$(CONFIG_SRAM:m=y) += sram.o
etaos-$(CONFIG_EEPROM:m=y) += eeprom.o
etaos-y += string.o
