#ifndef ANNA_LOG_UTILS_H
#define ANNA_LOG_UTILS_H

#include "anna-layer.h"
#include <stdio.h>

static void log_human_readable_size(unsigned long size)
{
	unsigned int count = 0;
	double working_size = (double) size;
	while (working_size > (double)1000)
	{
		working_size /= (double)1000;
		count++;
	}
	char size_designators[5][3] = {"B", "KB", "MB", "GB", "TB"};
	printf("%.3f%s\n", working_size, size_designators[count]);
	return;
}

#endif
