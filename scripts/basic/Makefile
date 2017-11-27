###
# Makefile.basic lists the most basic programs used during the build process.
# The programs listed herein are what are needed to do the basic stuff,
# such as fix file dependencies.
# This initial step is needed to avoid files to be recompiled
# when kernel configuration changes (which is what happens when
# .config is included by main Makefile.
# ---------------------------------------------------------------------------
# fixdep: 	 Used to generate dependency information during build process

hostprogs-y	:= fixdep

fixdep-objs += fixdependencies.o mmap.o

# fixdep is needed to compile other host programs
$(addprefix $(obj)/,$(filter-out fixdep,$(always))): $(obj)/fixdep

smon: $(obj)/scat
	$< -B $(BAUD) $(TTYFILE)

ifeq ($(MAKECMDGOALS),smon)
HOSTCFLAGS += -lpthread
hostprogs-y += scat
endif

clean-files += scat
always		:= $(hostprogs-y)
