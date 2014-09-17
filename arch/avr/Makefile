#
# AVR Makefile
# (c) Michel Megens 2014
#

KBUILD_CFLAGS += -nostdlib -nostdinc -ffreestanding
KBUILD_CFLAGS += -Iarch/avr/include
KBUILD_AFLAGS += -Iarch/avr/include

cflags-$(CONFIG_ATMEGA328) += -mmcu=atmega328
aflags-$(CONFIG_ATMEGA328) += -mmcu=avr5
lflags-$(CONFIG_ATMEGA328) += -mmcu=avr5

KBUILD_CFLAGS += $(cflags-y)
KBUILD_AFLAGS += $(aflags-y)
KBUILD_LDFLAGS += $(lflags-y)

LIB_GCC = $(dir $(shell $(CC) $(KBUILD_CFLAGS) --print-libgcc-file-name))
export LIB_GCC

head-y += arch/avr/kernel/start.o
init-y += arch/avr/kernel/init.o

core-y += arch/avr/kernel/
