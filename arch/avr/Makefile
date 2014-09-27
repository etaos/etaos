#
# AVR Makefile
# (c) Michel Megens 2014
#

DEFCONF-$(CONFIG_ATMEGA328) += arch/avr/defconf/atmega328.dc

include $(DEFCONF-y)

KBUILD_CFLAGS += -nostdlib -nostdinc -ffreestanding -std=gnu89
KBUILD_CFLAGS += -Iarch/avr/include $(KBUILD_DBG)
KBUILD_AFLAGS += -Iarch/avr/include

mcu-$(CONFIG_ATMEGA328) = atmega328

ifneq ($(CONFIG_PICO_POWER),)
	MCU=$(addsuffix p, $(mcu-y))
else
	MCU=$(mcu-y)
endif

cflags-y += -mmcu=$(MCU)
aflags-$(CONFIG_ATMEGA328) += -mmcu=avr5
lflags-$(CONFIG_ATMEGA328) += -mavr5 -nostdlib -nostdinc

lscript-$(CONFIG_ATMEGA328) += arch/avr/ldscripts/avr5.xn
lscript-r-$(CONFIG_ATMEGA328) += arch/avr/ldscripts/avr5.xr

KBUILD_CFLAGS += $(cflags-y)
KBUILD_AFLAGS += $(aflags-y)
LDFLAGS += $(lflags-y) -T$(lscript-r-y)
LDFLAGS_etaos += $(lflags-y) -T$(lscript-y) -Tdata $(DATA_LOAD_OFFSET)

softfloat-$(CONFIG_LIBFLT) = -lflt

LIB_GCC = $(dir $(shell $(CC) $(KBUILD_CFLAGS) --print-libgcc-file-name))
ETAOS_EXTRA_LIBS += -L$(LIB_GCC) --start-group -lgcc $(softfloat-m) --end-group
export ETAOS_EXTRA_LIBS

head-y += arch/avr/kernel/start.o
init-y += arch/avr/kernel/init.o

core-y += arch/avr/kernel/
libs-y += arch/avr/libflt/

