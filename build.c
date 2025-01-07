#include <stdint.h>
#include <stdio.h>
#include <string.h>
#define NOB_IMPLEMENTATION
#define NOB_STRIP_PREFIX
#define NOB_CUSTOM_LOG
#include "include/nob.h"
#define LOGGER_IMPL
#include "include/log.h"
#include "src/util.c"
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
#   define LDLIBS "-Lraylib/src", "-Lresources/libnotify/lib64","-l:libraylib.a","-lGL","-lm","-lpthread","-ldl","-lrt","-l:libnotify.a","-lgdk_pixbuf-2.0","-lgio-2.0","-lgobject-2.0","-lglib-2.0", "-lwayland-client","-lwayland-cursor","-lwayland-egl","-lxkbcommon"
#   else
#   define LDLIBS "-Lraylib/src", "-Lresources/libnotify/lib64","-l:libraylib.a","-lGL","-lm","-lpthread","-ldl","-lrt","-l:libnotify.a","-lgdk_pixbuf-2.0","-lgio-2.0","-lgobject-2.0","-lglib-2.0","-lX11"
#   define LDLIBS_WIN "-Lresources/raylib_mingw/lib", "-l:libraylib.a","-lopengl32","-lgdi32","-lwinmm","-lcomdlg32","-lole32","-static", "-lpthread"
static struct conf_t conf = {.rl = true};
# endif
#else
# error "platform not supported bc dev is lazy :'3"
#endif
//meson setup -Dprefix=/home/rebecca/code/c/something/resources/libnotify -Dman=false -Dgtk_doc=false -Ddocbook_docs=disabled -Dintrospection=disabled -Ddefault_library=static --reconfigure build
//"meson","setup","-Dprefix=../resources/libnotify","-Dman=false","-Dgtk_doc=false","-Ddocbook_docs=disabled","-Dintrospection=disabled","-Ddefault_library=static","--reconfigure","build"
bool build_notify(Cmd *cmd) {
  bool result = true;
  char *dirup = NULL;
  if(!mkdir_if_not_exists("resources/libnotify")) return_defer(false);
  const char *cwd = concat("-Dprefix=", nob_get_current_dir_temp());
  set_current_dir("./libnotify");
  dirup = "..";
  const char *libnotify_prefix = concat(cwd, "/resources/libnotify");
  cmd_append(cmd, "meson","setup",libnotify_prefix,"-Dman=false","-Dgtk_doc=false","-Ddocbook_docs=disabled","-Dintrospection=disabled","-Ddefault_library=static","build");
  
  if(!cmd_run_sync_and_reset(cmd)) return_defer(false);
  set_current_dir("./build");
  cmd_append(cmd, "meson", "install");
  dirup = "../..";
  if(!cmd_run_sync_and_reset(cmd)) return_defer(false);
defer:
  if(dirup) set_current_dir(dirup);
  temp_reset();
  return result;
}

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

static const char *desktop_file_str = "[Desktop Entry]\n"
"Type=Application\n"
"Version=0.1\n"
"Name=Cliccy\n"
"Comment=fun program for cuties :3\n"
"TryExec=cliccy\n"
"Exec=cliccy\n"
"Icon=cliccy\n"
"Terminal=false\n"
"Categories=Game;Adult;\n"
"StartupNotify=false\n"
"X-GNOME-Autostart-Delay=60\n"
"X-Desktop-File-Install-Version=0.26\n";

bool install_app(Cmd *cmd) {
  char *home = getenv("HOME");
  char *binhome = concat(home, "/.local/bin");
  char *autostart = concat(home, "/.config/autostart");
  char *dstpath = concat(binhome, "/cliccy");
  char *desktop_file = concat(autostart, "/cliccy.desktop");
  assert(home != NULL && "homedir is null");
  if(!nob_mkdir_if_not_exists(binhome)) return false;
  if(!nob_mkdir_if_not_exists(autostart)) return false;
  if(nob_file_exists(dstpath)) {
    remove(dstpath);
  }
  if(!nob_copy_file("./cliccy", binhome)) return false;
  if(nob_file_exists(desktop_file)) {
    remove(desktop_file);
  }
  if(!nob_write_entire_file(desktop_file, desktop_file_str, strlen(desktop_file_str))) return false;
  return true;
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
  printf("  \e[94;1musage:\e[0m \e[96;1m."PATHSEP"%s\e[0m [run|win|test|debug|install]\n", pfile);
  printf("|------------------- \e[95;1mflags\e[0m -------------------|\n");
  //printf("|   \e[96;1mrl\e[0m    - build raylib                      |\n");
  printf("|   \e[96;1mrun\e[0m     - run app after building          |\n");
  printf("|   \e[96;1mwin\e[0m     - compile using mingw64           |\n");
  printf("|            (doesn't do anything on windows) |\n");
  printf("|   \e[96;1mtest\e[0m    - run app with 'test' argument    |\n");
  printf("|   \e[96;1mdebug\e[0m   - compile app with -DDEBUG        |\n");
  printf("|   \e[96;1minstall\e[0m - install app with autostart      |\n");
  printf("|            (works only on linux)            |\n");
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
#ifdef __linux__
  if(!build_notify(&cmd)) return 1;
#endif
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