
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
  if(logger_print_time) logger_print_time = false;
  va_list args;
  va_start(args, fmt);
  vlogs(level+3, fmt, args);
  va_end(args);
}
struct conf_t {
  bool windows;
  bool debug;
  bool run;
  bool force;
};





#ifdef _WIN32
# define INCLUDES "-Iinclude", "-I.","-Iexternal"
# define CFLAGS "-std=c23","-D_DEFAULT_SOURCE","-Wno-missing-braces","-Wno-unused-value","-Wno-pointer-sign", "-Z"
# define PATHSEP "\\"
# define CC "clang-cl"
# define CC_WIN CC
# define LDLIBS "-lraylib","-lopengl32","-lgdi32","-lwinmm","-lcomdlg32","-lole32","-static", "-lpthread"
# define LDLIBS_WIN LDLIBS
static struct conf_t conf = {.windows=true};
#elif  __linux__
# define INCLUDES "-Iinclude", "-I.","-Iexternal","-I/usr/include/gdk-pixbuf-2.0","-I/usr/include/glib-2.0","-I/usr/lib64/glib-2.0/include","-I/usr/include/libpng16","-I/usr/include/webp","-DWITH_GZFILEOP","-I/usr/include/libmount","-I/usr/include/blkid","-I/usr/include/sysprof-6","-pthread"
# define INCLUDES_WIN "-Iinclude", "-I.","-Iexternal"
# define CFLAGS "-std=c23","-D_DEFAULT_SOURCE","-Wno-missing-braces","-Wno-unused-value","-Wno-pointer-sign", "-ggdb"
# define CFLAGS_WIN "-std=c23","-D_DEFAULT_SOURCE","-Wno-missing-braces","-Wno-unused-value","-Wno-pointer-sign", "-ggdb"
# define PATHSEP "/"
# define CC "gcc"
# define CC_WIN "x86_64-w64-mingw32-gcc"
# ifdef USE_WAYLAND_DISPLAY
#   define LDLIBS "-Lraylib/src", "-Lresources/libnotify/lib64","-l:libraylib.a","-lGL","-lm","-lpthread","-ldl","-lrt","-l:libnotify.a","-lgdk_pixbuf-2.0","-lgio-2.0","-lgobject-2.0","-lglib-2.0", "-lwayland-client","-lwayland-cursor","-lwayland-egl","-lxkbcommon"
#   else
#   define LDLIBS "-Lraylib/src", "-Lresources/libnotify/lib64","-l:libraylib.a","-lGL","-lm","-lpthread","-ldl","-lrt","-l:libnotify.a","-lgdk_pixbuf-2.0","-lgio-2.0","-lgobject-2.0","-lglib-2.0","-lX11"
#   define LDLIBS_WIN "-Lresources/raylib_mingw/lib","-Lresources/wintoastlibc_x64", "-l:libraylib.a","-l:wintoastlibc.dll.a","-lopengl32","-lgdi32","-lwinmm","-lcomdlg32","-lole32","-static", "-lpthread", "-mwindows"
static struct conf_t conf = {0};
# endif
#else
# error "platform not supported bc dev is lazy :'3"
#endif
//meson setup -Dprefix=/home/rebecca/code/c/something/resources/libnotify -Dman=false -Dgtk_doc=false -Ddocbook_docs=disabled -Dintrospection=disabled -Ddefault_library=static --reconfigure build
//"meson","setup","-Dprefix=../resources/libnotify","-Dman=false","-Dgtk_doc=false","-Ddocbook_docs=disabled","-Dintrospection=disabled","-Ddefault_library=static","--reconfigure","build"
bool build_notify(Cmd *cmd) {
  if(file_exists("resources/libnotify/lib64/libnotify.a")) {
    logs(Log_Info, "libnotify already built :3");
    return true;
  }
    
  bool result = true;
  const char *cwd = get_current_dir_temp();
  logs(Log_Info, "Building libnotify :3");
  if(!mkdir_if_not_exists("resources/libnotify")) return_defer(false);
  const char *d = concat("-Dprefix=", cwd);
  set_current_dir("./libnotify");
  const char *libnotify_prefix = concat(d, "/resources/libnotify");
  cmd_append(cmd, "meson","setup",libnotify_prefix,"-Dman=false","-Dgtk_doc=false","-Ddocbook_docs=disabled","-Dintrospection=disabled","-Ddefault_library=static","build");
  
  if(!cmd_run_sync_and_reset(cmd)) return_defer(false);
  set_current_dir("./build");
  cmd_append(cmd, "meson", "install");
  if(!cmd_run_sync_and_reset(cmd)) return_defer(false);
defer:
  set_current_dir(cwd);
  temp_reset();
  return result;
}

bool build_raylib(Cmd *cmd) {
  if(file_exists("resources/raylib/lib64/libraylib.a")) {
    logs(Log_Info, "raylib already built :3");
    return true;
  }
  bool result = true;
  const char *cwd = get_current_dir_temp();
  const char *rl_build = "raylib/build";
  if(!mkdir_if_not_exists(rl_build)) return false;
  const char *cmake_prefix = concat(cwd, "/resources/raylib");
  set_current_dir(rl_build);
  cmd_append(cmd, "cmake", concat("-DCMAKE_INSTALL_PREFIX:PATH=", cmake_prefix), "..");
  if(!cmd_run_sync_and_reset(cmd)) return_defer(false);
  cmd_append(cmd, "make", "-j5", "install");
  if(!cmd_run_sync_and_reset(cmd)) return_defer(false);

  //cmd_append(cmd, "make", "-C", "raylib/src");
defer:
  set_current_dir(cwd);
  temp_reset();
  return result;
}

const char *input_paths[] = {
  "src/cliccy.c",
  "src/cliccy.h",
  "src/ui.c",
  "src/util.c",
  "include/log.h",
  "include/nob.h",
  "include/clay.h",
  "include/clay_renderer_raylib.c"
};

bool build_app_win(Cmd *cmd) {
  if(!conf.force && !nob_needs_rebuild("./cliccy.exe", input_paths, ARRAY_LEN(input_paths))){
    logs(Log_Info, "cliccy.exe already latest version :3");
    return true;
  }
  cmd_append(cmd, CC_WIN, "-o", "cliccy");
  cmd_append(cmd, CFLAGS_WIN);
  if(conf.debug) cmd_append(cmd, "-DDEBUG");
  cmd_append(cmd, INCLUDES_WIN);
  cmd_append(cmd, "src/cliccy.c");
  // cmd_append(cmd, "resources/wintoastlibc_x64/wintoastlibc.lib");
  cmd_append(cmd, LDLIBS_WIN);
  return cmd_run_sync_and_reset(cmd);
}

bool build_app(Cmd *cmd) {
  if(!conf.force && !nob_needs_rebuild("./cliccy", input_paths, ARRAY_LEN(input_paths))){
    logs(Log_Info, "cliccy already latest version :3");
    return true;
  }
  cmd_append(cmd,CC, "-o", "cliccy");
  cmd_append(cmd,CFLAGS);
  if(conf.debug) cmd_append(cmd, "-DDEBUG");
  cmd_append(cmd, INCLUDES);
  cmd_append(cmd, "src/cliccy.c");
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
  printf("|------------- \e[95;1mbuilder for cliccy!\e[0m ------------|\n");
  printf("    \e[94;1musage:\e[0m \e[96;1m."PATHSEP"%s\e[0m [flag(s)]\n", pfile);
  printf("|------------------- \e[95;1mflags:\e[0m -------------------|\n");
  //printf("|   \e[96;1mrl\e[0m    - build raylib                      |\n");
  printf("|   \e[96;1mrun\e[0m     - run app after building           |\n");
  // printf("|   \e[96;1mtest\e[0m    - run app with 'test' argument     |\n");
  printf("|   \e[96;1mdebug\e[0m   - compile app with -DDEBUG         |\n");
  printf("|   \e[96;1mforce\e[0m   - force compile everything         |\n");
  printf("|   \e[96;1minstall\e[0m - install app with autostart       |\n");
  printf("|            (works only on linux)             |\n");
  printf("|                                              |\n");
  printf("|  unrecognised args will be passed to cliccy  |\n");
  printf("|               if [run] is set                |\n");
  printf("|----------------------------------------------|\n");
}

int main(int argc, char **argv) {
  logger_print_time = false;
  NOB_GO_REBUILD_URSELF(argc, argv);
  char *program = shift(argv, argc);
  Cmd cmd = {0};
  Cmd args = {0};
  while(argc > 0) {
    char *val = shift(argv, argc);
    while(val[0] == '-') val++;
    if(strcmp(val, "debug") == 0) 
      conf.debug = true;
    else if(strcmp(val, "run") == 0) 
      conf.run = true;
    else if(strcmp(val, "help") == 0
      || strcmp(val, "h") == 0) {
      print_help(program);
      return 0;
    } else if(strcmp(val, "force") == 0
      || strcmp(val, "f") == 0) {
      conf.force = true;
    } else if(conf.run) {
      cmd_append(&args, val);
      if(argc > 0) da_append_many(&args, argv, argc);
      break;
    } else {
      nob_log(ERROR, "unknown flag: %s", val);
      print_help(program);
      return 1;
    }
  }
  if(!build_raylib(&cmd)) return 1;
#ifdef __linux__
  if(!build_notify(&cmd)) return 1;
  if(!build_app_win(&cmd)) return 1;
#endif
  if(!build_app(&cmd)) return 1;
  if(conf.run) {
    cmd_append(&cmd, "./cliccy");
    if(args.count) da_append_many(&cmd, args.items, args.count);
    if(!cmd_run_sync_and_reset(&cmd)) return 1;
  }
  da_free(args);
  da_free(cmd);
  return 0;
}