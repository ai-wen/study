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
#include <stdio.h>
#include <stdbool.h>
#include <err.h>
#include <xcb/xkb.h>
#include <xkbcommon/xkbcommon.h>
#include <xkbcommon/xkbcommon-compose.h>
#include <xkbcommon/xkbcommon-x11.h>

#include "main.h"
#include "keyboard.h"

struct xkb_state *xkb_state;
struct xkb_compose_state *xkb_compose_state;
static struct xkb_context *xkb_context;

/*
 * Loads the XKB keymap from the X11 server and feeds it to xkbcommon.
 * Necessary so that we can properly let xkbcommon track the keyboard state and
 * translate keypresses to utf-8.
 */
bool load_keymap(void)
{
  static struct xkb_keymap *xkb_keymap = NULL;

  if (xkb_context == NULL) {
    if ((xkb_context = xkb_context_new(0)) == NULL) {
      fprintf(stderr, "could not create xkbcommon context\n");
      return false;
    }
  }

  xkb_keymap_unref(xkb_keymap);

  int32_t device_id = xkb_x11_get_core_keyboard_device_id(c);
  if ((xkb_keymap = xkb_x11_keymap_new_from_device(xkb_context, c, device_id, 0)) == NULL) {
    fprintf(stderr, "xkb_x11_keymap_new_from_device failed\n");
    return false;
  }

  struct xkb_state *new_state =
    xkb_x11_state_new_from_device(xkb_keymap, c, device_id);
  if (new_state == NULL) {
    fprintf(stderr, "xkb_x11_state_new_from_device failed\n");
    return false;
  }

  xkb_state_unref(xkb_state);
  xkb_state = new_state;

  return true;
}

/*
 * Loads the XKB compose table from the given locale.
 */
static bool load_compose_table(char const * const locale)
{
  static struct xkb_compose_table *xkb_compose_table = NULL;

  xkb_compose_table_unref(xkb_compose_table);

  if ((xkb_compose_table = xkb_compose_table_new_from_locale(xkb_context, locale, 0)) == NULL) {
    fprintf(stderr, "xkb_compose_table_new_from_locale failed\n");
    return false;
  }

  struct xkb_compose_state *new_compose_state = xkb_compose_state_new(xkb_compose_table, 0);
  if (new_compose_state == NULL) {
    fprintf(stderr, "xkb_compose_state_new failed\n");
    return false;
  }

  xkb_compose_state_unref(xkb_compose_state);
  xkb_compose_state = new_compose_state;

  return true;
}

void setup_keyboard(void)
{
  uint8_t xkb_base_event;
  uint8_t xkb_base_error;

  if (xkb_x11_setup_xkb_extension(c,
        XKB_X11_MIN_MAJOR_XKB_VERSION,
        XKB_X11_MIN_MINOR_XKB_VERSION,
        0,
        NULL,
        NULL,
        &xkb_base_event,
        &xkb_base_error) != 1)
    errx(EXIT_FAILURE, "Could not setup XKB extension.");

  static const xcb_xkb_map_part_t required_map_parts =
    (xcb_xkb_map_part_t)(XCB_XKB_MAP_PART_KEY_TYPES |
     XCB_XKB_MAP_PART_KEY_SYMS |
     XCB_XKB_MAP_PART_MODIFIER_MAP |
     XCB_XKB_MAP_PART_EXPLICIT_COMPONENTS |
     XCB_XKB_MAP_PART_KEY_ACTIONS |
     XCB_XKB_MAP_PART_VIRTUAL_MODS |
     XCB_XKB_MAP_PART_VIRTUAL_MOD_MAP);

  static const xcb_xkb_event_type_t required_events =
    (xcb_xkb_event_type_t)(XCB_XKB_EVENT_TYPE_NEW_KEYBOARD_NOTIFY |
     XCB_XKB_EVENT_TYPE_MAP_NOTIFY |
     XCB_XKB_EVENT_TYPE_STATE_NOTIFY);

  xcb_xkb_select_events(
      c,
      (uint16_t)xkb_x11_get_core_keyboard_device_id(c),
      required_events,
      0,
      required_events,
      required_map_parts,
      required_map_parts,
      0);

  if (!load_keymap())
    fprintf(stderr, "Could not load keymap");

  char const *locale = getenv("LC_ALL");
  if (!locale || !*locale)
    locale = getenv("LC_CTYPE");
  if (!locale || !*locale)
    locale = getenv("LANG");
  if (!locale || !*locale) {
    fprintf(stderr, "Can't detect your locale, fallback to C\n");
    locale = "C";
  }

  load_compose_table(locale);
}
