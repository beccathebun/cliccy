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
#include <nob.h>
#include <raylib.h>
#include <time.h>
#include <stdlib.h>

#define TOML_IMPLEMENTATION
#include <toml-c.h>

#define _CALLOC CALLOC
#include "util.c"

#define CLAY_IMPLEMENTATION
#include <clay.h>
#include <clay_renderer_raylib.c>
#include "ui.c"
#define LOGGER_IMPL
#include <log.h>
#if defined(DEBUG) && !defined(_WIN32)
# define PPRINT_IMPLEMENTATION
# include <pprint.h>
#endif //DEBUG
#if defined(_WIN32)
# include <Shlobj.h>
# include <EasyWinNotyCWrapper.h>
# define URL_OPEN "open"
# define sleep Sleep
# define PATHSEP "\\"
#elif defined(__linux__)
#include "glib-object.h"
#include "glib.h"
#include <libnotify/notification.h>
#include <libnotify/notify.h>
# define URL_OPEN "xdg-open"
# define PATHSEP "/"
#endif // _WIN32, __linux__
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
  char **items;
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