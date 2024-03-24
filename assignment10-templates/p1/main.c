#include "testlib.h"
#include "ls.h"
#include <stdio.h>
#include <unistd.h>


void _printLine(size_t size, const char* name, const char *typestr)
{
	printf("%13lu  %s%s\n", size, name, typestr);
}


int main()
{
	test_start("ls.c");

	printf("All files in current directory:\n");
	printf("         size  name and type\n");
	test_equals_int(list(".", 0), 0);

	printf("\n");
	printf("All files in /home/sty24/A10/files, recursively:\n");
	printf("         size  name and type\n");
	test_equals_int(list("/home/sty24/A10/files", 1), 0);

	return test_end();
}
