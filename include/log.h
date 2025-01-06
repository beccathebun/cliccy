/**
  @beccathebun: logging header i made :3
 */
#if !defined(_LOG_H_)
#define _LOG_H_
#include <stdio.h>
#include <time.h>
#include <stdarg.h>
#define T_RESET "\x1b[0m"
#define FG_BLK        "\x1b[30m"
#define FG_RED        "\x1b[31m"
#define FG_GRN        "\x1b[32m"
#define FG_YLW        "\x1b[33m"
#define FG_BLU        "\x1b[34m"
#define FG_MAG        "\x1b[35m"
#define FG_CYN        "\x1b[36m"
#define FG_WHT        "\x1b[37m"
#define FG_DEF        "\x1b[39m"
#define FG_BBLK       "\x1b[90m"
#define FG_BRED       "\x1b[91m"
#define FG_BGRN       "\x1b[92m"
#define FG_BYLW       "\x1b[93m"
#define FG_BBLU       "\x1b[94m"
#define FG_BMAG       "\x1b[95m"
#define FG_BCYN       "\x1b[96m"
#define FG_BWHT       "\x1b[97m"
#define FG_256(id)    "\x1b[38;5;" #id "m"
#define FG_RGB(r,g,b) "\x1b[38;2;" #r ";" #g ";" #b "m"

#define BG_BLK        "\x1b[40m"
#define BG_RED        "\x1b[41m"
#define BG_GRN        "\x1b[42m"
#define BG_YLW        "\x1b[43m"
#define BG_BLU        "\x1b[44m"
#define BG_MAG        "\x1b[45m"
#define BG_CYN        "\x1b[46m"
#define BG_WHT        "\x1b[47m"
#define BG_DEF        "\x1b[49m"
#define BG_BBLK       "\x1b[100m"
#define BG_BRED       "\x1b[101m"
#define BG_BGRN       "\x1b[102m"
#define BG_BYLW       "\x1b[103m"
#define BG_BBLU       "\x1b[104m"
#define BG_BMAG       "\x1b[105m"
#define BG_BCYN       "\x1b[106m"
#define BG_BWHT       "\x1b[107m"
#define BG_256(id)    "\x1b[48;5;" #id "m"
#define BG_RGB(r,g,b) "\x1b[48;2;" #r ";" #g ";" #b "m"

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
#endif // _LOG_H_
#if defined(LOGGER_IMPL)
static LoggerLogLevel __logger_log_lvl = Log_Info;

extern void vlogs(int msgType, const char *text, va_list args)
{
    if(msgType < __logger_log_lvl) return;
    char timeStr[64] = { 0 };
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);

    strftime(timeStr, sizeof(timeStr), "%H:%M:%S", tm_info);
    printf(FG_256(255)"[%s] ", timeStr);

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