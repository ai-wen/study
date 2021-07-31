// leah
// 
//
#include <libgen.h> // dirname()
#include <signal.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>

#include <glib.h>
#include <glib/gprintf.h>

// function declarations
//
static gboolean start_greeter(gpointer _data);
static gboolean start_xserver(gpointer _data);

// default config path
static gchar* config_path = "/etc/tcdm/tcdm.conf";

// command options
static GOptionEntry entries[] =
{
	{ "config", 'c', 0, G_OPTION_ARG_FILENAME, &config_path, "Path to config file", NULL},
	{ NULL }
};

// Helper function for print error message and exit
//
void log_error(gchar * message, GError *error) {
	if(error != NULL) {
		g_error ("%s: %s\n",message,  error->message);
		exit (1);
	}
}

// Storage structure for X, Greeter data
//
typedef struct _Greeter {
	GPid x_pid;
	GPid c_pid;
	gchar *x_command;
	gchar *c_command;
	gchar *display;
	gchar *xauth;
	gchar **envp;
} Greeter;


// XATHORITY generation
//
static void xauth_gen(const char *display, const char *xauth) {
	char cmd[256];

	snprintf(cmd, sizeof(cmd),"/bin/touch '%s'",xauth);
	g_message("%s", cmd);
	system(cmd);
	snprintf(cmd, sizeof(cmd),"/usr/bin/xauth -f '%s' generate %s . trusted", xauth, display);
	g_message("%s", cmd);
	system(cmd);
	snprintf(cmd, sizeof(cmd),"/bin/chmod a+rw '%s'",xauth);
	g_message("%s", cmd);
	system(cmd);
}

// Watch Greeter to exit and respawn it
//
static void watch_greeter (GPid pid, gint status, gpointer _data) {
	  Greeter *data = (Greeter *) _data;
	  //g_message ("Greeter %" G_PID_FORMAT " exited %s", pid,
	  g_message ("Greeter %p exited %s", pid,
			               g_spawn_check_exit_status (status, NULL) ? "normally" : "abnormally");
	  g_spawn_close_pid(pid);
	  //start_greeter(_data);

	  kill(data->x_pid, SIGTERM);
	  //start_xserver(data);
       	  g_timeout_add_seconds (2, start_greeter, data);
}

// Start Greeter and add watch function on exit
//
static gboolean start_greeter(gpointer _data) {
	GError *error = NULL;
	Greeter *data = (Greeter *) _data;

	xauth_gen(data->display, data->xauth);

	g_spawn_async(NULL, g_strsplit(data->c_command, " ", 0), data->envp, G_SPAWN_DO_NOT_REAP_CHILD, NULL, NULL, &data->c_pid, &error);
	log_error("Greeter error", error);
	g_child_watch_add (data->c_pid, watch_greeter, data);

	return FALSE;
}

// Watch Xserver to exit and respawn it
//
static void watch_xserver (GPid pid, gint status, gpointer _data) {
	Greeter *data = (Greeter *) _data;
	g_message ("Child %p exited %s", pid,
	               g_spawn_check_exit_status (status, NULL) ? "normally" : "abnormally");
	g_spawn_close_pid(pid);
  	start_xserver(data);
}

// Start Xserver and add watch function on exit
//
static gboolean start_xserver(gpointer _data) {
	GError *error = NULL;
	Greeter *data = (Greeter *) _data;

	g_spawn_async(NULL, g_strsplit(data->x_command, " ", 0), NULL, G_SPAWN_DO_NOT_REAP_CHILD, NULL, NULL, &data->x_pid, &error);
	log_error("Xserver error", error);
	g_child_watch_add (data->x_pid, watch_xserver, data);

	return FALSE;
}

// MAIN function
//
int main(int argc, char *argv[]) {
	GError *error = NULL;
	GOptionContext *context;


	// get argv options and parse
	context = g_option_context_new ("- Thin Client Display Manager");
	g_option_context_add_main_entries (context, entries, NULL);
	if (!g_option_context_parse (context, &argc, &argv, &error))
		log_error("option parsing failed", error);

	g_message("Use config file: %s", config_path);

	GKeyFile *config = g_key_file_new();
	g_key_file_load_from_file (config, config_path, 0, &error);
	log_error("config error", error);

	gsize len =0;
	gchar ** vtX = g_key_file_get_string_list(config, "Manager", "Terminals", &len, &error);
	log_error("config error", error);

	gchar *xserver = g_key_file_get_string(config, "Manager", "XserverCommand", &error);
	log_error("config error", error);
	gchar *greeter = g_key_file_get_string(config, "Manager", "GreeterCommand", &error);
	log_error("config error", error);

	// main cycle for all VTXX
	for (int i =0; i < len; i++) {

		Greeter *data = g_slice_new (Greeter);
		data->x_pid = 0;
		data->c_pid = 0;
		data->envp = g_get_environ();

		data->display = g_slice_alloc(256);
		data->xauth = g_slice_alloc(256);
		g_sprintf(data->display, ":%d", i);
		g_sprintf(data->xauth, "/tmp/.Xauthority%s",data->display);

		data->envp = g_environ_setenv(data->envp, "DISPLAY", data->display, true);
		data->envp = g_environ_setenv(data->envp, "XAUTHORITY", data->xauth, true);

		data->x_command = g_slice_alloc(256);
    		g_sprintf(data->x_command, "%s %s %s",xserver, data->display, vtX[i]);

		data->c_command = greeter;

		g_message("%s - %s", data->display, data->xauth);
		g_message("%s", data->x_command);
		g_message("%s", data->c_command);

		start_xserver(data);
    		g_timeout_add_seconds (2, start_greeter, data);
		sleep(1);
	}

    system("chvt 7");
    // create main loop
    GMainLoop *loop = g_main_loop_new(NULL,FALSE);

    // main loop
    g_main_loop_run(loop);

    return 0;
}

