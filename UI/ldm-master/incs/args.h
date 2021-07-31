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

#pragma once
#include <stdbool.h>
#include <getopt.h>

typedef struct
{
  char *optarg;
  char **av;
  int ac;
  int optind;
  bool x;
  char gap[7];
  char *display;
  char *vt;
} t_args;

typedef bool (*t_args_func)(t_args *);
typedef struct __attribute__((__packed__))
{
  char opt;
  t_args_func func;
} t_func_args;


bool args_usage(t_args *args);
bool args_x(t_args *args);
bool args_display(t_args *args);
bool args_virtual_terminal(t_args *args);

#ifdef __GNUC__
  #ifndef __clang__
    static __attribute__((__unused__)) t_func_args g_args_parsing_table[] = {
  #else
    static t_func_args g_args_parsing_table[] = {
  #endif
#endif
  {'h', args_usage},
  {'x', args_x},
  {'d', args_display},
  {'t', args_virtual_terminal},
  {0, NULL}
};

void init_args(t_args **args, int ac, char **av);
bool parse_args(int ac, char **av, t_args *args);
