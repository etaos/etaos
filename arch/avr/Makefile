#
# AVR Makefile
# (c) Michel Megens 2014
#

KBUILD_CFLAGS += -nostdlib -nostdinc -ffreestanding
KBUILD_CFLAGS += -Iarch/avr/include
KBUILD_AFLAGS += -Iarch/avr/include

cflags-$(CONFIG_ATMEGA328) += -mmcu=atmega328
aflags-$(CONFIG_ATMEGA328) += -mmcu=avr5

KBUILD_CFLAGS += $(cflags-y)
KBUILD_AFLAGS += $(aflags-y)

head-y += arch/avr/kernel/start.o
init-y += arch/avr/kernel/init.o

core-y += arch/avr/kernel/
