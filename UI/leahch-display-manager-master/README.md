Linux Display/Login Manager
===========================

This is a display/login manager for Linux, similar to GDM, KDM, or SLiM. It is a very simple display manager written in C using GTK3.

Originally writen by [Gulshan Singh](https://www.gulshansingh.com/).

You can find the original tutorial for how to make this display manager [here](https://www.gulshansingh.com/posts/how-to-write-a-display-manager/). The [tutorial](https://github.com/gsingh93/display-manager/tree/tutorial) branch more closely follows the tutorial.

This display manager has tested on Debian 8/9x with `i3` as the window manager. The window manager must be placed in `/etc/X11/Xsession` (i.e. it should contain `exec i3`).


