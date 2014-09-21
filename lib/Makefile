libc-files-$(CONFIG_STDLIB) += stdlib/memcpy.o stdlib/memcmp.o
libc-files-$(CONFIG_STDIO) += stdio/putc.o stdio/printf.o stdio/vfprintf.o
libc-files-$(CONFIG_STDIO) += stdio/fputs.o stdio/fputc.o

obj-$(CONFIG_LIBC) += libc.o

libc-y += $(libc-files-y) $(libc-files-m)

