/*
 *  ETA/OS - Serial CAT
 *  Copyright (C) 2006   Swedish Institute of Computer Science
 *  Copyright (C) 2017   Michel Megens
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <err.h>

#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <termios.h>
#include <pthread.h>

#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <arpa/inet.h>
#include <netinet/in.h>

#define SLIP_END     0300
#define SLIP_ESC     0333
#define SLIP_ESC_END 0334
#define SLIP_ESC_ESC 0335

#ifndef BAUDRATE
#define BAUDRATE B115200
#endif

#if !defined(true) && !defined(false)
#define false 0
#define true !false
#endif

static speed_t b_rate = BAUDRATE;

static void err_exit(const int code, const char *fmt, ...)
{
	va_list va;

	va_start(va, fmt);
	vfprintf(stderr, fmt, va);
	va_end(va);
	exit(code);
}

static void stty_setup(int fd)
{
	struct termios tty;
	int i;

	if(tcflush(fd, TCIOFLUSH) == -1)
		err_exit(1, "tcflush");

	if(tcgetattr(fd, &tty) == -1)
		err_exit(1, "tcgetattr");

	cfmakeraw(&tty);

	tty.c_cc[VTIME] = 0;
	tty.c_cc[VMIN] = 1;
	tty.c_cflag &= ~CRTSCTS;
	tty.c_cflag &= ~HUPCL;
	tty.c_cflag &= ~CLOCAL;

	cfsetispeed(&tty, b_rate);
	cfsetospeed(&tty, b_rate);

	if(tcsetattr(fd, TCSAFLUSH, &tty) == -1)
		err_exit(1, "tcsetattr");

	tty.c_cflag |= CLOCAL;
	if(tcsetattr(fd, TCSAFLUSH, &tty) == -1)
		err_exit(1, "tcsetattr");

	i = TIOCM_DTR;
	if(ioctl(fd, TIOCMBIS, &i) == -1)
		err_exit(1, "ioctl");

	usleep(10*1000);		/* Wait for hardware 10ms. */

	if(tcflush(fd, TCIOFLUSH) == -1)
		err_exit(1, "tcflush");
}

static void stdin_setup(struct termios *old)
{
	struct termios new;

	tcgetattr(STDIN_FILENO, old);
	new = *old;
	new.c_lflag &= ~ECHO;
	tcsetattr(STDIN_FILENO, TCSANOW, &new);
}

static void *input_thread(void *arg)
{
	int fd, data, *_arg;
	struct termios old;

	_arg = arg;
	fd = *_arg;

	if(fd < 0)
		err_exit(-EXIT_FAILURE, "Invalid file descriptor: %i\n", fd);

	stdin_setup(&old);
	system("/bin/stty raw"); /* Beautiful ugliness isn't ugly, right? */
	while(true) {
		fread(&data, 1, 1, stdin);
		data &= 0xFF;

		if(data == 0x3 || data == 0x4) {
			tcsetattr(STDIN_FILENO, TCSANOW, &old);
			system("/bin/stty cooked");
			exit(-EXIT_SUCCESS);
		}

		write(fd, &data, 1);
	}

	pthread_exit(NULL);
}

int main(int argc, char **argv)
{
	int c;
	int slipfd;
	FILE *inslip;
	const char *siodev;
	int baudrate = -2;
	pthread_t input_thread_data;

	while ((c = getopt(argc, argv, "B:")) != -1) {
		switch (c) {
		case 'B':
			baudrate = atoi(optarg);
			break;

		case '?':
		case 'h':
		default:
			err_exit(1, "usage: scat [-B baudrate] device-file");
			break;
		}
	}

	argc -= (optind - 1);
	argv += (optind - 1);

	switch (baudrate) {
	case -2:
		break;
	case 9600:
		b_rate = B9600;
		break;
	case 19200:
		b_rate = B19200;
		break;
	case 38400:
		b_rate = B38400;
		break;
	case 57600:
		b_rate = B57600;
		break;
	case 115200:
		b_rate = B115200;
		break;
	default:
		err_exit(1, "unknown baudrate %d", baudrate);
		break;
	}

	if (argc != 2)
		err_exit(1, "usage: scat device-file");

	siodev = argv[1];
	setvbuf(stdout, NULL, _IOLBF, 0); /* Line buffered output. */

	slipfd = open(siodev, O_RDWR);
	if (slipfd == -1)
		err_exit(1, "can't open '%s'", siodev);

	stty_setup(slipfd);
	inslip = fdopen(slipfd, "rw");
	if(inslip == NULL)
		err_exit(1, "main: fdopen");

	if(pthread_create(&input_thread_data, NULL, input_thread, &slipfd)) {
		fprintf(stderr, "Couldn't create input thread!\n");
		return -EXIT_FAILURE;
	}

	while (1) {
		int c = getc(inslip);
		while (c == SLIP_END)
			c = getc(inslip);
		do {
			if (c == SLIP_ESC) {
				c = getc(inslip);
				if (c == SLIP_ESC_ESC)
					c = SLIP_ESC;
				else if (c == SLIP_ESC_END)
					c = SLIP_END;
			}

			switch (c) {
			case EOF:
				err_exit(1, "getc(inslip)");
				break;

			case '\007':
			case '\b':
			case '\f':
			case '\n':
			case '\r':
			case '\t':
			case '\v':
				putchar(c);
				break;

			default:
			if (isprint(c))
				putchar(c);
			else
				printf("%02x ", c);
			break;
			}
			c = getc(inslip);
		} while (c != SLIP_END);
	}

	return -EXIT_SUCCESS;
}
