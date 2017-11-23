#
# AVR Makefile
# (c) Michel Megens 2014
#

DEFCONF-$(CONFIG_ATMEGA328) += arch/avr/defconf/atmega328.dc
DEFCONF-$(CONFIG_ATMEGA1280) += arch/avr/defconf/atmega1280.dc
DEFCONF-$(CONFIG_ATMEGA2560) += arch/avr/defconf/atmega2560.dc

include $(DEFCONF-y)

KBUILD_CFLAGS += -nostdlib -nostdinc -ffreestanding
KBUILD_CFLAGS += -Iarch/avr/include $(KBUILD_DBG)
KBUILD_CFLAGS += -ffunction-sections -fdata-sections
KBUILD_AFLAGS += -Iarch/avr/include

# Optimisatio flags.
debug-flags-$(CONFIG_DEBUG) := -gstabs
ifeq ($(debug-flags-y),)
optimise-size-flags-$(CONFIG_OPTIMIZE_SIZE) := -Os
optimise-full-flags-$(CONFIG_OPTIMIZE_O3) := -O2 -fpredictive-commoning -fpeel-loops -funswitch-loops
optimise-full-flags-$(CONFIG_OPTIMIZE_O3) += -fgcse-after-reload -ftree-loop-distribute-patterns \
                        -ftree-slp-vectorize -fvect-cost-model -ftree-partial-pre -fipa-cp-clone
optimise-default-flags-$(CONFIG_OPTIMIZE_O2) := -O2
optimise-fast-flags-$(CONFIG_OPTIMIZE_FAST) := $(optimise-full-flags-) -ffast-math
endif

KBUILD_CFLAGS += $(debug-flags-y)
KBUILD_CFLAGS += $(optimise-default-flags-y) $(optimise-full-flags-y) $(optimise-size-flags-y)
KBUILD_CFLAGS += $(optimise-fast-flags-y)

KBUILD_CXXFLAGS += $(debug-flags-y)
KBUILD_CXXFLAGS += $(optimise-default-flags-y) $(optimise-full-flags-y) $(optimise-size-flags-y)
KBUILD_CXXFLAGS += $(optimise-fast-flags-y)

ifneq ($(CONFIG_PICO_POWER),)
	MCU=$(addsuffix p, $(mcu-y))
else
	MCU=$(mcu-y)
endif

cflags-y += -mmcu=$(MCU)

KBUILD_CFLAGS += $(cflags-y)
KBUILD_AFLAGS += $(aflags-y)
LDFLAGS += $(lflags-y) -T$(lscript-r-y)
LDFLAGS_etaos += $(lflags-y) -T$(lscript-y) -Tdata $(DATA_LOAD_OFFSET)
LDFLAGS_etaos += --gc-sections --whole-archive

softfloat-$(CONFIG_LIBFLT) = -lflt

ifneq ($(NOLIBGCC),1)
ifneq ($(MCU),)
LIB_GCC = $(dir $(shell $(CC) $(KBUILD_CFLAGS) --print-libgcc-file-name))
endif
ETAOS_EXTRA_LIBS += -L$(LIB_GCC) --no-whole-archive \
		    --start-group -lgcc $(softfloat-m) --end-group
export ETAOS_EXTRA_LIBS
endif

KBUILD_CXXFLAGS += $(KBUILD_CFLAGS)

head-y += arch/avr/kernel/start.o
init-y += arch/avr/kernel/devinit.o arch/avr/kernel/init.o

core-y += arch/avr/kernel/
libs-y += arch/avr/libflt/

ldscripts-y += $(lscript-y)

define archhelp
  @echo '*  etaos.elf       - ELF image using load address 0x0'
endef
