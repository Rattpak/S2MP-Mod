#include "string.h"
#include <cstdarg>
#include <stdio.h>
#include <gzguts.h>

const char* string::va(const char* fmt, ...)
{
	va_list argList;
	va_start(argList, fmt);

	char temp[500];

	vsnprintf(temp, sizeof(temp), fmt, argList);
	va_end(argList);

	char* buffer = (char*)malloc(strlen(temp) + 1);
	if (buffer) {
		strcpy(buffer, temp);
	}

	return buffer;
}