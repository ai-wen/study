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

#define pam_err(name) \
  do { \
    fprintf(stderr, "%s: %s\n", name, \
        pam_strerror(pam_handle, result)); \
    end(result); \
    break; \
  } while (1); \

bool login(char const * const username, char const * const password);
bool pam_login(char const * const username, char const * const password, pid_t * const child_pid);
bool pam_logout(void);
