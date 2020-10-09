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
#include <errno.h>
#include <err.h>
#include <string.h>
#include "args.h"
#include "config.h"

bool args_usage(t_args *args)
{
  printf("USAGE: %s\n", args->av[0]);
  puts("\thelp\t\tDisplay this help");
  puts("\txoff\t\tDo not init X server");
  puts("\tdisplay\t\tSet display (:1 by default)");
  puts("\tvt\t\tSet virtual terminal (vt01 by default)");
  return (false);
}

bool args_x(t_args *args)
{
  args->x = false;
  return (true);
}

bool args_display(t_args *args)
{
  args->display = args->optarg;
  return (true);
}

bool args_virtual_terminal(t_args *args)
{
  args->vt = args->optarg;
  return (true);
}

bool parse_args(int ac, char **av, t_args *args)
{
  int opt;
  bool ret = true;

  struct option long_opt[] = {
    {"help", no_argument, NULL, 'h'},
    {"xoff", no_argument, NULL, 'x'},
    {"display", required_argument, NULL, 'd'},
    {"vt", required_argument, NULL, 't'},
    {NULL, 0, NULL, 0}};
  while ((opt = getopt_long(ac, av, "hxd:t:", long_opt, NULL)) != -1)
  {
    size_t i = 0;
    while (i < (sizeof(g_args_parsing_table) / sizeof(*g_args_parsing_table)))
    {
      args->optarg = optarg;
      args->optind = optind;
      if (opt == g_args_parsing_table[i].opt)
        if (((*g_args_parsing_table[i].func)(args)) == false)
          ret = false;
      i++;
    }
  }
  return (ret);
}

void init_args(t_args **args, int ac, char **av)
{
  if ((*args = malloc(sizeof(**args))) == NULL)
    errx(84, "malloc failed : %s\n", strerror(errno));
  (*args)->ac = ac;
  (*args)->av = av;
  (*args)->x = true;
  (*args)->display = DISPLAY;
  (*args)->vt = VT;
}
