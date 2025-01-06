#include <stdio.h>
#include <string.h>
#define NOB_IMPLEMENTATION
#define NOB_STRIP_PREFIX
#define NOB_CUSTOM_LOG
#include "include/nob.h"
#define LOGGER_IMPL
#include "include/log.h"
void nob_log(Nob_Log_Level level, const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  vlogs(level+3, fmt, args);
  va_end(args);
}
struct conf_t {
  bool windows;
  bool test;
  bool debug;
  bool run;
  bool rl;
  bool lines;
  bool question;
};

#define CFLAGS "-std=c23","-D_DEFAULT_SOURCE","-Wno-missing-braces","-Wno-unused-value","-Wno-pointer-sign", "-ggdb"
#define INCLUDES "-Iinclude", "-I.","-Iexternal","-I/usr/include/gdk-pixbuf-2.0","-I/usr/include/glib-2.0","-I/usr/lib64/glib-2.0/include","-I/usr/include/libpng16","-I/usr/include/webp","-DWITH_GZFILEOP","-I/usr/include/libmount","-I/usr/include/blkid","-I/usr/include/sysprof-6","-pthread"


#ifdef _WIN32
# define PATHSEP "\\"
# define CC "clang-cl"
# define CC_WIN CC
# define LDLIBS "-lraylib","-lopengl32","-lgdi32","-lwinmm","-lcomdlg32","-lole32","-static", "-lpthread"
# define LDLIBS_WIN LDLIBS
static struct conf_t conf = {.windows=true};
#elif  __linux__
# define PATHSEP "/"
# define CC "gcc"
# define CC_WIN "x86_64-w64-mingw32-gcc"
# ifdef USE_WAYLAND_DISPLAY
#   define LDLIBS "-Lraylib/src","-lraylib","-lGL","-lm","-lpthread","-ldl","-lrt","-lnotify","-lgdk_pixbuf-2.0","-lgio-2.0","-lgobject-2.0","-lglib-2.0", "-lwayland-client","-lwayland-cursor","-lwayland-egl","-lxkbcommon"
#   else
#   define LDLIBS "-Lraylib/src","-lraylib","-lGL","-lm","-lpthread","-ldl","-lrt","-lnotify","-lgdk_pixbuf-2.0","-lgio-2.0","-lgobject-2.0","-lglib-2.0","-lX11"
#   define LDLIBS_WIN "-Lresources/raylib_mingw/lib", "-lraylib","-lopengl32","-lgdi32","-lwinmm","-lcomdlg32","-lole32","-static", "-lpthread"
static struct conf_t conf = {.rl = true};
# endif
#else
# error "platform not supported bc dev is lazy :'3"
#endif

bool build_raylib(Cmd *cmd) {
  cmd_append(cmd, "make", "-C", "raylib/src");
  return cmd_run_sync_and_reset(cmd);
}

bool build_app(Cmd *cmd) {
  cmd_append(cmd, conf.windows ? CC_WIN : CC, "-o", "cliccy");
  cmd_append(cmd,CFLAGS);
  if(conf.debug) cmd_append(cmd, "-DDEBUG");
  cmd_append(cmd, INCLUDES);
  cmd_append(cmd, "src/cliccy.c");
  if(conf.windows)
    cmd_append(cmd, LDLIBS_WIN);
  else
    cmd_append(cmd, LDLIBS);
  
  return cmd_run_sync_and_reset(cmd);
}

void print_help(char *program_name) {
  char *pfile = program_name + strlen(program_name);
  for (; pfile > program_name; pfile--) {
    if ((*pfile == '\\') || (*pfile == '/')) {
      pfile++;
      break;
    }
  }
  printf("|------------- \e[95;1mbuilder for cliccy\e[0m ------------|\n");
  printf("    \e[94;1musage:\e[0m \e[96;1m."PATHSEP"%s\e[0m [run|win|test|debug]\n", pfile);
  printf("|------------------- \e[95;1mflags\e[0m -------------------|\n");
  //printf("|   \e[96;1mrl\e[0m    - build raylib                      |\n");
  printf("|   \e[96;1mrun\e[0m   - run app after building            |\n");
  printf("|   \e[96;1mwin\e[0m   - compile using mingw64             |\n");
  printf("|           (doesn't do anything on windows)  |\n");
  printf("|   \e[96;1mtest\e[0m  - run app with 'test' argument      |\n");
  printf("|   \e[96;1mdebug\e[0m - compile app with -DDEBUG          |\n");
  printf("|---------------------------------------------|\n");
}

int main(int argc, char **argv) {
  NOB_GO_REBUILD_URSELF(argc, argv);
  char *program = shift(argv, argc);
  Cmd cmd = {0};
  while(argc > 0) {
    char *val = shift(argv, argc);
    while(val[0] == '-') val++;
    if(strcmp(val, "test") == 0) 
      conf.test = true;
    else if(strcmp(val, "debug") == 0) 
      conf.debug = true;
    else if(strcmp(val, "win") == 0) 
      conf.windows = true;
    else if(strcmp(val, "run") == 0) 
      conf.run = true;
    // else if(strcmp(val, "rl") == 0) 
    //   conf.rl = true;
    else if(strcmp(val, "test-l") == 0){
      conf.test = true;
      conf.lines = true;
    } else if(strcmp(val, "test-q") == 0){
      conf.test = true;
      conf.question = true;
    } else if(strcmp(val, "help") == 0
      || strcmp(val, "h") == 0) {
      print_help(program);
      return 0;
    } else {
      nob_log(WARNING, "unknown flag: %s", val);
      print_help(program);
      return 1;
    }
  }
  if(conf.rl && !build_raylib(&cmd)) return 1;
  if(!build_app(&cmd)) return 1;
  if(conf.run) {
    cmd_append(&cmd, "./cliccy");
    if(conf.test) cmd_append(&cmd, "test");
    
    if(conf.lines) cmd_append(&cmd, "l");
    else if(conf.question) cmd_append(&cmd, "q");

    if(!cmd_run_sync_and_reset(&cmd)) return 1;
  }
  return 0;
}