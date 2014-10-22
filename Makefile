#
# ETA/OS root Makefile
#

VERSION = 0
PATCHLEVEL = 2
SUBLEVEL = 2
EXTRAVERSION =
NAME = Shuffling Donkey

MAKEFLAGS += -rR --no-print-directory
# DO NOT make these files show up anywhere. For your own safety.
export RCS_FIND_IGNORE := \( -name SCCS -o -name BitKeeper -o -name .svn -o    \
			  -name CVS -o -name .pc -o -name .hg -o -name .git \) \
			  -prune -o

# To put more focus on warnings, be less verbose as default
# Use 'make V=1' to see the full commands
SRCARCH 	:= $(ARCH)
export SRCARCH

ifeq ("$(origin V)", "command line")
  KBUILD_VERBOSE = $(V)
endif
ifndef KBUILD_VERBOSE
  KBUILD_VERBOSE = 0
endif

KBUILD_CHECKSRC = $(CHKSRC)
ifeq ($(KBUILD_CHECKSRC),)
	KBUILD_CHECKSRC = 0
endif

ifeq ("$(origin A)", "command line")
  KBUILD_EXTMOD := $(A)
endif

ifneq ($(KBUILD_EXTMOD),)
  appdir	:= $(KBUILD_EXTMOD)
endif
srctree		:= $(if $(KBUILD_SRC),$(KBUILD_SRC),$(CURDIR))
objtree		:= $(CURDIR)

export srctree objtree

CONFIG_SHELL := $(shell if [ -x "$$BASH" ]; then echo $$BASH; \
	  else if [ -x /bin/bash ]; then echo /bin/bash; \
	  else echo sh; fi ; fi)

HOSTCC       = gcc
HOSTCXX      = g++
HOSTCFLAGS   = -Wall -Wno-char-subscripts -Wmissing-prototypes -Wstrict-prototypes -O2 -fomit-frame-pointer
HOSTCXXFLAGS = -O2

KBUILD_CFLAGS := -Wall -Iinclude
KBUILD_AFLAGS := -Iinclude

ifneq ($(DBG),)
	KBUILD_DBG := -g
endif
export KBUILD_DBG

ifeq ($(MAKECMDGOALS),prepare)
	NOLIBGCC := 1
endif

ifneq ($(filter config menuconfig xconfig gconfig nconfig oldconfig \
	silentoldconfig, $(MAKECMDGOALS)),)
	NOLIBGCC := 1
endif

ifeq ($(MAKECMDGOALS),modules)
else
  KBUILD_BUILTIN := 1
endif
# if we have a make <whatever> modules also build modules besides whatever
# else we're doing.
ifneq ($(filter all _all modules,$(MAKECMDGOALS)),)
	KBUILD_MODULES := 1
endif

ifeq ($(MAKECMDGOALS),)
	KBUILD_MODULES := 1
endif

# Basic helpers built in scripts/
PHONY += scripts_basic 
scripts_basic: 
	$(Q)$(MAKE) $(build)=scripts/basic
	$(Q)rm -f .tmp_quiet_recordmcount

config: scripts_basic FORCE
	$(Q)$(MAKE) $(build)=scripts/kconfig $@

%config: scripts_basic FORCE
	$(Q)$(MAKE) $(build)=scripts/kconfig $@

-include include/config/auto.conf
ifneq ($(CONFIG_CROSS_COMPILE),)
  CROSS_COMPILE=$(CONFIG_CROSS_COMPILE)
endif
include $(srctree)/arch/$(SRCARCH)/Makefile

export CONFIG_SHELL HOSTCC HOSTCXX HOSTCFLAGS HOSTCXXFLAGS
export KBUILD_BUILTIN KBUILD_MODULES
export KBUILD_CFLAGS KBUILD_AFLAGS

# Beautify output
# ---------------------------------------------------------------------------
#
# Normally, we echo the whole command before executing it. By making
# that echo $($(quiet)$(cmd)), we now have the possibility to set
# $(quiet) to choose other forms of output instead, e.g.
#
#         quiet_cmd_cc_o_c = Compiling $(RELDIR)/$@
#         cmd_cc_o_c       = $(CC) $(c_flags) -c -o $@ $<
#
# If $(quiet) is empty, the whole command will be printed.
# If it is set to "quiet_", only the short version will be printed. 
# If it is set to "silent_", nothing will be printed at all, since
# the variable $(silent_cmd_cc_o_c) doesn't exist.
#
# A simple variant is to prefix commands with $(Q) - that's useful
# for commands that shall be hidden in non-verbose mode.
#
#	$(Q)ln $@ :<
#
# If KBUILD_VERBOSE equals 0 then the above command will be hidden.
# If KBUILD_VERBOSE equals 1 then the above command is displayed.

ifeq ($(KBUILD_VERBOSE),1)
  quiet =
  Q =
else
  quiet=quiet_
  Q = @
endif
export quiet Q KBUILD_VERBOSE

export MODVERDIR := $(if $(KBUILD_EXTMOD),$(firstword $(appdir))/).tmp_versions

# We need some generic definitions (do not try to remake the file).
include $(srctree)/scripts/Kbuild.include

AS		= $(CROSS_COMPILE)as
LD		= $(CROSS_COMPILE)ld
CC		= $(CROSS_COMPILE)gcc
CPP		= $(CC) -E
AR		= $(CROSS_COMPILE)ar
NM		= $(CROSS_COMPILE)nm
STRIP		= $(CROSS_COMPILE)strip
OBJCOPY		= $(CROSS_COMPILE)objcopy
OBJDUMP		= $(CROSS_COMPILE)objdump
AWK		= awk
GENKSYMS	= scripts/genksyms/genksyms
INSTALLKERNEL  := installkernel
DEPMOD		= /sbin/depmod
PERL		= perl
CHECK		= sparse

export VERSION PATCHLEVEL SUBLEVEL KERNELRELEASE KERNELVERSION
export ARCH SRCARCH CONFIG_SHELL HOSTCC HOSTCFLAGS CROSS_COMPILE AS LD CC
export CPP AR NM STRIP OBJCOPY OBJDUMP
export MAKE AWK GENKSYMS INSTALLKERNEL PERL UTS_MACHINE
export HOSTCXX HOSTCXXFLAGS LDFLAGS_MODULE CHECK CHECKFLAGS KBUILD_CHECKSRC
export LDFLAGS
export LDFLAGS_etaos


PHONY += headers_install
# header install
export INSTALL_HDR_PATH = $(objtree)/usr
hdr-inst := -rR -f $(srctree)/scripts/Makefile.headersinst obj

__headers: scripts_basic
	$(Q)$(MAKE) $(build)=scripts build_unifdef

headers_install: __headers
	$(Q)$(MAKE) $(hdr-inst)=include

# module installation
PHONY += modules_install _modinst_
modules_install: _modinst_
_modinst_:
	$(Q)$(MAKE) -f $(srctree)/scripts/Makefile.modinst

MODLIB	= $(INSTALL_MOD_PATH)/etaos
export MODLIB

# building
core-y += kernel/ mm/ fs/
drivers-y += drivers/
libs-y += lib/
etaos-img := etaos.elf
etaos-target := etaos.img
export etaos-img

etaos-dirs	:= $(patsubst %/,%,$(filter %/, $(core-y) $(core-m) \
				$(drivers-y) $(drivers-m) $(libs-y) $(libs-m)))
etaos-alldirs	:= $(sort $(etaos-dirs) $(patsubst %/,%,$(filter %/, \
		     $(core-n) $(core-) $(drivers-n) $(drivers-) \
		     $(net-n)  $(net-)  $(libs-n)    $(libs-))))

core-y		:= $(patsubst %/, %/built-in.o, $(core-y))
drivers-y	:= $(patsubst %/, %/built-in.o, $(drivers-y))
libs-y		:= $(patsubst %/, %/built-in.o, $(libs-y))

etaos-deps := $(head-y) $(init-y) $(core-y) $(drivers-y) $(libs-y)
etaos-link := $(init-y) $(core-y) $(drivers-y) $(libs-y)
etaos-head := $(head-y)
export etaos-deps etaos-head

modules: prepare $(etaos-dirs)
etaos: prepare $(etaos-img)
app: $(etaos-target)
prepare: cremodverdir
	$(Q)$(MAKE) -f $(srctree)/Makefile silentoldconfig

cmd_crmodverdir = $(Q)mkdir -p $(MODVERDIR) \
                  $(if $(KBUILD_MODULES),; rm -f $(MODVERDIR)/*)
cremodverdir:
	$(cmd_crmodverdir)
PHONY += modules etaos cremodverdir app

PHONY += $(etaos-dirs) $(etaos-img)
$(etaos-dirs): scripts
	$(Q)$(MAKE) $(build)=$@

# build objects with decending
$(sort $(etaos-deps)): $(etaos-dirs) ;

quiet_cmd_link_etaos = LD      $@
cmd_link_etaos = $(LD) $(LDFLAGS) -r -o $@ $(etaos-link)
$(etaos-img): $(etaos-deps)
	$(call if_changed,link_etaos)

quiet_cmd_link_app = LD      $@
cmd_link_app = $(LD) $(LDFLAGS_etaos) -o $(etaos-target) $(etaos-img) \
	       $(app-img) $(ETAOS_LIBS) $(ETAOS_EXTRA_LIBS)
$(etaos-target):
	$(Q)$(MAKE) $(build)=$(appdir)
	$(Q)$(MAKE) $(link)=$(appdir)


quiet_cmd_install_etaos = INSTALL      etaos core
cmd_install_etaos = cp $(etaos-img) $(INSTALL_ETAOS_PATH)/$(etaos-img)

etaos_install: etaos.elf
	$(call if_changed,install_etaos)


PHONY += all
all: etaos modules

# mrproper - Delete all generated files, including .config
#

quiet_cmd_mrproper_dirs = CLEAN $(mrproper-rmdirs)
      cmd_mrproper_dirs = rm -rf $(mrproper-rmdirs)
mrproper-dirs      := $(addprefix _mrproper_,scripts)
mrproper-rmdirs += include/config include/generated

PHONY += $(mrproper-dirs) mrproper
$(mrproper-dirs):
	$(call cmd,mrproper_dirs)
	$(Q)$(MAKE) $(clean)=$(patsubst _mrproper_%,%,$@)

mrproper: clean $(mrproper-dirs)

quiet_cmd_rmdirs = $(if $(wildcard $(rm-dirs)),CLEAN   $(wildcard $(rm-dirs)))
      cmd_rmdirs = rm -rf $(rm-dirs)

quiet_cmd_rmfiles = $(if $(wildcard $(rm-files)),CLEAN   $(wildcard $(rm-files)))
      cmd_rmfiles = rm -f $(rm-files)

ifeq ($(KBUILD_EXTMOD),)
# Shorthand for $(Q)$(MAKE) -f scripts/Makefile.clean obj=dir
# Usage:
# $(Q)$(MAKE) $(clean)=dir
# clean - Delete most, but leave enough to build external modules
#
CLEAN_DIRS  += $(MODVERDIR)
CLEAN_FILES += etaos.elf etaos.img
clean		:= -f $(if $(KBUILD_SRC),$(srctree)/)scripts/Makefile.clean obj
clean: rm-dirs  := $(CLEAN_DIRS)
clean: rm-files := $(CLEAN_FILES)
clean-dirs      := $(addprefix _clean_, . $(etaos-alldirs))

PHONY += $(clean-dirs) clean

$(clean-dirs):
	$(Q)$(MAKE) $(clean)=$(patsubst _clean_%,%,$@)

clean: $(clean-dirs)
	$(call cmd,rmdirs)
	$(call cmd,rmfiles)
	@find $(if $(KBUILD_EXTMOD), $(KBUILD_EXTMOD), .) $(RCS_FIND_IGNORE) \
		\( -name '*.[oas]' -o -name '*.a' -o -name '.*.cmd' \
		-o -name '*.a.*' \
		-o -name '.*.d' -o -name '.*.tmp' -o -name '*.mod.c' \
		-o -name '*.symtypes' -o -name 'modules.order' \
		-o -name modules.builtin -o -name '.tmp_*.o.*' \
		-o -name '*.gcno' \) -type f -print | xargs rm -f
else
CLEAN_DIRS += $(MODVERDIR)
clean		:= -f $(if $(KBUILD_SRC),$(srctree)/)scripts/Makefile.clean obj
clean: rm-dirs  := $(CLEAN_DIRS)
clean-dirs      := $(addprefix _clean_, $(appdir))

$(clean-dirs):
	$(Q)$(MAKE) $(clean)=$(patsubst _clean_%,%,$@)

clean: $(clean-dirs)
	$(call cmd,rmdirs)
	@find $(appdir) $(RCS_FIND_IGNORE) \
		\( -name '*.[oas]' -o -name '*.a' -o -name '.*.cmd' \
		-o -name '*.a.*' \
		-o -name '.*.d' -o -name '.*.tmp' -o -name '*.mod.c' \
		-o -name '*.symtypes' -o -name 'modules.order' \
		-o -name modules.builtin -o -name '.tmp_*.o.*' \
		-o -name '*.gcno' \) -type f -print | xargs rm -f
endif

PHONY += FORCE
FORCE:



# Declare the contents of the .PHONY variable as phony.  We keep that
# information in a variable so we can use it in if_changed and friends.
.PHONY: $(PHONY)

