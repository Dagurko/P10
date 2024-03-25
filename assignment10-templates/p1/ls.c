/* 
 * Group number (on canvas): xx
 * Student 1 name: Dagur Kári Ólafsson
 * Student 2 name: Jónas Ingi Þórisson
 */

#include "ls.h"

// You may not need all these headers ...
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>


int list(const char* path, int recursive)
{	
	DIR* dir = opendir(path);
	if (dir == NULL) {
		fprintf(stderr, "Error opening directory %s: %s\n", path, strerror(errno));
		return 1;
	}

	
	struct dirent* entry;
	while ((entry = readdir(dir)) != NULL) {
		if (entry->d_name[0] == '.' || strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

		char full_path[1024];
		snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);

		struct stat info;
		if (lstat(full_path, &info) < 0) {
        	perror("lstat");
        	exit(EXIT_FAILURE);
    	}

		char* typestr = "";
		if (S_ISDIR(info.st_mode)) {
			typestr = "/";
		} else if (S_ISFIFO(info.st_mode)) {
			typestr = "|";
		} else if (S_ISREG(info.st_mode) && (info.st_mode & (S_IXUSR))) {
			typestr = "*";
    	} else if (S_ISLNK(info.st_mode)) {
			typestr = "->";
		}
		_printLine(info.st_size, full_path, typestr);

		
		if (S_ISDIR(info.st_mode) && recursive) {
			list(full_path, recursive);
		}
		
	}

	closedir(dir);
	return 0;
}
