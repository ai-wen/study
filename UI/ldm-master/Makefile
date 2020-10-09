#
# This file is part of the ldm distribution (https://github.com/valkheim/ldm)
# Copyright (c) 2018 Charles Paulet.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, version 3.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
# General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.
#

CC = gcc
RM = rm -rf
NAME = ldm
RULES = all clean fclean dbg re test check

PREFIX ?= /usr
BINDIR = $(PREFIX)/bin
INSTALL = install -p
INSTALL_PROGRAM = $(INSTALL) -m755
INSTALL_DATA = $(INSTALL) -m644
INSTALL_DIR = $(INSTALL) -d

D_OBJS = objs
D_SRCS = srcs
D_INCS = incs

SRCS = $(addprefix $(D_SRCS)/, \
			 main.c \
			 events.c \
			 window.c \
			 keyboard.c \
			 pam.c \
			 x.c \
			 args.c \
			 draw.c \
			 )

OBJS = $(SRCS:$(D_SRCS)/%.c=$(D_OBJS)/%.o)

INC = -I $(D_INCS)
INC += -I /usr/include/xcb

CFLAGS = -W -Wall -Werror -Wextra -pedantic
CFLAGS += $(INC)

LDFLAGS = `pkg-config --libs xcb xcb-util xcb-keysyms xcb-xkb xkbcommon xkbcommon-x11` -lpam -lpthread

all: $(NAME)

install: all
	$(INSTALL_DIR) $(DESTDIR)$(BINDIR)
	$(INSTALL_PROGRAM) $(NAME) $(DESTDIR)$(BINDIR)/$(NAME)

	$(INSTALL_DIR) $(DESTDIR)$(PREFIX)/lib/systemd/system/
	$(INSTALL_DATA) $(NAME).service $(DESTDIR)$(PREFIX)/lib/systemd/system/

	$(INSTALL_DIR) $(DESTDIR)$(PREFIX)/share/xsessions/
	$(INSTALL_DATA) $(NAME).desktop $(DESTDIR)$(PREFIX)/share/xsessions/

uninstall:
	rm -f $(DESTDIR)$(BINDIR)/$(NAME)
	rm -f $(DESTDIR)$(PREFIX)/lib/systemd/system/$(NAME).service
	rm -f $(DESTDIR)$(PREFIX)/share/xsessions/$(NAME).desktop

$(NAME): $(OBJS)
	$(CC) -o $(NAME) $(OBJS) $(CFLAGS) $(LDFLAGS)
	@echo -e "\033[38;5;10m=== $(NAME): Compiled !\033[0m"

$(D_OBJS)/%.o: $(D_SRCS)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	find -name "*~" -delete -o -name "#*#" -delete
	$(RM) $(D_OBJS)
	$(RM) $(OBJS)

fclean: clean uninstall
	$(RM) $(NAME)

re: fclean all

test: re
	sudo Xephyr -br -ac -noreset -screen "1024x640" :2 &
	sleep 1
	DISPLAY=:2 sudo ./$(NAME) -d ":2"

check:
	@echo -e "No tests."

dbg: CFLAGS += -g3 -O0 -DDEBUG
dbg: re
	@echo -e "\033[1;49;36m=== $(NAME): Debug mode\033[0m"

postnazi: CC := clang -Weverything
postnazi: re

nazi:
	@echo -e "\033[1;49;41m=== Attention: Nazi mode activated\033[0m"⏎
	$(MAKE) postnazi

help:
	@echo "Available rules : $(NAME) $(RULES)"

.PHONY: $(RULES)
