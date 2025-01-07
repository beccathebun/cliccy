#include "cliccy.h"
#if defined(_WIN32)
// PEASYWINNOTY noty;
#elif defined(__linux__)
static NotifyNotification *curr_notif = NULL;
static GMainLoop *loop = NULL;
#endif // _WIN32, __linux__
static struct config_t {
  config_cfg_t config;
  config_data_t data;
} cfg = {0};

static const char *cfg_fmt = "[config] # configuration for cliccy :3\n"
"\n"
"# set any of these to false in order to\n"
"# disable them\n"
"[config.features]\n"
"lines  = %s\n"
"links  = %s\n"
"notifs = %s\n"
"popups = %s\n"
"\n"
"[config.timer]\n"
"# minimum/maximum sleep between stuff happening in minutes\n"
"minimum = %d\n"
"maximum = %d\n"
"\n"
"# notifications\n"
"[config.notifs]\n"
"icon = 'notification_icon.png'\n"
"\n"
"[config.lines]\n"
"# minimum/maximum amount of lines to write\n"
"minimum = %d\n"
"maximum = %d\n"
"# minimum/maximum added when line doesn't match\n"
"penalty_min = %d\n"
"penalty_max = %d\n"
"[data]\n"
"# window/notification title\n"
"title    = 'Cliccy :3'\n"
"# what cliccy calls you :3\n"
"petnames = [\n"
"  'love',\n"
"  'cutie',\n"
"  'pet',\n"
"  'my bunny',\n"
"  'my puppy',\n"
"  'my darling'\n"
"]\n"
"# what lines cliccy asks you to write\n"
"lines = ['i am a good bunny']\n"
"# links to open\n"
"links    = [\n"
"  'https://google.com'\n"
"]\n";
config_cfg_t default_cfg = {
  .feat = {true,true,true,true},
  .lines = {3,20,1,5},
  .notifs = {"notification_icon.png"},
  .timer = {5,20}
};
#define bs(b) (b ? "true" : "false")

static char *config_fmt(config_cfg_t* c){
  return nob_temp_sprintf(cfg_fmt,
  bs(c->feat.lines),
  bs(c->feat.links),
  bs(c->feat.notifs),
  bs(c->feat.popups),
  c->timer.minimum,
  c->timer.maximum,
  c->lines.minimum,
  c->lines.maximum,
  c->lines.penalty_min,
  c->lines.penalty_max
  );
}

#define swap(t,a,b) do{ t tmp = a; a = b; b = tmp;}while(0)
static bool parse_config(const char *path) {
  String_Builder sb = {0};
  bool result       = true;
  char *errbuf      = CALLOC(200, sizeof(char));
  if(!read_entire_file(path, &sb)) return_defer(false);
  sb_append_null(&sb);
  toml_table_t *tbl = toml_parse(sb.items, errbuf, 200*sizeof(char));
  if(!tbl) {
    logs(LOG_ERROR, "toml-c: couldn't parse config: %s", errbuf);
    return_defer(false);
  }
  toml_table_t *config_tbl = toml_table_table(tbl, "config");
  toml_table_t *data_tbl   = toml_table_table(tbl, "data");

  toml_table_t *feat_tbl   = toml_table_table(config_tbl, "features");
  toml_table_t *timer_tbl  = toml_table_table(config_tbl, "timer");
  toml_table_t *notifs_tbl = toml_table_table(config_tbl, "notifs");
  toml_table_t *lines_tbl  = toml_table_table(config_tbl, "lines");

  toml_value_t feat_notifs = toml_table_bool(feat_tbl, "notifs");
  toml_value_t feat_links  = toml_table_bool(feat_tbl, "links");
  toml_value_t feat_lines  = toml_table_bool(feat_tbl, "lines");
  toml_value_t feat_popups = toml_table_bool(feat_tbl, "popups");
  if(!feat_notifs.ok) feat_notifs.u.b = default_cfg.feat.notifs;
  if(!feat_links.ok)  feat_links.u.b  = default_cfg.feat.links;
  if(!feat_lines.ok)  feat_lines.u.b  = default_cfg.feat.lines;
  if(!feat_popups.ok) feat_popups.u.b = default_cfg.feat.popups;
  cfg.config.feat.popups = feat_popups.u.b;
  cfg.config.feat.lines  = feat_lines.u.b;
  cfg.config.feat.links  = feat_links.u.b;
  cfg.config.feat.notifs = feat_notifs.u.b;
  toml_value_t timer_min = toml_table_int(timer_tbl, "minimum");
  toml_value_t timer_max = toml_table_int(timer_tbl, "maximum");
  if(!timer_min.ok) timer_min.u.i = default_cfg.timer.minimum;
  if(!timer_max.ok) timer_max.u.i = default_cfg.timer.maximum;
  if(timer_min.u.i > timer_max.u.i) swap(int64_t, timer_min.u.i,timer_max.u.i);
  cfg.config.timer.minimum = timer_min.u.i;
  cfg.config.timer.maximum = timer_max.u.i;

  toml_value_t notifs_icon = toml_table_string(notifs_tbl, "icon");
  if(!notifs_icon.ok) notifs_icon.u.s = "notification_icon.png";
  cfg.config.notifs.icon = CALLOC(strlen(notifs_icon.u.s)+1,sizeof(char));
  strcpy(cfg.config.notifs.icon,notifs_icon.u.s);

  toml_value_t lines_min = toml_table_int(lines_tbl, "minimum");
  toml_value_t lines_max = toml_table_int(lines_tbl, "maximum");
  toml_value_t lines_pen_max 
    = toml_table_int(lines_tbl, "penalty_max");
  toml_value_t lines_pen_min 
    = toml_table_int(lines_tbl, "penalty_min");
  if(!lines_min.ok) lines_min.u.i = default_cfg.lines.minimum;
  if(!lines_max.ok) lines_max.u.i = default_cfg.lines.maximum;
  if(!lines_pen_min.ok) lines_pen_min.u.i = default_cfg.lines.penalty_min;
  if(!lines_pen_max.ok) lines_pen_max.u.i = default_cfg.lines.penalty_max;
  if(lines_min.u.i > lines_max.u.i) 
    swap(int64_t, lines_min.u.i,lines_max.u.i);
  if(lines_pen_min.u.i > lines_pen_max.u.i) 
    swap(int64_t, lines_pen_min.u.i,lines_pen_max.u.i);
  cfg.config.lines.minimum = lines_min.u.i;
  cfg.config.lines.maximum = lines_max.u.i;
  cfg.config.lines.penalty_min = lines_pen_min.u.i;
  cfg.config.lines.penalty_max = lines_pen_max.u.i;
  toml_value_t title     = toml_table_string(data_tbl, "title");
  toml_array_t *petnames = toml_table_array(data_tbl, "petnames");
  toml_array_t *lines    = toml_table_array(data_tbl, "lines");
  toml_array_t *links    = toml_table_array(data_tbl, "links");
  if(!title.ok) title.u.s = "Cliccy :3";
  cfg.data.title = CALLOC(strlen(title.u.s), sizeof(char));
  strcpy(cfg.data.title, title.u.s);
  for(int i = 0; i < toml_array_len(petnames); ++i) {
    toml_value_t val = toml_array_string(petnames,i);
    if(!val.ok) return_defer(false);
    char *str = CALLOC(strlen(val.u.s)+1, sizeof(char));
    strcpy(str, val.u.s);
    da_append(&cfg.data.petnames, str);
  }
  for(int i = 0; i < toml_array_len(lines); ++i) {
    toml_value_t val = toml_array_string(lines,i);
    if(!val.ok) return_defer(false);
    char *str = CALLOC(strlen(val.u.s)+1, sizeof(char));
    strcpy(str, val.u.s);
    da_append(&cfg.data.lines, str);
  }
  for(int i = 0; i < toml_array_len(links); ++i) {
    toml_value_t val = toml_array_string(links,i);
    if(!val.ok) return_defer(false);
    char *str = CALLOC(strlen(val.u.s)+1, sizeof(char));
    strcpy(str, val.u.s);
    da_append(&cfg.data.links, str);
  }
#if defined(DEBUG) && !defined(_WIN32)
  structInfo cfg_info[] = {
    {
      .data = &cfg,
      .structName = "config_t",
      .level = 0
    }
  };
  pprint_struct("cfg", cfg_info, ARRAY_SIZE(cfg_info));
#endif //DEBUG
defer:
  free(errbuf);
  if(tbl != NULL) toml_free(tbl);
  return result;
}

//globals bc can't be arsed to deal with glib :3


static bool reinitializeClay = false;


static char *cliccy_messages[] = {
  "Good slut :3"
};

static char *yess[] = {
  "yes >.<",
  "yes :3",
  "mhm >//<",
  "yess",
  "*nod*",
  ">////<",
  "*blushes*"
};

static char *nos[] = {
  "no >:3",
  "*shakes head*",
  "nopers",
  "nooo",
  "nuh-uh!",
  "niet ;A;"
};

static char *emotes_pos[] = {
  ">///<",
  ">.<",
  ":3",
  ">/w/<",
  "o///o",
  "<3",
  ">w<",
  ">_<",
  ">//x//<"
};

static char *wrongs[] = {
  "wrong!",
  "nope!",
  "no!",
  "incorrect!"
};

static char *emotes_tease[] = {
  "~",
  " :3",
  " ;3"
};

static char *cliccy_questions[] = {
  "Are you a good clickslut?",
  "Do you love my shiny links? :3",
  "Does clicking make you tingly? :b"
};

void HandleClayErrors(Clay_ErrorData errorData) {
    printf("%s", errorData.errorText.chars);
    if (errorData.errorType == CLAY_ERROR_TYPE_ELEMENTS_CAPACITY_EXCEEDED) {
        reinitializeClay = true;
        Clay_SetMaxElementCount(Clay__maxElementCount * 2);
    } else if (errorData.errorType == CLAY_ERROR_TYPE_TEXT_MEASUREMENT_CAPACITY_EXCEEDED) {
        reinitializeClay = true;
        Clay_SetMaxMeasureTextCacheWordCount(Clay__maxMeasureTextCacheWordCount * 2);
    }
}
bool is_time(time_t timer) {
  double diff = difftime(time(NULL), timer);
  return diff >= 0.0;
}
Clickslut_Action rand_action() {
  int action = rand() % CA_COUNT;
  assert(action < CA_COUNT);
  return action;
}



bool link_new() {
  Cmd cmd;
  const char *l = da_rand(cfg.data.links);
  cmd_append(&cmd, URL_OPEN, l);
  if(!cmd_run_sync_and_reset(&cmd)) {
    logs(LOG_ERROR,"could not open link :c");
    return false;
  }
  return true;
}

static bool notif_closed = true;
#if !defined(_WIN32)
void closed_callback() {
  notif_closed = true;
  logs(LOG_TRACE, "notif closed :3");
  if(loop != NULL) g_main_loop_quit(loop);
}

int close_callback(void* data) {
  NotifyNotification* notif = (NotifyNotification*)data;
  logs(LOG_TRACE, "notif timeout :3");
  notify_notification_close(notif, NULL);
  return 0;
}

void dismiss_callback(
  NotifyNotification* notification,
  char* action,
  gpointer user_data
) {
  link_new();
  notify_notification_close(notification, NULL);
  logs(LOG_TRACE, "notif dismissed :3");
}
#endif //_WIN32
bool notif_new() {
#if defined(_WIN32)
#warning "notifications not yet supported on windows"
  return true;
#else
  assert(curr_notif != NULL);
  if(!notif_closed) return true;
  notify_notification_update(curr_notif, 
  cfg.data.title,
    arr_rand(cliccy_messages),
    NULL);
  g_signal_connect(curr_notif, "closed", closed_callback, NULL);
  notify_notification_add_action(curr_notif, "dismiss", "Click! :3", dismiss_callback, NULL, NULL);
  
  if (!notify_notification_show(curr_notif, 0)) 
  {
      logs(LOG_ERROR, "libnotify: show has failed :c");
      return false;
  }
  notif_closed = false;
  if(loop != NULL){
    //just make it block for a minute so that it doesn't stop the whole app :b
    g_timeout_add_seconds(60, close_callback, curr_notif);
    g_main_loop_run(loop);
  } 
  return true;
#endif //_WIN32
}
bool dialog_new() {
  TODO("DIALOG");
  return true;
}



hoverCBDef(HandleQuestionOk) {
  QuestionConfig *conf = (QuestionConfig*)userData;
  if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
      // the yes text is callocd only if stage 2
      if(conf->stage == Qs_Bad) free(conf->yes);
      #ifdef DEBUG
        conf->stage = Qs_First;
        conf->text  = arr_rand(cliccy_questions);
        conf->yes   = arr_rand(yess);
        conf->no    = arr_rand(nos);
      #else
        conf->stage = Qs_Done;
      #endif
  }
}

hoverCBDef(HandleQuestionBad) {
  QuestionConfig *conf = (QuestionConfig*)userData;
  if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
      // Do some click handling
      conf->stage = Qs_Bad;
      char *emote = arr_rand(emotes_pos);
      char *y;
      if(istartsWith("are you", conf->text))
        y = rand() % 2 == 0 ? "I amm"    : "i am";
      else if(istartsWith("do you", conf->text))
        y = rand() % 2 == 0 ? "I doo"    : "i do";
      else
        y = rand() % 2 == 0 ? "It doess" : "it does";
      // conf->yes = malloc(strlen(y)+strlen(emote)+1);
      // strcpy(conf->yes, y);
      // strcat(conf->yes, emote);
      conf->yes = concatse(y, emote, rand()%4);
      char *w = arr_rand(wrongs);
      char *tmp = concats(w, "try again");
      conf->wrong = concat(tmp, arr_rand(emotes_tease));
  }
}


// Examples of re-usable "Components"

Clay_RenderCommandArray CreateQuestionboxLayout(
  QuestionConfig *conf
) {
  bool is2nd = conf->stage == Qs_Bad;
  Clay_BeginLayout();
  {
    CLAY(CLAY_ID("Container"), 
      CLAY_LAYOUT({ 
        .sizing   = {.width=CLAY_SIZING_GROW({}),.height=CLAY_SIZING_GROW({})},
        .padding  = { 16, 16 },
        .childGap = 16,
        .layoutDirection = CLAY_TOP_TO_BOTTOM
        }), 
        CLAY_RECTANGLE({ 
          .color = COLOR_BG
    })){
      CLAY(CLAY_ID("TextPanel"),
      CLAY_LAYOUT({
        .childAlignment = {
          .x = CLAY_ALIGN_X_CENTER,
          .y = CLAY_ALIGN_Y_CENTER
        },
        .sizing = {
          .height = CLAY_SIZING_GROW({}),
          .width  = CLAY_SIZING_GROW({})
        },
        .layoutDirection = CLAY_TOP_TO_BOTTOM,
        .childGap = 16
      })){
        CTEXT(conf->text, 35, COLOR_GRN);
        if(is2nd) {
          CTEXT(conf->wrong, 35, COLOR_RED);
          CTEXT(conf->text, 35, COLOR_GRN);
        }
      }
      CLAY(CLAY_ID("Footer"),
      CLAY_LAYOUT({
        .sizing = {
          .width = CLAY_SIZING_GROW({})
        },
        .childAlignment = {
          .x = is2nd ? CLAY_ALIGN_X_CENTER : CLAY_ALIGN_X_LEFT
        }
      })){
        if(!is2nd){
          RenderButton("ButtonBad", COLOR_RED, conf->no, TEXT_CONF(28, COLOR_RED), HandleQuestionBad, (intptr_t)conf);
          CLAY(CLAY_LAYOUT({.sizing = {.width = CLAY_SIZING_GROW({})}}));
        }
        RenderButton("ButtonOk", COLOR_GRN, conf->yes, TEXT_CONF(is2nd ? 32 : 28, COLOR_GRN), HandleQuestionOk,(intptr_t)conf);
      }
    }
  }
  return Clay_EndLayout();
}

#ifdef DEBUG
static bool debugEnabled = false;
#endif
void common_update_frame(bool debug) {
  Vector2 mouseWheelDelta = GetMouseWheelMoveV();
  float mouseWheelX       = mouseWheelDelta.x;
  float mouseWheelY       = mouseWheelDelta.y;

  #ifdef DEBUG
  if (IsKeyPressed(KEY_D) && debug) {
      debugEnabled = !debugEnabled;
      Clay_SetDebugModeEnabled(debugEnabled);
  }
  #endif
  Clay_Vector2 mousePosition = RLV2_TO_CLAYV2(GetMousePosition());
  Clay_SetPointerState(mousePosition, IsMouseButtonDown(0));
  Clay_SetLayoutDimensions((Clay_Dimensions) { (float)GetScreenWidth(), (float)GetScreenHeight() });


  Clay_UpdateScrollContainers(true, (Clay_Vector2) {mouseWheelX, mouseWheelY}, GetFrameTime());
}
void UpdateQuestionFrame(QuestionConfig *conf)
{
    common_update_frame(true);
    // Generate the auto layout for rendering
    double currentTime = GetTime();
    Clay_RenderCommandArray renderCommands 
                       = CreateQuestionboxLayout(conf);
    // printf("layout time: %f microseconds\n", (GetTime() - currentTime) * 1000 * 1000);
    // RENDERING ---------------------------------
//    currentTime = GetTime();
    BeginDrawing();
    ClearBackground(CLAY_COLOR_TO_RAYLIB_COLOR(COLOR_BG));
    Clay_Raylib_Render(renderCommands);
    EndDrawing();
//    printf("render time: %f ms\n", (GetTime() - currentTime) * 1000);

    //----------------------------------------------------------------------------------
}

static Clay_Arena clayMemory;
static bool fonts_inited;

void rl_init(int w, int h) {
  Clay_Raylib_Initialize(w,h, cfg.data.title, FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI | FLAG_MSAA_4X_HINT);
  if(!fonts_inited) {
    Raylib_fonts[FONT_ID_BODY_24] = (Raylib_Font) {
      .font = LoadFontEx("resources/font/PixelifySans.ttf", 48, 0, 400),
      .fontId = FONT_ID_BODY_24,
    };
    Raylib_fonts[FONT_ID_BODY_16] = (Raylib_Font) {
      .font = LoadFontEx("resources/font/PixelifySans.ttf", 32, 0, 400),
      .fontId = FONT_ID_BODY_16,
    };
    fonts_inited = true;
  }
  
	SetTextureFilter(Raylib_fonts[FONT_ID_BODY_24].font.texture, TEXTURE_FILTER_BILINEAR);
  SetTextureFilter(Raylib_fonts[FONT_ID_BODY_16].font.texture, TEXTURE_FILTER_BILINEAR);
}

bool question_new() {
  bool result = false;
  rl_init(500, 350);
  //InitWindow(500, 350, arr_rand(cliccy_titles));
  
  // Font f = LoadFont("./PixelifySans.ttf");
  const char *text    = arr_rand(cliccy_questions);
  bool quit           = false;
  int stage           = 0;
  QuestionConfig conf = {
    .text = arr_rand(cliccy_questions),
    .yes  = arr_rand(yess),
    .no   = arr_rand(nos)
  };
  while(!WindowShouldClose() && conf.stage != Qs_Done) {
    if (reinitializeClay) {
      if(clayMemory.memory != NULL) free(clayMemory.memory);
      Clay_SetMaxElementCount(8192);
      uint32_t totalMemorySize = Clay_MinMemorySize();
      clayMemory = Clay_CreateArenaWithCapacityAndMemory(totalMemorySize, malloc(totalMemorySize));
      Clay_Initialize(clayMemory, (Clay_Dimensions) { (float)GetScreenWidth(), (float)GetScreenHeight() }, (Clay_ErrorHandler) { HandleClayErrors });
      reinitializeClay = false;
    }
    UpdateQuestionFrame(&conf);
  }
  CloseWindow();
  
  // if(!result) for(size_t i = 0; i < 10;++i) {
  //   link_new();
  //   sleep(1);
  // }
  
  
  return true;
}

Clay_RenderCommandArray CreateLinesLayout(
  InputConfig  *conf
) {
  LinesCfg *lines = (LinesCfg*)conf->userdata;
  Clay_BeginLayout();
  {
    CLAY(CLAY_ID("Container"), 
      CLAY_LAYOUT({ 
        .sizing   = {.width=CLAY_SIZING_GROW({}),.height=CLAY_SIZING_GROW({})},
        .padding  = { 16, 16 },
        .childGap = 16,
        .layoutDirection = CLAY_TOP_TO_BOTTOM
        }), 
        CLAY_RECTANGLE({ 
          .color = COLOR_BG
    })){
      CLAY(CLAY_ID("Header"),
      CLAY_LAYOUT({
        .sizing = {
          .height = CLAY_SIZING_FIXED(100),
          .width = CLAY_SIZING_GROW({})
        },
        .childAlignment = {
          .x = CLAY_ALIGN_X_LEFT,
          .y = CLAY_ALIGN_Y_CENTER
        },
      })){
        CLAY(CLAY_LAYOUT({
            .sizing = {
              .width = CLAY_SIZING_FIXED(150),
              .height = CLAY_SIZING_GROW({})
            },
            .childAlignment = {
              .x = CLAY_ALIGN_X_LEFT,
              .y = CLAY_ALIGN_Y_CENTER
            },
            .layoutDirection = CLAY_TOP_TO_BOTTOM
        })){
          CLAY(CLAY_LAYOUT({.sizing = {.width = CLAY_SIZING_FIT({})},
            .childAlignment = {
              .x = CLAY_ALIGN_X_CENTER,
              .y = CLAY_ALIGN_Y_CENTER
            },.layoutDirection = CLAY_TOP_TO_BOTTOM})){
            CTEXT(temp_sprintf("attempts"),30, COLOR_GRN);
            CTEXT(temp_sprintf("%zu", lines->attempts), 30, COLOR_GRN);
            };
          
        }
        CLAY(CLAY_LAYOUT({.sizing = {.width = CLAY_SIZING_GROW({})}}));
        CLAY(CLAY_LAYOUT({
            .sizing = {
              .width = CLAY_SIZING_FIXED(150),
              .height = CLAY_SIZING_GROW({})
            },
            .childAlignment = {
              .x = CLAY_ALIGN_X_CENTER,
              .y = CLAY_ALIGN_Y_CENTER
            },
            .layoutDirection = CLAY_TOP_TO_BOTTOM
        })){
          
          CTEXT("Target",35, COLOR_TEAL);
          CTEXT(temp_sprintf("%zu", lines->target), 30, COLOR_TEAL);
        }
        CLAY(CLAY_LAYOUT({.sizing = {.width = CLAY_SIZING_GROW({})}}));
        
        CLAY(CLAY_LAYOUT({
            .sizing = {
              .width = CLAY_SIZING_FIXED(150),
              .height = CLAY_SIZING_GROW({})
            },
            .childAlignment = {
              .x = CLAY_ALIGN_X_RIGHT,
              .y = CLAY_ALIGN_Y_CENTER
            },
            .layoutDirection = CLAY_TOP_TO_BOTTOM
        })){
          CLAY(CLAY_LAYOUT({.sizing = {.width = CLAY_SIZING_FIT({})},
            .childAlignment = {
              .x = CLAY_ALIGN_X_CENTER,
              .y = CLAY_ALIGN_Y_CENTER
            },.layoutDirection = CLAY_TOP_TO_BOTTOM})){
              CTEXT(temp_sprintf("fails"),30, COLOR_RED);
              CTEXT(temp_sprintf("%zu", lines->fails), 30, COLOR_RED);
            };
          
        }
      }
      CLAY(CLAY_ID("TextPanel"),
      CLAY_LAYOUT({
        .childAlignment = {
          .x = CLAY_ALIGN_X_CENTER
        },
        .sizing = {
          .height = CLAY_SIZING_GROW({}),
          .width  = CLAY_SIZING_GROW({})
        },
        .layoutDirection = CLAY_TOP_TO_BOTTOM,
        .childGap = 16
      })){
        CTEXT("Be a good pet and write:",30, COLOR_BLU);
        CTEXT(lines->line, 35, COLOR_PUR);
        CTEXT("For me :3",30, COLOR_BLU);
        CLAY(CLAY_LAYOUT({.sizing = {.width = CLAY_SIZING_GROW({})}}));
        if(lines->show_fail && !is_time(lines->close_fail)) {
          CTEXT("you made an oopsie", 35, COLOR_RED);
          CTEXT("target increased :b", 30, COLOR_RED);
        } 
        if(lines->done) {
          CTEXT("you did it!! :3", 35, COLOR_GRN);
          CTEXT("so proud of you <3", 30, COLOR_GRN);
        }
      }
      CLAY(CLAY_ID("Footer"),
      CLAY_LAYOUT({
        .sizing = {
          .width = CLAY_SIZING_GROW({}),
          .height = CLAY_SIZING_FIXED(100)
        },
        .padding = {.x = 10, .y = 20}
      })){
        RenderInputbox(TEXT_CONF(30, COLOR_TEAL), conf);
      }
    }
  }
  return Clay_EndLayout();
}

/**
callback for when input box is submitted
 */
void input_submit(InputConfig *conf) {
  LinesCfg *lines = (LinesCfg *)conf->userdata;
  lines->attempts++;
  if(!streq(lines->line, conf->buf)) {
    lines->fails++;
    lines->target 
      += rand_range(
        cfg.config.lines.penalty_min,
        cfg.config.lines.penalty_max);
    lines->show_fail    = true;
    conf->color         = COLOR_RED;
    lines->close_fail   = time_offset_s(3);
  } else if(lines->show_fail) {
    lines->show_fail = false;
    conf->color = COLOR_BLU;
  }
  input_reset_buf(conf);
  if(lines->attempts >= lines->target && !lines->done) {
    lines->done    = true;
    lines->close   = time_offset_s(5);
    conf->enabled  = false;
  }
}

bool lines_new() {
  rl_init(500,600);
  LinesCfg lines = {
    .line = da_rand(cfg.data.lines),
    .target = (size_t)rand_range(cfg.config.lines.minimum, cfg.config.lines.maximum)
  };
  size_t lnlen = strlen(lines.line)+1;
  InputConfig input = {
    .buf = CALLOC(lnlen, sizeof(char)),
    .buf_size = lnlen,
    .color = COLOR_TEAL,
    .id = "LineInput",
    .submit = input_submit,
    .userdata = (intptr_t)&lines,
    .enabled = true,
    .disabled_text = "well done cutie~",
    .diabled_color = COLOR_BLU
  };
  if(input.buf == NULL) {
    logs(Log_Fatal, "couldn't allocate input buffer");
    exit(-1);
  }
  bool lines_done = false;
  while(!WindowShouldClose() && !lines_done) {
    if(!lines.done) {
      poll_input(&input);
    } else {
      lines_done = is_time(lines.close);
    }
    common_update_frame(!input.focused);
    
    double currentTime = GetTime();
    Clay_RenderCommandArray renderCommands 
                       = CreateLinesLayout(&input);
    BeginDrawing();
    ClearBackground(CLAY_COLOR_TO_RAYLIB_COLOR(COLOR_BG));
    Clay_Raylib_Render(renderCommands);
    
    EndDrawing();
    temp_reset();
  }
  CloseWindow();
  free(input.buf);
  return true;
}
const dispatcher dispatchers[] = {
  [CA_Link]     = link_new,
  [CA_Lines]    = lines_new,
  [CA_Notif]    = notif_new,
  [CA_Dialog]   = dialog_new,
  [CA_Question] = question_new
};

static_assert(ARRAY_LEN(dispatchers) == CA_COUNT , "add methods eejit");

bool dispatch_action() {
  //Clickslut_Action action = rand_action();
  //if(action == CA_Lines) return dispatch_action();
  return dispatchers[CA_Question]();
}

time_t rand_time() {
  int min = rand_range(cfg.config.timer.minimum, cfg.config.timer.maximum);
  return time_offset_min(min);
}

bool init_config(char *path) {
  char *conf_dir;
  char *conf_file;
  if(path != NULL) {
    conf_file = path;
    goto mkfile;
  }
#if defined(__linux__)
  if((conf_dir = getenv("XDG_CONFIG_HOME")) == NULL) {
    char *home = getenv("HOME");
    assert(home != NULL && "homedir is null");
    conf_dir = concat(home, "/.config");
  }
#elif defined(_WIN32)
  if((conf_dir = getenv("LocalAppData")) == NULL) {
    assert(false && "Local app data null???");
  }
#endif
  assert(conf_dir != NULL && "config directory null");
  char *cliccy_dir = concat(conf_dir, PATHSEP "cliccy");
  if(!nob_mkdir_if_not_exists(cliccy_dir)) return false;
  conf_file = concat(cliccy_dir, PATHSEP "config.toml");
mkfile:
  if(!nob_file_exists(conf_file)) {
    const char *conf = config_fmt(&default_cfg);
    if(!nob_write_entire_file(
      conf_file, 
      conf, 
      strlen(conf))) return false;
    logs(LOG_INFO, "Configuration file created: %s", conf_file);
    temp_reset();
  }
  return parse_config(conf_file);
}



void clay_init() {
  #if defined(DEBUG)
    set_log_level(Log_Debug);
    SetTraceLogLevel(Log_Debug);
  #else
    SetTraceLogLevel(Log_Error);
  #endif // DEBUG
  SetTraceLogCallback(vlogs);
  uint64_t totalMemorySize = Clay_MinMemorySize();
  clayMemory = Clay_CreateArenaWithCapacityAndMemory(totalMemorySize, malloc(totalMemorySize));
  Clay_SetMeasureTextFunction(Raylib_MeasureText);
  Clay_Initialize(clayMemory, (Clay_Dimensions) { (float)GetScreenWidth(), (float)GetScreenHeight() }, (Clay_ErrorHandler) { HandleClayErrors });
  
}

Result init() {
#if defined(__linux__)
  if(!notify_init("Clicksluts Central")) {
    logs(LOG_ERROR, "libnotify: couldn't initialise ;w;");
    return Res_Fail_Libnotify;
  }
  
  curr_notif = notify_notification_new(
    cfg.data.title,
    arr_rand(cliccy_messages),
    NULL
  );
  notify_notification_set_timeout(curr_notif, 1000);
  loop = g_main_loop_new(NULL, FALSE);
#elif defined(_WIN32)
  // CoInitializeEx(0, COINIT_MULTITHREADED);
  // noty = EasyWinNoty_CreateInstance();
  // if (!EasyWinNoty_IsSupportSystem(noty)) {
	// 	printf("System does not support windows notification.");
	// }
	
	// if (!EasyWinNoty_IsSupportAdvancedFeature(noty)) {
	// 	printf("System does not support advanced windows notification.");
	// }
#endif
  clay_init();
  return Res_Success;
}



bool test_main(int argc, char **argv) {
  const char *test = shift(argv,argc);
  char *feat = shift(argv,argc);
  clay_init();
  if(streq(feat, "q"))
    return question_new();
  else if(streq(feat, "l"))
    return lines_new();
  return false;
}



int main(int argc, char **argv)
{
  seed();
  const char *program_name = shift(argv, argc);
  bool result              = true;
  if(!init_config(NULL)) return_defer(false);
  if(argc > 0) {
    if(streq(argv[0], "test")) 
      return_defer(test_main(argc, argv));
  }
  time_t timer = rand_time();
  bool quit = false;
  
  if(!init()) return_defer(false);
  #if defined(__linux__)
  assert(curr_notif != NULL);
  #endif //__linux__
  printf("timer scheduled for: %s :3", ctime(&timer));
  //if(!notif_new()) return_defer(Res_Fail_Libnotify);
  
  while (!quit) {
    sleep(1);
    if(!is_time(timer)   ) continue;
    if(!dispatch_action()) quit = true;
    timer = rand_time();
    printf("timer scheduled for: %s :3", ctime(&timer));
  }
  
defer:
#if defined(__linux__)
  notify_uninit();
#endif //__linux__
  if(clayMemory.memory != NULL)
    free(clayMemory.memory);
  if(!result) return 1;
  da_free(cfg.data.lines);
  da_free(cfg.data.links);
  da_free(cfg.data.petnames);
  printf(T_RESET);
  return 0;
}

// TODO : clean up the mess
// TODO : windows??