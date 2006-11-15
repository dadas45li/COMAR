/*
** Copyright (c) 2006, TUBITAK/UEKAE
**
** This program is free software; you can redistribute it and/or modify it
** under the terms of the GNU General Public License as published by the
** Free Software Foundation; either version 2 of the License, or (at your
** option) any later version. Please read the COPYING file.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>

#include "common.h"

int
devnode_mknod(const char *name, const char *major, const char *minor)
{
	char buf[512];

	sprintf(buf, "mknod /dev/%s b %s %s", name, major, minor);
	if (cfg_debug)
		puts(buf);
	else
		system(buf);
	return 0;
}

static int
mknod_parts(char *dev)
{
	char *path;
	DIR *dir;
	struct dirent *dirent;
	char *tmp;
	char *major;
	char *minor;

	path = concat("/sys/block/", dev);
	dir = opendir(path);
	if (!dir) return -1;
	while((dirent = readdir(dir))) {
		char *name = dirent->d_name;
		if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0 || strlen(name) < 3)
			continue;
		if (strncmp(name, dev, strlen(dev)) != 0)
			continue;
		tmp = concat(concat(path, "/"), name);
		tmp = sys_value(tmp, "dev");
		major = strtok(tmp, ":");
		minor = strtok(NULL, "");
		if (minor) devnode_mknod(name, major, minor);
	}
	closedir(dir);
	return 0;
}

int
devnode_populate(void)
{
	DIR *dir;
	struct dirent *dirent;

	dir = opendir("/sys/block");
	if (!dir) return -1;
	while((dirent = readdir(dir))) {
		char *path;
		char *tmp;
		char *dev;
		char *major;
		char *minor;
		char *name = dirent->d_name;
		if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0 || strlen(name) < 3)
			continue;
		tmp = NULL;
		if (name[0] == 'h' && name[1] == 'd')
			tmp = name;
		if (name[0] == 's' && name[1] == 'd')
			tmp = name;
		if (name[0] == 's' && name[1] == 'r')
			tmp = name;
		if (tmp) {
			path = concat("/sys/block/", tmp);
			dev = sys_value(path, "dev");
			major = strtok(dev, ":");
			minor = strtok(NULL, "");
			if (minor) {
				devnode_mknod(name, major, minor);
				mknod_parts(tmp);
			}
		}
	}
	closedir(dir);

	return 0;
}
