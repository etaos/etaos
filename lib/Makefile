libc-files-$(CONFIG_STDLIB) += stdlib/memcpy.o stdlib/memcmp.o stdlib/rand.o
libc-files-$(CONFIG_STDLIB) += stdlib/strtol.o stdlib/ctype.o stdlib/atoi.o
libc-files-$(CONFIG_STDLIB) += stdlib/abs.o stdlib/labs.o
libc-files-$(CONFIG_STDLIB) += kmalloc.o kzalloc.o kfree.o kcalloc.o
libc-files-$(CONFIG_STDLIB) += time/gmtime.o time/localtime.o

libc-files-$(CONFIG_STDIO) += stdio/putc.o stdio/printf.o stdio/vfprintf.o
libc-files-$(CONFIG_STDIO) += stdio/fputs.o stdio/fputc.o stdio/fprintf.o
libc-files-$(CONFIG_STDIO) += stdio/read.o stdio/write.o
libc-files-$(CONFIG_STDIO) += stdio/ioctl.o stdio/getc.o stdio/fgetc.o

libc-files-$(CONFIG_STRING) += string/strlen.o string/strchr.o
libc-files-$(CONFIG_STRING) += string/strcmp.o string/strnlen.o 
libc-files-$(CONFIG_STRING) += string/memset.o string/strcat.o string/strdup.o

libc-files-$(CONFIG_HARVARD) += stdio/printf_p.o stdio/vfprintf_p.o

obj-$(CONFIG_LIBC) += c.o

c-y += $(libc-files-y) $(libc-files-m)

