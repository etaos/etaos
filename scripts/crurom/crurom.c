/*
 *  ETA/OS - CRUROM tool
 *  Copyright (C) 2014, 2015   Michel Megens, ETHERNUT
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

#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <dirent.h>

#ifndef O_BINARY
#define O_BINARY 0
#endif

#define IDENT   "crurom"

#ifdef VERSION
#undef VERSION
#endif

#define VERSION "1.1.0"

#define ROMENTRY "struct romfs"
#define PROG_CHAR "const char __pgm"

static int entryno = 0;
static int verbose = 0;
static int recursive = 0;
static char rootdir[256];
static int rootlen = 0;
static char outname[256];
static FILE *fpout;

static int dofile(char *name)
{
	int rc = -EXIT_SUCCESS;
	int fd;
	unsigned char buf[512];
	int i;
	int cnt;
	long total = 0;
	char *fsname = name;

	if(strncasecmp(fsname, rootdir, rootlen) == 0)
		fsname += rootlen;

	if((fd = open(name, O_RDONLY | O_BINARY)) == -1) {
		perror(name);
		return -1;
	}

	if(verbose)
		fprintf(stderr, IDENT ": Reading %s\n", name);

	for(;;) {
		if((cnt = read(fd, buf, sizeof(buf))) < 0) {
			perror(name);
			rc = -EXIT_FAILURE;
			total = 0;
			break;
		}
 
		if(total == 0) {
			entryno++;
			fprintf(fpout, "/*\n * File entry %d: %s\n */\n", 
				entryno, fsname);
			fprintf(fpout, "static " PROG_CHAR " file%ddata[] = {", entryno);
		}

		if(cnt == 0)
			break;
		
		for(i = 0; i < cnt; i++) {
			if((i % 16) == 0) {
				if(total != 0 || i != 0) {
					fputc(',', fpout);
				}
				fputs("\n ", fpout);
			} else {
				fputc(',', fpout);
			}
			if (buf[i] < 32 || buf[i] > 127 || buf[i] == '\'' || 
				buf[i] == '\\') {
				if(buf[i] == '\n' && (i+1) == cnt)
					fprintf(fpout, "4"); /* insert EOF */
				else
					fprintf(fpout, "%3u", buf[i]);
			} else {
				fprintf(fpout, "'%c'", buf[i]);
			}
		}
		total += cnt;
	}
	close(fd);

	fprintf(fpout, "\n};\n\n");
	fprintf(fpout, "static const char file%dname[] = \"%s\";\n\n", entryno, fsname);
	fprintf(fpout, "static " ROMENTRY " file%dentry = { ", entryno);

	if(entryno > 1)
		fprintf(fpout, "&file%dentry, ", entryno - 1);
	else
		fprintf(fpout, "0, ");

	fprintf(fpout, "(const char *)file%dname, %ld, (" PROG_CHAR " *)file%ddata };\n", 
		entryno, total, entryno);

	return rc;
}

static int dodir(char *dirpath)
{
	int rc = -EXIT_SUCCESS;
	char path[256];
	DIR *dir;
	struct dirent *dire;
	struct stat statbuf;

	if((dir = opendir(dirpath)) == NULL) {
		fprintf(stderr, "Failed to scan directory %s\n", dirpath);
		return -1;
	}

	if(verbose)
		fprintf(stderr, "Scan %s\n", dirpath);
    
	while((dire = readdir(dir)) != NULL && rc == -EXIT_SUCCESS) {
		if((dire->d_name[0] == '.') || 
			(strcasecmp(dire->d_name, "cvs") == 0) || 
			(strcasecmp(dire->d_name, "svn") == 0))
			continue;
        
		strcpy(path, dirpath);
		strcat(path, "/");
		strcat(path, dire->d_name);
		stat(path, &statbuf);

		if(statbuf.st_mode & S_IFDIR)
			rc = dodir(path);
		else if(statbuf.st_mode & S_IFREG)
			rc = dofile(path);
	}

	closedir(dir);
	return rc;
}

static void usage_short(FILE *output)
{
	fputs("Usage: crurom OPTIONS DIRECTORY\n"
	"OPTIONS:\n"
	"-o <file>  output file\n"
	"-r         recursive\n"
	"-v         verbose\n"
	"-h         display this help text\n"
	, output);
}

static void usage_long(FILE *output)
{
	fputs("General\n"
	"crurom is a tool to generate a basic read-only file system for HARVARD\n"
	"processors.\n\n"

	"Output\n"
	"crurom will generate a source file, which for obvious reasons has to\n"
	"be included in the build system of the application, containing the\n"
	"contents of one or more directories. The output source code is generated\n"
	"in the C programming language.\n\n"

	"Input\n"
	"As said before the content of one or more directories is read into a\n"
	"buffer by crurom and written into a source file systematically, such\n"
	"the ETA/OS romfs driver can interpret it. In order to do so, input has\n"
	"to be given to crurom. Therefore the final arguments of to an invocation\n"
	"of crurom are the direcory('s) it has to consider. An example of a\n"
	"single directory and multiple directory invocation:\n\n"

	"Single:\n"
	"./crurom -o data.c directory-name\n\n"
	"Multiple:\n"
	"./crurom -o data.c directory1 directory2 .. directoryN\n", output);
}

int main(int argc, char **argv)
{
	int option;
	int i;
	int rc = -EXIT_SUCCESS;

	while((option = getopt(argc, argv, "o:rvh?")) != EOF) {
		switch(option) {
		case 'o':
			strcpy(outname, optarg);
			break;
		case 'r':
			recursive++;
			break;
		case 'v':
			verbose++;
			break;
		case 'h':
			usage_long(stdout);
			fputc('\n', stdout);
			usage_short(stdout);
			return rc;

		default:
			usage_short(stderr);
			return 1;
		}
	}

	argc -= optind;
	argv += optind;

	if(outname[0]) {
		if((fpout = fopen(outname, "w")) == NULL) {
			perror(outname);
			return 3;
		}
	} else {
		fpout = stdout;
	}

	fprintf(fpout, "/*\n");
	fprintf(fpout, " * This file is automatically created by " IDENT " " VERSION "\n");
	fprintf(fpout, " */\n\n");

	fputs("#include <etaos/kernel.h>\n", fpout);
	fputs("#include <etaos/types.h>\n", fpout);
	fputs("#include <etaos/romfs.h>\n\n", fpout);

	if(argc) {
		for(i = 0; i < argc && rc == -EXIT_SUCCESS; i++) {
			strcpy(rootdir, argv[i]);
			strcat(rootdir, "/");
			rootlen = strlen(rootdir);
			rc = dodir(argv[i]);
		}
	} else {
		strcpy(rootdir, "./");
		rootlen = 2;
		rc = dodir(".");
	}

	fprintf(fpout, "\n" ROMENTRY " *romEntryList = &file%dentry;\n", entryno);

	if(fpout != stdout)
		fclose(fpout);

	return rc;
}

