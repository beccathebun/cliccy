#ifndef CLICCY_H
#define CLICCY_H

#include <stddef.h>
#include <stdint.h>
#include <time.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#define NOB_STRIP_PREFIX
#define NOB_IMPLEMENTATION
#define NOB_CUSTOM_LOG
#include <nob.h>

#include <raylib.h>
#include <resources/font/PixelifySans.h>
#include <time.h>
#include <stdlib.h>
#define TOML_IMPLEMENTATION
#include <toml-c.h>
#define CLAY_IMPLEMENTATION
#include <clay.h>
#include <clay_renderer_raylib.c>
#include "ui.c"
#define LOGGER_IMPL
#include <log.h>
inline void nob_log(Nob_Log_Level level, const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  vlogs(level+3, fmt, args);
  va_end(args);
}
#if defined(_WIN32)
//# include <Shlobj.h>
# include <wintoastlibc.h>
# include <float.h>
# define URL_OPEN "open"
# define sleep Sleep
# define PATHSEP "\\"

// --------------------------------------------------
// stolen from:
// musl/src/string/stpcpy.c
#define ALIGN (sizeof(size_t))
#define ONES ((size_t)-1/UCHAR_MAX)
#define HIGHS (ONES * (UCHAR_MAX/2+1))
#define HASZERO(x) ((x)-ONES & ~(x) & HIGHS)
char *stpcpy(char *restrict d, const char *restrict s)
{
#ifdef __GNUC__
	typedef size_t __attribute__((__may_alias__)) word;
	word *wd;
	const word *ws;
	if ((uintptr_t)s % ALIGN == (uintptr_t)d % ALIGN) {
		for (; (uintptr_t)s % ALIGN; s++, d++)
			if (!(*d=*s)) return d;
		wd=(void *)d; ws=(const void *)s;
		for (; !HASZERO(*ws); *wd++ = *ws++);
		d=(void *)wd; s=(const void *)ws;
	}
#endif
	for (; (*d=*s); s++, d++);

	return d;
}
// --------------------------------------------------
#elif defined(__linux__)
#include "glib-object.h"
#include "glib.h"
#include <libnotify/notification.h>
#include <libnotify/notify.h>
# define URL_OPEN "xdg-open"
# define PATHSEP "/"
#endif // _WIN32, __linux__
#define _CALLOC CALLOC
//must be included after stpcpy is defined for windows :'3
#include "util.c"
typedef bool (*dispatcher)(void);

// TODO: get rid of this shit it makes no sense
typedef enum {
  Res_Success = 0,
  Res_Fail_Gen,
  Res_Fail_Libnotify,
  Res_Fail_Nvd,
  Res_Fail_Crash
} Result;

typedef enum {
  CA_Link = 0,
  CA_Notif,
  CA_Dialog,
  CA_Question,
  CA_Lines,
  CA_COUNT
} Clickslut_Action;

typedef enum {
  Qs_First = 0,
  Qs_Bad,
  Qs_Done,
} Question_Stage;

typedef struct {
  const char *text;
  char       *yes;
  char       *no;
  char       *wrong;
  Question_Stage stage;
} QuestionConfig;

typedef struct {
  const char *line;
  size_t attempts;
  size_t fails;
  size_t target;
  bool show_fail;
  time_t close_fail;
  bool done;
  time_t close;
} LinesCfg;


typedef struct {
  int64_t minimum;
  int64_t maximum;
} TimerConfig;

typedef struct {
  size_t capacity;
  size_t count;
  union {
    char **items;
    char *items_arr[10];
  };
  bool static_arr;
} CliccyStrs;

typedef struct {
  bool lines;
  bool links;
  bool notifs;
  bool popups;
} FeatConfig;

typedef struct {
  char *icon;
} NotifConfig;

typedef struct {
  size_t minimum;
  size_t maximum;
  size_t penalty_min;
  size_t penalty_max;
} LinesConfig;

typedef struct config_cfg_t {
  FeatConfig  feat;
  TimerConfig timer;
  NotifConfig notifs;
  LinesConfig lines;
} config_cfg_t;

typedef struct config_data_t {
  char      *title;
  CliccyStrs lines;
  CliccyStrs petnames;
  CliccyStrs links;
  
} config_data_t;

#endif // CLICCY_H