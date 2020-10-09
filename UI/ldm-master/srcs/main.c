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
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include "main.h"
#include "window.h"
#include "events.h"
#include "keyboard.h"
#include "x.h"
#include "args.h"
#include "config.h"

xcb_connection_t *c;
xcb_screen_t *screen;
pthread_mutex_t lock_ctxs;

__attribute__((unused))
static void display_screen_infos(void)
{
  printf ("\n");
  printf ("Informations of screen %u:\n", screen->root);
  printf ("  width.........: %d\n", screen->width_in_pixels);
  printf ("  height........: %d\n", screen->height_in_pixels);
  printf ("  white pixel...: %u\n", screen->white_pixel);
  printf ("  black pixel...: %u\n", screen->black_pixel);
  printf ("\n");
}

static int gtfo(t_args const * const args, int const exit_status)
{
  if (args->x == true)
    stop_x_server();
  xcb_disconnect(c);
  xcb_void_cookie_t const cookie_gc = xcb_free_gc(c, font_ctx);
  xcb_generic_error_t const * error = xcb_request_check(c, cookie_gc);
  if (error)
    fprintf(stderr, "ERROR: can't free font_ctx : %d\n", error->error_code);
  return exit_status;
}

int main(int const argc, char **argv)
{
  t_args *args;

  init_args(&args, argc, argv);
  if (parse_args(argc, argv, args) == false)
    return (84);
  if (args->x == true)
    start_x_server(args);
  // Wait for X to be launched
  sleep(3);
  c = xcb_connect(NULL, NULL);
  if (xcb_connection_has_error(c) > 0)
  {
    fprintf(stderr, "xcb_connect failed\n");
    xcb_disconnect(c);
    return 1;
  }
  screen = xcb_setup_roots_iterator(xcb_get_setup(c)).data;

#ifdef DEBUG
  display_screen_infos();
#endif
  create_window();
  create_font_context(FONT);
  setup_keyboard();
  if (pthread_mutex_init(&lock_ctxs, NULL) != 0)
  {
    fprintf(stderr, "mutex init failed\n");
    return gtfo(args, EXIT_FAILURE);
  }
  event_loop();

  // use atexit(). fork() behaviour ?
  pthread_mutex_destroy(&lock_ctxs);
  return gtfo(args, EXIT_SUCCESS);
}
