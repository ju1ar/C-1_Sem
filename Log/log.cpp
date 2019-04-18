#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "log.h"


log_error_t log_err = LOG_NOERR;

FILE* log = NULL;
const char* log_name = "log.txt";

void LogCtor()
{
    log = fopen("log.txt", "w");

    if (!log) fprintf(stderr, "Error: file %s is not created\n", log_name);
}

void LogDtor()
{
    fclose(log);
}

void PrintLog(const char* msg, ...)
{
    fprintf(stderr, "Check log file to find something interesting!:)\n");

	va_list args;
	va_start(args, msg);
	vfprintf(log, msg, args);
	va_end(args);

    PrintDetectedError();
}

#define DEF_ERR(err_name, num, msg)     \
{                                       \
    case LOG_##err_name :               \
       fprintf(log, "%s", msg);       \
       break;                           \
}                                       
// funny error

void PrintDetectedError()
{

    fprintf(log, " ;");

    switch(log_err)
    {
        #include "defs.h"

        default:
            fprintf(log, "Unexpected error  = %d!", log_err);
    }
}

#undef DEF_ERR
