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

#define BORDER_WIDTH (10)

/* Tango palette color */
/* https://sobac.com/sobac/tangocolors.htm */
#define COLOR_IDLE (0) /* black */
#define COLOR_TYPE (0x555753) /* slate */
#define COLOR_PROCESSING (0xedd400) /* butter */
#define COLOR_DENIED (0xcc0000) /* scarlet red */
#define COLOR_GRANTED (0x73d216) /* chameleon */
#define COLOR_TEXT (0xd3d7cf) /* aluminium */

#define PASSWORD_MAX_LENGTH (512)

#define SERVICE_NAME ("ldm")
#define FONT ("6x13")
#define DISPLAY (":1")
#define VT ("vt01")
