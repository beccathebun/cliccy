#ifndef UI_H
#define UI_H
#include <raylib.h>
#include <clay.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <log.h>
#include <assert.h>
#define BTN_WIDTH CLAY_SIZING_FIXED(200)
#define LINE_THIC 2.0f
#define DIALOG_LAYOUT CLAY_LAYOUT({            \
        .sizing = {.width=CLAY_SIZING_GROW({}),\
        .height = CLAY_SIZING_GROW({})},       \
        .padding = { 16, 16 },                 \
        .childGap = 16,                        \
        .layoutDirection = CLAY_TOP_TO_BOTTOM  \
        }),                                    \
        CLAY_RECTANGLE({ .color = COLOR_BG })
#define TEXT_CONF(size,colour) CLAY_TEXT_CONFIG((Clay_TextElementConfig){\
.fontId    = FONT_ID_BODY_24,                                            \
.fontSize  = (size),                                                     \
.textColor = (colour) })
#define CTEXT(s,size,colour) CLAY_TEXT(CLAY_STRING(s), TEXT_CONF(size,colour))
#define FONT_ID_BODY_24 0

#define COLOR_BG  Clay_GetColor(0x181818FF)
#define COLOR_GRN Clay_GetColor(0x3ac952ff)
#define COLOR_RED Clay_GetColor(0xd64684ff)
#define COLOR_BLU Clay_GetColor(0x5ec3f2ff)
#define COLOR_PUR Clay_GetColor(0xbf77f9ff)
#define COLOR_TEAL Clay_GetColor(0x74f7d8ff)
#define hoverCBDef(n) void n(Clay_ElementId elementId, Clay_PointerData pointerData, intptr_t userData)

typedef void (*clay_onhover)(Clay_ElementId elementId, Clay_PointerData pointerInfo, intptr_t userData);


typedef struct input_t Input;

typedef void (*input_submit_t)(Input *input);

struct input_t {
  const char *id; // id passed to clay
  Clay_Color color; // color of the input box
  Clay_Color diabled_color; // color of the input box / text when disabled
  Clay_TextElementConfig *textConfig;
  Clay_TextElementConfig *disabled_textConfig;
  input_submit_t submit; // callback for when input box is submitted
  intptr_t userdata; // anything you want to access in callback
  size_t buf_size;
  size_t count;
  bool focused;
  bool enabled;
  size_t framecount; // for rendering blinking cursor
  char *disabled_text; // text shown in box when disabled
  char *buf; // buffer (allocate separately)
};

#define INPUT_BOX(input_id, szbuf, col) CLITERAL(InputConfig) {\
  .id = (input_id),                                            \
  .buf = (char *)CALLOC((szbuf), sizeof(char)),                \
  .buf_size = (szbuf),                                         \
  .color = (col)                                               \
}

extern void poll_input(Input *input);
extern void RenderButton(const char *id, Clay_Color color, const char *text, Clay_TextElementConfig *textConfig, clay_onhover hoverCB, intptr_t user_data);
extern void RenderInputbox(Input *input);
extern void input_reset_buf(Input *input);




#endif // UI_H