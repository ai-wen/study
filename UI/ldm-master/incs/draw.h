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

#define CTX_IDLE (0)
#define CTX_TYPE (1)
#define CTX_PROCESSING (2)
#define CTX_DENIED (3)
#define CTX_GRANTED (4)
#define CTX_TEXT (5)
#define CTXS_NUMBER (6)

extern xcb_gcontext_t main_ctx;
extern xcb_gcontext_t ctxs[CTXS_NUMBER];
extern xcb_generic_error_t const * font_err;

typedef struct
{
  int from;
  int to;
} t_draw_options;

void draw(void);
void draw_borders(t_draw_options *args);
