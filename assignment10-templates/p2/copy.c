/* 
 * Group number (on canvas): 109
 * Student 1 name: Jonas Ingi Þórisson 
 * Student 2 name: Dagur Kári Ólafsson
 */

#define _POSIX_C_SOURCE 2
#include "copy.h"
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>

// No need to change this. Parses argc into the CopyArgs structure
int parseCopyArgs(int argc, char * const argv[], CopyArgs* args)
{
	if (args == NULL) {
		return -1;
	}

	// Initialize with default values
	args->blocksize  = 4096;

	int opt;
	while ((opt = getopt(argc, argv, "b:")) != -1) {
		switch (opt) {
			case 'b':
				args->blocksize = atoi(optarg);

				if ((errno != 0) || (args->blocksize <= 0)) {
					return -1; // Invalid blocksize argument.
				}

				break;

			default: /* '?' */
				return -1;
		}
	}

	if (optind != argc - 2) {
		return -1; // We expect two parameters after the options.
	}

	args->from = argv[optind];
	args->to   = argv[optind + 1];

	return 0;
}


int doCopy(CopyArgs* args)
{
	if (args == NULL) {
		return -1;
	}

    int src_fd = open(args->from, O_RDONLY);
    if (src_fd == -1) {
        perror("Failed to open source file");
        return -1;
    }

    if (access(args->to, F_OK) == 0) {
        fprintf(stderr, "Destination file already exists\n");
        close(src_fd);
        return -1;
    }

    int dest_fd = open(args->to, O_WRONLY | O_CREAT | O_EXCL, 0666);
    if (dest_fd == -1) {
        perror("Failed to create destination file");
        close(src_fd);
        return -1;
    }

    char buffer[args->blocksize];

    ssize_t bytes_read, bytes_written;
    off_t offset = 0;

    while ((bytes_read = read(src_fd, buffer, args->blocksize)) > 0) {
        int all_zeros = 1;
        for (ssize_t i = 0; i < bytes_read; ++i) {
            if (buffer[i] != 0) {
                all_zeros = 0;
                break;
            }
        }

        if (!all_zeros) {
            bytes_written = write(dest_fd, buffer, bytes_read);
            if (bytes_written == -1) {
                perror("Failed to write to destination file");
                close(src_fd);
                close(dest_fd);
                return -1;
            offset += bytes_written;
            if (lseek(dest_fd, offset, SEEK_SET) == -1) {
                perror("Failed to seek in destination file");
                close(src_fd);
                close(dest_fd);
                return -1;
            }
        } else {
            offset += bytes_read;
            if (lseek(dest_fd, offset, SEEK_SET) == -1) {
                perror("Failed to seek in destination file");
                close(src_fd);
                close(dest_fd);
                return -1;
            }
        }
    }

    if (bytes_read == -1) {
        perror("Error reading from source file");
        close(src_fd);
        close(dest_fd);
        return -1;
    }

    // Retrieve source file permissions
    struct stat statbuf;
    if (stat(args->from, &statbuf) == -1) {
        perror("Failed to get source file permissions");
        close(src_fd);
        close(dest_fd);
        return -1;
    }

    // Set destination file permissions
    if (fchmod(dest_fd, statbuf.st_mode) == -1) {
        perror("Failed to set destination file permissions");
        close(src_fd);
        close(dest_fd);
        return -1;
    }

    // Close files
    close(src_fd);
    close(dest_fd);

    return 0;
}
