#pragma once

#define DEF_ERR(err_name, num, msg) LOG_##err_name = (num),

typedef enum log_error_t
{

    #include "defs.h"

} log_err_t;

#undef DEF_ERR


#ifdef __DEBUG__
#define PRINT_LOG       PrintLog("Error was detected! # line = %d", __LINE__)        //add line func?

#else
#define PRINT_LOG if(0) PrintLog("Error was detected! # line = %d", __LINE__)                             


#endif


#define CHECK_LOG(what, var, command)               \
do                                                  \
{                                                   \
    if ((var) == NULL)                              \
    {                                               \
        log_err = LOG_BAD##what;                    \
        PRINT_LOG;                                  \
        command;                                    \
    }                                               \
}                                                   \
while(0);                                       


#define LOG_OK(command)              \
do                                   \
{                                    \
    if (log_err != LOG_NOERR)        \
    {                                \
        PRINT_LOG;                   \
        command;                     \
    }                                \
}                                    \
while(0);


#define DO_FUNC_LOG(func)       \
do                              \
{                               \
    PrintLog(func);             \
}                               \
while(0);

extern log_error_t log_err;


void LogCtor();
void LogDtor();
void PrintLog(const char* msg, ...);
void PrintDetectedError();

#ifdef __GNUC__
	int print_log( char *msg, ... ) __attribute__(( format(printf, 1, 2) ));
#else
	int print_log( char *msg, ... );
#endif /*__GNUC__*/
