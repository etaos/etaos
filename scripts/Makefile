# Let clean descend into subdirs
subdir-	+= basic kconfig

subdir-$(CONFIG_ROMFS) += crurom

always		:= $(hostprogs-y) $(hostprogs-m)

# The following hostprogs-y programs are only build on demand
hostprogs-y += unifdef

# These targets are used internally to avoid "is up to date" messages
PHONY += build_unifdef
build_unifdef: $(obj)/unifdef
	@:

