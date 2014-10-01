libc-files-$(CONFIG_STDLIB) += stdlib/memcpy.o stdlib/memcmp.o

libc-files-$(CONFIG_STDIO) += stdio/putc.o stdio/printf.o stdio/vfprintf.o
libc-files-$(CONFIG_STDIO) += stdio/fputs.o stdio/fputc.o

libc-files-$(CONFIG_STRING) += string/strlen.o string/strchr.o
libc-files-$(CONFIG_STRING) += string/strcmp.o string/strnlen.o

obj-$(CONFIG_LIBC) += c.o

c-y += $(libc-files-y) $(libc-files-m)

