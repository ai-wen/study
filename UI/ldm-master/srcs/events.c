/*
 * This file is part of the ldm distribution (https://github.com/valkheim/ldm)
 * Copyright (c) 2018 Charles Paulet.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdlib.h>
#include <string.h>
#include <xkbcommon/xkbcommon-compose.h>

#include "config.h"
#include "main.h"
#include "events.h"
#include "keyboard.h"
#include "window.h"
#include "pam.h"
#include "draw.h"
#include "users.h"

#define isutf(c) (((c)&0xC0) != 0x80)

size_t current_user = 0;

static unsigned int input_position = 0;
static char password[PASSWORD_MAX_LENGTH] = { 0 };

__attribute__((unused))
static void print_modifiers(xcb_keycode_t const detail, uint32_t state)
{
  const char **mod, *mods[] = {
    "Shift", "Lock", "Ctrl", "Alt",
    "Mod2", "Mod3", "Mod4", "Mod5",
    "Button1", "Button2", "Button3", "Button4", "Button5"
  };
  printf("xcb: keypress: code: %d mod: %d (", detail, state);
  for (mod = mods ; state; state >>= 1, mod++)
    if (state & 1)
      printf("%s", *mod);
  putchar(')');
}

static void decrements_utf8(char const * const s, unsigned int * const i)
{
  if (*i > 0)
    (void)(isutf(s[--(*i)]) || isutf(s[--(*i)]) || isutf(s[--(*i)]) || --(*i));
}

static bool handle_control_keysym(xkb_keysym_t const ksym, t_draw_options * const colors)
{
  switch (ksym) {
    case XKB_KEY_Left:
      current_user--;
      return true;
    case XKB_KEY_Up:
      return true;
    case XKB_KEY_Right:
      current_user++;
      return true;
    case XKB_KEY_Down:
      return true;
    case XKB_KEY_Return:
      colors->from = CTX_PROCESSING;
      if (login(users[current_user % (sizeof(users) / sizeof(users[0]))], password) == false)
      {
        colors->from = CTX_DENIED;
        bzero(password, PASSWORD_MAX_LENGTH);
        input_position = 0;
      }
      return true;
    case XKB_KEY_BackSpace:
      colors->from = CTX_PROCESSING;
      decrements_utf8(password, &input_position);
      password[input_position] = '\0';
      return true;
    default:
      return false;
  }
}

static void xkb_get_keysym(xcb_keycode_t const detail)
{
  char buffer[128];
  bool composed = false;
  int n = 0;
  t_draw_options * const colors = (t_draw_options *)malloc(sizeof(*colors));
  colors->from = CTX_TYPE;
  colors->to = CTX_IDLE;
  xkb_keysym_t ksym = xkb_state_key_get_one_sym(xkb_state, detail);
  if (handle_control_keysym(ksym, colors))
  {
    draw_borders(colors);
    return;
  }
  if (xkb_compose_state && xkb_compose_state_feed(xkb_compose_state, ksym) == XKB_COMPOSE_FEED_ACCEPTED) {
    switch (xkb_compose_state_get_status(xkb_compose_state)) {
      case XKB_COMPOSE_NOTHING:
      case XKB_COMPOSE_COMPOSING:
        break;
      case XKB_COMPOSE_COMPOSED:
        n = xkb_compose_state_get_utf8(xkb_compose_state, buffer, sizeof(buffer)) + 1;
        ksym = xkb_compose_state_get_one_sym(xkb_compose_state);
        composed = true;
        break;
      case XKB_COMPOSE_CANCELLED:
        xkb_compose_state_reset(xkb_compose_state);
        return;
    }
  }
  draw_borders(colors);

  if (!composed) {
    n = xkb_keysym_to_utf8(ksym, buffer, sizeof(buffer));
  }

  if (n < 2)
    return;

  if (input_position < PASSWORD_MAX_LENGTH)
  {
    memcpy(password + input_position, buffer, (size_t)(n - 1));
    input_position += (size_t)(n - 1);
  }
#ifdef DEBUG
  printf("current password = %.*s\ninput position = %u\n", input_position, password, input_position);
#endif
}

static void key_press_management(xcb_generic_event_t const * const e)
{
  xcb_key_press_event_t const * const ev = (xcb_key_press_event_t const * const)e;
#ifdef DEBUG
  print_modifiers(ev->detail, ev->state);
#endif
  xkb_get_keysym(ev->detail);
}

void event_loop(void)
{
  xcb_generic_event_t *e;

  /* enter the main loop */
  while ((e = xcb_wait_for_event(c)))
  {
    switch(e->response_type)
    {
      /* (re)draw the window */
      case XCB_EXPOSE:
        draw();
        break;
      case XCB_KEY_PRESS:
        if (!load_keymap())
          fprintf(stderr, "Could not load keymap");
        key_press_management(e);
        break;
    }
    free(e);
  }
}
