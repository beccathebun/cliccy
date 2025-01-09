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


extern void RenderButton(const char *id, Clay_Color color, const char *text, Clay_TextElementConfig *textConfig, clay_onhover hoverCB, intptr_t user_data) {
    CLAY(CLAY_ID(id),
    CLAY_LAYOUT({ 
      .sizing = {
        .width = {
          .size ={
            .minMax = {
              .min = 160,
              .max = 250,
              }},
          .type = CLAY__SIZING_TYPE_FIT
        },
        .height = {
          .size ={
            .minMax = {
              .min = 60,
              .max = 100,
              }},
          .type = CLAY__SIZING_TYPE_GROW
        }
      },
      .padding = {10, 15},
      .childAlignment = {
        .x = CLAY_ALIGN_X_CENTER,
        .y = CLAY_ALIGN_Y_CENTER
      },
    }),
      CLAY_RECTANGLE({ 
          .color        = color,
          .cornerRadius = CLAY_CORNER_RADIUS(15.f),
          .lineThick    = LINE_THIC,
          }),
        Clay_OnHover(hoverCB, user_data)) {
        CLAY_TEXT(CLAY_STRING(text), textConfig);
    }
}

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

extern void poll_input(Input *input){
  if(!input->enabled) return;
  assert(input->buf != NULL && "inputbox: forgot to alloc buffer?");
  bool btnDown = IsMouseButtonDown(0);
  bool pointerOver = Clay_PointerOver(Clay_GetElementId(CLAY_STRING(input->id)));
  if (btnDown && pointerOver) {
    input->focused = true;
    logs(LOG_TRACE, "input focused!");
  } else if(btnDown) {
    input->focused = false;
    logs(LOG_TRACE, "input unfocused!");
    
  }
  if(pointerOver) SetMouseCursor(MOUSE_CURSOR_IBEAM);
  else SetMouseCursor(MOUSE_CURSOR_DEFAULT);

  if(input->focused) {
    input->framecount++;
    int key = GetCharPressed();
    logs(LOG_TRACE, "key: %i", key);
    while (key > 0) {
      // NOTE: Only allow keys in range [32..125]
      if ((key >= 32) && (key <= 125) && (input->count < input->buf_size-1)) {
        //logs(LOG_INFO, "key pressed: %c", (char)key);
        input->buf[input->count++] = (char)key;
      }
        
      key = GetCharPressed();
    }
    if (IsKeyPressed(KEY_BACKSPACE)){
      if(input->count > 0) input->count--;
      input->buf[input->count] = '\0';
    }
    if (IsKeyPressed(KEY_ENTER)){
      input->submit(input);
    }
  } else if(input->framecount) input->framecount = 0;
}

extern void RenderInputbox(Input *input) {
  CLAY(CLAY_ID(input->id),
    CLAY_LAYOUT({ 
      .sizing = {
        .width = CLAY_SIZING_GROW({}),
        .height = {
          .size ={
            .minMax = {
              .min = 60,
              .max = 100,
              }},
          .type = CLAY__SIZING_TYPE_GROW
        }
      },
      .padding = {10, 15},
      .childAlignment = {
        .x = CLAY_ALIGN_X_CENTER,
        .y = CLAY_ALIGN_Y_CENTER
      },
      .childGap = 2
    }),
      CLAY_RECTANGLE({ 
          .color        = input->enabled ? input->color : input->diabled_color,
          .cornerRadius = CLAY_CORNER_RADIUS(15.f),
          .lineThick    = LINE_THIC,
          })) {
        if(input->enabled) {
          CLAY_TEXT(CLAY_STRING(input->buf), input->textConfig);
          if (((input->framecount/20)%2) == 0 && input->focused){
            CLAY_TEXT(CLAY_STRING("|"), TEXT_CONF(input->textConfig->fontSize + 2, Clay_GetColor(0xffffffff)));
          }
        } else {
          CLAY_TEXT(CLAY_STRING(input->disabled_text), input->textConfig);
        }
        
          
    }
}
extern void input_reset_buf(Input *input) {
  input->count = 0;
  memset(input->buf, 0, input->buf_size);
}
#endif // UI_H