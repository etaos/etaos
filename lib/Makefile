libc-files-$(CONFIG_STDLIB) += stdlib/memcpy.o stdlib/memcmp.o stdlib/rand.o
libc-files-$(CONFIG_STDLIB) += kmalloc.o kzalloc.o kfree.o kcalloc.o

libc-files-$(CONFIG_STDIO) += stdio/putc.o stdio/printf.o stdio/vfprintf.o
libc-files-$(CONFIG_STDIO) += stdio/fputs.o stdio/fputc.o stdio/fprintf.o
libc-files-$(CONFIG_STDIO) += stdio/read.o stdio/write.o
libc-files-$(CONFIG_STDIO) += stdio/ioctl.o stdio/getc.o stdio/fgetc.o

libc-files-$(CONFIG_STRING) += string/strlen.o string/strchr.o
libc-files-$(CONFIG_STRING) += string/strcmp.o string/strnlen.o 
libc-files-$(CONFIG_STRING) += string/memset.o

obj-$(CONFIG_LIBC) += c.o

c-y += $(libc-files-y) $(libc-files-m)

