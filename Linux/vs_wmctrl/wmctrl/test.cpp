
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <locale.h>
#include <X11/Xlib.h>
#include <glib.h>
#include <X11/Xatom.h>

#include <X11/cursorfont.h>
#include <X11/Xmu/WinUtil.h>


#define p_verbose(...) if (1) { \
    fprintf(stderr, __VA_ARGS__); \
}

#define MAX_PROPERTY_VALUE_LEN 4096
#define SELECT_WINDOW_MAGIC ":SELECT:"
#define ACTIVE_WINDOW_MAGIC ":ACTIVE:"


static gchar *get_property(Display *disp, Window win, Atom xa_prop_type, gchar *prop_name, unsigned long *size) 
{
	Atom xa_prop_name;
	Atom xa_ret_type;
	int ret_format;
	unsigned long ret_nitems;
	unsigned long ret_bytes_after;
	unsigned long tmp_size;
	unsigned char *ret_prop;
	gchar *ret;

	xa_prop_name = XInternAtom(disp, prop_name, False);

	/* MAX_PROPERTY_VALUE_LEN / 4 explanation (XGetWindowProperty manpage):
	 *
	 * long_length = Specifies the length in 32-bit multiples of the
	 *               data to be retrieved.
	 */
	if (XGetWindowProperty(disp, win, xa_prop_name, 0, MAX_PROPERTY_VALUE_LEN / 4, False,
		xa_prop_type, &xa_ret_type, &ret_format,
		&ret_nitems, &ret_bytes_after, &ret_prop) != Success) 
	{
		p_verbose("Cannot get %s property.\n", prop_name);
		return NULL;
	}

	if (xa_ret_type != xa_prop_type) 
	{
		p_verbose("Invalid type of %s property.\n", prop_name);
		XFree(ret_prop);
		return NULL;
	}

	/* null terminate the result to make string handling easier */
	tmp_size = (ret_format / (32 / sizeof(long))) * ret_nitems;
	ret = (gchar*)g_malloc(tmp_size + 1);
	memcpy(ret, ret_prop, tmp_size);
	ret[tmp_size] = '\0';

	if (size) 
	{
		*size = tmp_size;
	}

	XFree(ret_prop);
	return ret;
}

static Window *get_client_list(Display *disp, unsigned long *size) 
{
	Window *client_list;

	if ((client_list = (Window *)get_property(disp, DefaultRootWindow(disp),XA_WINDOW, "_NET_CLIENT_LIST", size)) == NULL) 
	{
		if ((client_list = (Window *)get_property(disp, DefaultRootWindow(disp),XA_CARDINAL, "_WIN_CLIENT_LIST", size)) == NULL) 
		{
			fputs("Cannot get client list properties. \n""(_NET_CLIENT_LIST or _WIN_CLIENT_LIST)""\n", stderr);
			return NULL;
		}
	}

	return client_list;
}


static gchar *get_window_title(Display *disp, Window win) 
{
	gchar *title_utf8;
	gchar *wm_name;
	gchar *net_wm_name;

	wm_name = get_property(disp, win, XA_STRING, "WM_NAME", NULL);
	net_wm_name = get_property(disp, win,XInternAtom(disp, "UTF8_STRING", False), "_NET_WM_NAME", NULL);

	if (net_wm_name) 
	{
		title_utf8 = g_strdup(net_wm_name);
	}
	else 
	{
		if (wm_name) 
		{
			title_utf8 = g_locale_to_utf8(wm_name, -1, NULL, NULL, NULL);
		}
		else 
		{
			title_utf8 = NULL;
		}
	}

	g_free(wm_name);
	g_free(net_wm_name);

	return title_utf8;
}

static gboolean envir_utf8;

static void init_charset(void) 
{
	const gchar *charset; /* unused */
	gchar *lang = getenv("LANG") ? g_ascii_strup(getenv("LANG"), -1) : NULL;
	gchar *lc_ctype = getenv("LC_CTYPE") ? g_ascii_strup(getenv("LC_CTYPE"), -1) : NULL;

	/* this glib function doesn't work on my system ... */
	envir_utf8 = g_get_charset(&charset);

	/* ... therefore we will examine the environment variables */
	if (lc_ctype && (strstr(lc_ctype, "UTF8") || strstr(lc_ctype, "UTF-8"))) 
	{
		envir_utf8 = TRUE;
	}
	else if (lang && (strstr(lang, "UTF8") || strstr(lang, "UTF-8"))) 
	{
		envir_utf8 = TRUE;
	}

	g_free(lang);
	g_free(lc_ctype);

	if (0) 
	{
		envir_utf8 = TRUE;
	}
	p_verbose("envir_utf8: %d\n", envir_utf8);
}

static gchar *get_output_str(gchar *str, gboolean is_utf8) 
{
	gchar *out;

	if (str == NULL) 
	{
		return NULL;
	}

	if (envir_utf8) 
	{
		if (is_utf8) 
		{
			out = g_strdup(str);
		}
		else 
		{
			if (!(out = g_locale_to_utf8(str, -1, NULL, NULL, NULL))) 
			{
				p_verbose("Cannot convert string from locale charset to UTF-8.\n");
				out = g_strdup(str);
			}
		}
	}
	else 
	{
		if (is_utf8) 
		{
			if (!(out = g_locale_from_utf8(str, -1, NULL, NULL, NULL))) 
			{
				p_verbose("Cannot convert string from UTF-8 to locale charset.\n");
				out = g_strdup(str);
			}
		}
		else 
		{
			out = g_strdup(str);
		}
	}

	return out;
}


static gchar *get_window_class(Display *disp, Window win) 
{
	gchar *class_utf8;
	gchar *wm_class;
	unsigned long size;

	wm_class = get_property(disp, win, XA_STRING, "WM_CLASS", &size);
	if (wm_class) 
	{
		gchar *p_0 = strchr(wm_class, '\0');
		if (wm_class + size - 1 > p_0) 
		{
			*(p_0) = '.';
		}
		class_utf8 = g_locale_to_utf8(wm_class, -1, NULL, NULL, NULL);
	}
	else 
	{
		class_utf8 = NULL;
	}

	g_free(wm_class);

	return class_utf8;
}

int main()
{
	sleep(10);

	init_charset();

	Display *disp;
	if (!(disp = XOpenDisplay(NULL))) {
		fputs("Cannot open display.\n", stderr);
		return EXIT_FAILURE;
	}


	Window *client_list;


	unsigned long client_list_size;
	int i;
	int max_client_machine_len = 0;

	if ((client_list = get_client_list(disp, &client_list_size)) == NULL) 
	{
		return EXIT_FAILURE;
	}

	/* find the longest client_machine name */
	for (i = 0; i < client_list_size / sizeof(Window); i++) 
	{
		gchar *client_machine;
		if ((client_machine = get_property(disp, client_list[i], XA_STRING, "WM_CLIENT_MACHINE", NULL))) 
		{
			max_client_machine_len = strlen(client_machine);
		}
		g_free(client_machine);
	}

	/* print the list */
	for (i = 0; i < client_list_size / sizeof(Window); i++) 
	{
		gchar *title_utf8 = get_window_title(disp, client_list[i]); /* UTF8 */
		gchar *title_out =  get_output_str(title_utf8, TRUE);
		gchar *client_machine;
		gchar *class_out =  get_window_class(disp, client_list[i]); /* UTF8 */
		unsigned long *pid;
		unsigned long *desktop;
		int x, y, junkx, junky;
		unsigned int wwidth, wheight, bw, depth;
		Window junkroot;

		/* desktop ID */
		if ((desktop = (unsigned long *)get_property(disp, client_list[i],XA_CARDINAL, "_NET_WM_DESKTOP", NULL)) == NULL) 
		{
			desktop = (unsigned long *)get_property(disp, client_list[i],XA_CARDINAL, "_WIN_WORKSPACE", NULL);
		}

		/* client machine */
		client_machine = get_property(disp, client_list[i],XA_STRING, "WM_CLIENT_MACHINE", NULL);

		/* pid */
		pid = (unsigned long *)get_property(disp, client_list[i],XA_CARDINAL, "_NET_WM_PID", NULL);

		/* geometry */
		XGetGeometry(disp, client_list[i], &junkroot, &junkx, &junky,&wwidth, &wheight, &bw, &depth);
		XTranslateCoordinates(disp, client_list[i], junkroot, junkx, junky,&x, &y, &junkroot);

		/* special desktop ID -1 means "all desktops", so we have to convert the desktop value to signed long */

		printf("0x%.8lx %2ld", client_list[i],desktop ? (signed long)*desktop : 0);
		printf(" %-6lu", pid ? *pid : 0);
		printf(" %-4d %-4d %-4d %-4d", x, y, wwidth, wheight);
		printf(" %-20s ", class_out ? class_out : "N/A");
		
		/*
		if (options.show_pid) 
		{
			printf(" %-6lu", pid ? *pid : 0);
		}
		if (options.show_geometry) 
		{
			printf(" %-4d %-4d %-4d %-4d", x, y, wwidth, wheight);
		}
		if (options.show_class) 
		{
			printf(" %-20s ", class_out ? class_out : "N/A");
		}
		*/

		printf(" %*s %s\n",max_client_machine_len,client_machine ? client_machine : "N/A",title_out ? title_out : "N/A");

		Window    wRoot;
		Window    wParent;
		Window   *wChild;
		unsigned  nChildren;
		if (0 != XQueryTree(disp, client_list[i], &wRoot, &wParent, &wChild, &nChildren))
		{
			printf("---------%d\n", nChildren);
			for (unsigned i = 0; i < nChildren; i++)
			{
				gchar *title_utf8 = get_window_title(disp, wChild[i]);
				
			}
		}

		g_free(title_utf8);
		g_free(title_out);
		g_free(desktop);
		g_free(client_machine);
		g_free(class_out);
		g_free(pid);
	}


	

	g_free(client_list);


	return 0;
}



#include <vector>
using namespace std;

vector<Window> _result;

void search(Display *_display, Window w, Atom _atomPID, int _pid)
{
	Atom           type;
	int            format;
	unsigned long  nItems;
	unsigned long  bytesAfter;
	unsigned char *propPID = 0;
	if (Success == XGetWindowProperty(_display, w, _atomPID, 0, 1, False, XA_CARDINAL,
		&type, &format, &nItems, &bytesAfter, &propPID))
	{
		if (propPID != 0)
		{
			if (_pid == *((unsigned long *)propPID))
				_result.push_back(w);
			XFree(propPID);
		}
	}

	Window    wRoot;
	Window    wParent;
	Window   *wChild;
	unsigned  nChildren;
	if (0 != XQueryTree(_display, w, &wRoot, &wParent, &wChild, &nChildren))
	{
		for (unsigned i = 0; i < nChildren; i++)
			search(_display,wChild[i], _atomPID, _pid);
	}
}

void EnumWindows(int pid)
{
	//int pid = getpid();

	Display *display = XOpenDisplay(0);
	Atom _atomPID = XInternAtom(display, "_NET_WM_PID", True);
	if (_atomPID == None)
	{
		return;
	}

	search(display,XDefaultRootWindow(display), _atomPID, pid);

	for (int i = 0; i < _result.size(); i++)
	{
		unsigned long winId = (unsigned long)_result[i];

		//QWidget* win = QWidget::find(winId);
		//if (win != NULL && win->isWindow() && win->isVisible()) 
		//{
		//	m_pformList->append((QObject*)win);
		//}
	}
	XCloseDisplay(display);
}


int main4()
{


	return 0;
}