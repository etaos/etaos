libc-files-$(CONFIG_CRT) += stdlib/memcpy.o stdlib/memcmp.o stdlib/rand.o
libc-files-$(CONFIG_CRT) += stdlib/strtol.o stdlib/ctype.o stdlib/atoi.o
libc-files-$(CONFIG_CRT) += stdlib/abs.o stdlib/labs.o
libc-files-$(CONFIG_CRT) += stdlib/itoa.o stdlib/ltoa.o
libc-files-$(CONFIG_CRT) += kmalloc.o kzalloc.o kfree.o kcalloc.o

libc-files-$(CONFIG_CRT) += time/gmtime.o time/localtime.o time/mktime.o

libc-files-$(CONFIG_CRT) += stdio/putc.o stdio/printf.o stdio/vfprintf.o
libc-files-$(CONFIG_CRT) += stdio/fputs.o stdio/fputc.o stdio/fprintf.o
libc-files-$(CONFIG_CRT) += stdio/read.o stdio/write.o stdio/fgets.o
libc-files-$(CONFIG_CRT) += stdio/ioctl.o stdio/getc.o stdio/fgetc.o

libc-files-$(CONFIG_CRT) += string/strlen.o string/strchr.o
libc-files-$(CONFIG_CRT) += string/strcmp.o string/strnlen.o 
libc-files-$(CONFIG_CRT) += string/memset.o string/strcat.o string/strdup.o

libc-files-$(CONFIG_HARVARD) += stdio/printf_p.o stdio/vfprintf_p.o
libc-files-$(CONFIG_HARVARD) += stdio/fprintf_P.o

obj-$(CONFIG_CRT) += c.o
c-y += $(libc-files-y) $(libc-files-m)
obj-$(CONFIG_LIBETAOS) += etaos/

