#include "ui.h"

void RenderButton(const char *id, Clay_Color color, const char *text, Clay_TextElementConfig *textConfig, clay_onhover hoverCB, intptr_t user_data) {
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


static float FMoveTowards(float v, float target)
{
  // sqrtf((*f1 - *f2)*(*f2-*f1))
  float maxDistance = sqrtf((v - target) * (target - v)) / 10;
  float d           = target - v;
  if ((d == 0) || ((maxDistance >= 0) && (d <= maxDistance)))
    return target;
  return d / d * maxDistance;
}
extern void animate_towards(intptr_t from, const intptr_t to, Animatable type)
{
  switch (type)
  {
    case Anim_Vec2: {
      Vector2 *v1       = (Vector2 *)from;
      const Vector2 *v2 = (Vector2 *)to;
      if (!Vector2Equals(*v1, *v2))
      {
        *v1 = Vector2MoveTowards(*v1, *v2, Vector2Distance(*v1, *v2) / 10);
      }
    }
    break;
    case Anim_Flt: {
      float *f1       = (float *)from;
      const float *f2 = (float *)to;
      if (!FloatEquals(*f1, *f2))
      {
        *f1 = FMoveTowards(*f1, *f2);
      }
    }
    break;
    case Anim_Color: {
      Vector3 c1 = ColorToHSV(*(Color *)from);
      Vector3 c2 = ColorToHSV(*(Color *)to);
      if (!FloatEquals(c1.x, c2.x))
      {
        c1.x = FMoveTowards(c1.x, c2.x);
      }
      if (!FloatEquals(c1.y, c2.y))
      {
        c1.y = FMoveTowards(c1.y, c2.y);
      }
      if (!FloatEquals(c1.z, c2.z))
      {
        c1.z = FMoveTowards(c1.z, c2.z);
      }
      Color *c = (Color *)from;
      *c       = ColorFromHSV(c1.x, c1.y, c1.z);
    }
    break;
  }
}