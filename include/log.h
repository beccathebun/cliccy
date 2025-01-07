/**
  @beccathebun: logging header i made :3
 */
#if !defined(_LOG_H_)
#define _LOG_H_
#include <stdio.h>
#include <time.h>
#include <stdarg.h>
#include "colour.h"

#ifndef FG_INFO
#define FG_INFO FG_256(105)
#endif
#ifndef FG_ERR
#define FG_ERR FG_256(198)
#endif
#ifndef FG_WARN
#define FG_WARN FG_256(220)
#endif
#ifndef FG_DEBUG
#define FG_DEBUG FG_256(213)
#endif
#ifndef FG_TRACE
#define FG_TRACE FG_256(222)
#endif
#ifndef FG_FATAL
#define FG_FATAL FG_256(196)
#endif

// Trace log level
// NOTE: Organized by priority level
typedef enum {
    Log_All = 0,        // Display all logs
    Log_Trace,          // Trace logging, intended for internal use only
    Log_Debug,          // Debug logging, used for internal debugging, it should be disabled on release builds
    Log_Info,           // Info logging, used for program execution info
    Log_Warn,        // Warning logging, used on recoverable failures
    Log_Error,          // Error logging, used on unrecoverable failures
    Log_Fatal,          // Fatal logging, used to abort program: exit(EXIT_FAILURE)
    Log_None            // Disable logging
} LoggerLogLevel;
/**
logger equivalent of vprintf

Define FG_INFO,FG_ERR,FG_WARN,and FG_DEBUG, FG_TEXT for custom colours
 */
extern void vlogs(int msgType, const char *text, va_list args);
/**
logger equivalent of printf

Define FG_INFO,FG_ERR,FG_WARN,and FG_DEBUG, FG_TEXT for custom colours
 */
extern void logs(int msgType, const char *text, ...);

// set minimum log level for logger
extern void set_log_level(int log_level);
static bool logger_print_time = true;
#endif // _LOG_H_
#if defined(LOGGER_IMPL)
static LoggerLogLevel __logger_log_lvl = Log_Info;

extern void vlogs(int msgType, const char *text, va_list args)
{
    if(msgType < __logger_log_lvl) return;
    
    if(logger_print_time) {
      char timeStr[64] = { 0 };
      time_t now = time(NULL);
      struct tm *tm_info = localtime(&now);
      strftime(timeStr, sizeof(timeStr), "%H:%M:%S", tm_info);
      printf(FG_256(255)"[%s] ", timeStr);
    }
    

    switch (msgType)
    {
        case Log_Trace:   printf(FG_TRACE"(trace)"); break;
        case Log_Debug:   printf(FG_DEBUG"(debug)"); break;
        case Log_Info:    printf(FG_INFO "(info)"); break;
        case Log_Warn:    printf(FG_WARN "(WARNING)"); break;
        case Log_Error:   printf(FG_ERR  "(ERROR)"); break;
        case Log_Fatal:   printf(FG_ERR  "(FATAL)"); break;
        default: break;
    }
    printf(FG_256(255)": ");
    vprintf(text, args);
    printf("\n");
}

extern void logs(int msgType, const char *text, ...) {
  va_list args;
  va_start(args, text);
  vlogs(msgType, text, args);
  va_end(args);
}

extern void set_log_level(int log_level){
  if(log_level <= Log_None && log_level >= 0)
    __logger_log_lvl = log_level;
}
#endif