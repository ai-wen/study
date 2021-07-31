# ldm - Lean Display Manager

[![Codacy Badge](https://api.codacy.com/project/badge/Grade/cfa67e70c07541ffa6eb51260d2e65d7)](https://www.codacy.com/app/valkheim/ldm?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=valkheim/ldm&amp;utm_campaign=Badge_Grade)

Trying to get comfy with x11, xcb, xkbcommon, pam.

ldm starts an X server, open a pam session, source user's xinitrc, waits, closes pam session.

User selection (left/right arrow selection) :

Available on AUR under name lean-display-manager
```
(yay|trizen|pacaur) -S lean-display-manager
```

![preview](preview.png)

Border color is added when typing/erasing and on pam success/failure

## Lean

Using valgrind's massif tool and massif-visualizer.

![Memory consumption](memory_consumption.png)

## Troubleshoot

*  `xlsfonts` to list fonts of system
*  `incs/users.h` is list of users
*  `incs/config.h` is some config (font, colors, terminal, default display)
*  `make test` to execute in Xephyr

## License

Copyright (C) 2017-2018 Charles Paulet

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 3.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
