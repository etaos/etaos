libc-files-$(CONFIG_CRT) += memcpy.o memcmp.o rand.o
libc-files-$(CONFIG_CRT) += stricmp.o
libc-files-$(CONFIG_CRT) += strtol.o ctype.o atoi.o
libc-files-$(CONFIG_CRT) += abs.o labs.o
libc-files-$(CONFIG_CRT) += itoa.o ltoa.o
libc-files-$(CONFIG_CRT) += kmalloc.o kzalloc.o kfree.o kcalloc.o
libc-files-$(CONFIG_CRT) += krealloc.o

libc-files-$(CONFIG_CRT) += gmtime.o localtime.o mktime.o

libc-files-$(CONFIG_CRT) += putc.o printf.o vfprintf.o snprintf.o
libc-files-$(CONFIG_CRT) += fputs.o fputc.o fprintf.o
libc-files-$(CONFIG_CRT) += read.o write.o fgets.o
libc-files-$(CONFIG_CRT) += ioctl.o getc.o fgetc.o
libc-files-$(CONFIG_CRT) += strncpy_P.o puts.o puts_P.o
libc-files-$(CONFIG_CRT) += open.o close.o

libc-files-$(CONFIG_CRT) += strlen.o strchr.o memchr.o
libc-files-$(CONFIG_CRT) += strcmp.o strnlen.o 
libc-files-$(CONFIG_CRT) += memset.o strcat.o strdup.o
libc-files-$(CONFIG_CRT) += strcpy.o strncpy.o strtok.o

libc-files-$(CONFIG_CRT) += unlink.o mount.o mkdir.o ftell.o lseek.o
libc-files-$(CONFIG_CRT) += opendir.o readdir.o readdir_r.o closedir.o

libc-files-$(CONFIG_CRT) += fmode.o fopen.o fclose.o fread.o fwrite.o
libc-files-$(CONFIG_CRT) += flush.o fflush.o ferror.o feof.o fileno.o

libc-files-$(CONFIG_HARVARD) += printf_p.o vfprintf_p.o
libc-files-$(CONFIG_HARVARD) += fprintf_P.o

c-y += $(libc-files-y) $(libc-files-m)
obj-$(CONFIG_CRT) += c.o
