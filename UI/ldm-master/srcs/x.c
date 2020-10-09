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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include "args.h"
#include "x.h"

static pid_t x_server_pid;

void stop_x_server(void)
{
  if (x_server_pid != 0)
    kill(x_server_pid, SIGKILL);
}

static void sig_handler(/*int signo*/)
{
  stop_x_server();
}

void start_x_server(t_args *args)
{
  signal(SIGSEGV, sig_handler);
  signal(SIGTRAP, sig_handler);
  char cmd[32]; // may overflow
  switch(x_server_pid = fork())
  {
    case 0:
      snprintf(cmd, sizeof(cmd), "/usr/bin/X %s %s", args->display, args->vt);
      execl("/bin/sh", "/bin/sh", "-c", cmd, NULL);
      break;
    case -1:
      fprintf(stderr, "No fork done\n");
      break;
    default:
      setenv("DISPLAY", args->display, true);
      break;
  }
}
