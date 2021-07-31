#include <libgen.h> // dirname()
#include <signal.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include <gtk/gtk.h>

#include "pam.h"

#define ENTER_KEY    65293
#define ESC_KEY      65307
/*
#define UI_CONF_DIR  "/etc/tcdm"
#define UI_FILE      "tcdm.ui"
#define UI_CSS       "tcdm.css"
*/
#define UI_CONF_DIR  "/home/console/display-manager"
#define UI_FILE      "gui1.ui"
#define UI_CSS       "gui.css"

static GtkWidget *window;
static GtkEntry *user_text_field;
static GtkEntry *pass_text_field;
static GtkLabel *status_label;

// default config path
static gchar* config_path = "/etc/tcdm/tcdm.conf";

// command options
static GOptionEntry entries[] =
{
	{ "config", 'c', 0, G_OPTION_ARG_FILENAME, &config_path, "Path to config file", NULL},
	{ NULL }
};

typedef struct _PamData {
	gchar *resource;
	gchar *pam_name;
	pam_handle_t *pamh;
} PamData;

// !!!threaded function!!!!
static void do_login(void *_data) {
	PamData *data = _data;

	pid_t pid = 0;
	int status;

	// start user processs!!!
	//
	pid = p_login(data->pamh, data->resource);
        waitpid(pid, &status, 0); // TODO: Handle errors
	p_logout(data->pamh);
        gdk_threads_add_idle (gtk_main_quit, NULL);
}


static void login_func(PamData *data) {
    //GtkWidget *widget = GTK_WIDGET(data);
    const gchar *username = gtk_entry_get_text(user_text_field);
    const gchar *password = gtk_entry_get_text(pass_text_field);

    gtk_label_set_text(status_label, "Logging in...");
    g_message("Logging in...");

    while(gtk_events_pending())
	      gtk_main_iteration ();

    // pid_t child_pid = 0;

    //pam_handle_t *pamh = NULL;
    data->pamh = p_check_login(username, password, data->pam_name);
    if(data->pamh != NULL) {
        g_message("Do login...");
    	gtk_widget_hide(window);
    	// gtk_main_quit();
	pthread_t thread;
	pthread_create(&thread, NULL, do_login, (void*) data);
	// child_pid = p_login(pamh);
	// p_logout(pamh);

	// exit(0);

    } else {
        g_message("Login ERROR...");
        gtk_widget_show(window);
        gtk_label_set_text(status_label, "Login error");
    }

    gtk_entry_set_text(pass_text_field, "");

    // return NULL;
}

static gboolean login_event(GtkWidget *widget, gpointer _data) {
	PamData *data = _data;

    login_func(data);

    // pthread_create(&login_thread, NULL, login_func, (void*) widget);
    return TRUE;
}

static gboolean set_pass_focus(GtkWidget *widget, GdkEventKey *event) {
    gtk_widget_grab_focus(GTK_WIDGET(pass_text_field));
    return TRUE;
}

static gboolean reboot_event(GtkWidget *widget, gchar *_data) {
    g_message("Do reboot... %s", _data);
    //system("/sbin/reboot -f");
    system(_data);
    return TRUE;
}
static gboolean halt_event(GtkWidget *widget, gchar *_data) {
    g_message("Do halt... %s", _data);
    //system("/sbin/poweroff -f");
    system(_data);
    return TRUE;
}

static gboolean cancel_event(GtkWidget *widget, GdkEventKey *event) {
    // clear user and password fields
    gtk_widget_destroy(window);
    gtk_main_quit();
    //exit(1);
/*
    gtk_label_set_text(status_label, "");
    gtk_entry_set_text(pass_text_field, "");
    gtk_widget_grab_focus(GTK_WIDGET(user_text_field));
*/
    return TRUE;
}

static void login_window(GKeyFile *config) {

/*
[Greeter]]
PamName=tcdm

GuiFile=/home/console/display-manager/gui1.ui
CSSFile=/home/console/display-manager/gui1.css

RebootCommand=/sbin/reboot -f
RebootButton=Reboot
RebootButton[ru]=Рестарт

HaltCommand=/sbin/poweroff -f
HaltButton=Off
HaltButton[ru]=Выкл.

ResourceFile=/home/console/display-manager/.xinitrc
*/

    gtk_init(NULL, NULL);

    GError *error = NULL;

    PamData pam_data;

    gchar *reboot_command = g_key_file_get_string(config, "Greeter", "RebootCommand", &error);
    gchar *halt_command = g_key_file_get_string(config, "Greeter", "HaltCommand", &error);
    gchar *gui_path = g_key_file_get_string(config, "Greeter", "GuiFile", &error);
    gchar *css_path = g_key_file_get_string(config, "Greeter", "CSSFile", &error);
    pam_data.pam_name = g_key_file_get_string(config, "Greeter", "PamName", &error);
    pam_data.resource = g_key_file_get_string(config, "Greeter", "ResourceFile", &error);

    /*
    char ui_file_path[256];
    if (readlink("/proc/self/exe", ui_file_path, sizeof(ui_file_path)) == -1) {
        printf("Error: could not get location of binary");
        exit(1);
    }

    dirname(ui_file_path);
    strcat(ui_file_path, "/" UI_FILE);
    */
/*
    const char *ui_file_path[256];
    const char *ui_css_path[256];
    strcpy((char *)ui_file_path, UI_CONF_DIR);
    strcpy((char *)ui_css_path, UI_CONF_DIR);
    strcat((char *)ui_file_path, "/" UI_FILE);
    strcat((char *)ui_css_path, "/" UI_CSS);
*/
    GtkBuilder *builder = gtk_builder_new_from_file(gui_path);
    window = GTK_WIDGET(gtk_builder_get_object(builder, "window"));
    user_text_field = GTK_ENTRY(gtk_builder_get_object(builder,
                                                       "user_text_entry"));
    pass_text_field = GTK_ENTRY(gtk_builder_get_object(builder,
                                                       "pass_text_entry"));
    status_label = GTK_LABEL(gtk_builder_get_object(builder, "status_label"));

    GtkButton *cancel_button = GTK_BUTTON(gtk_builder_get_object(builder, "login_cancel"));
    GtkButton *ok_button = GTK_BUTTON(gtk_builder_get_object(builder, "login_ok"));
    GtkButton *halt_button = GTK_BUTTON(gtk_builder_get_object(builder, "shutdown"));
    GtkButton *reboot_button = GTK_BUTTON(gtk_builder_get_object(builder, "reboot"));

///////// CSS Style
    GtkCssProvider *css = gtk_css_provider_new();
    GdkDisplay *dspl = gdk_display_get_default();
    GdkScreen *screen = gdk_display_get_default_screen(dspl);
    gtk_style_context_add_provider_for_screen (screen, GTK_STYLE_PROVIDER (css), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    GFile *fcss = g_file_parse_name(css_path);
    if (gtk_css_provider_load_from_file(css, fcss, NULL) == FALSE) {
	    printf("error style provider");
	    exit(1);
    }


    // Make full screen
//    GdkScreen *screen = gdk_screen_get_default();

//            GdkDisplay *dpy = gtk_widget_get_display(widget);
     GdkWindow *win = gtk_widget_get_window(GTK_WIDGET(window));
     GdkMonitor *monitor = gdk_display_get_monitor_at_window(dspl, win);
     GdkRectangle geometry;
     gdk_monitor_get_geometry(monitor, &geometry);
     gint width = geometry.width;
     gint height = geometry.height;

     g_message("+ Resolution: %dx%d", width, height);
/*
    gint height = gdk_screen_get_height(screen);
    gint width = gdk_screen_get_width(screen);
     g_message("- Resolution: %dx%d", width, height);
*/
    gtk_widget_set_size_request(GTK_WIDGET(window), width, height);
    gtk_widget_grab_focus(GTK_WIDGET(user_text_field));


    g_signal_connect(GTK_ENTRY(user_text_field), "activate", G_CALLBACK(set_pass_focus), NULL);
    g_signal_connect(GTK_ENTRY(pass_text_field), "activate", G_CALLBACK(login_event), &pam_data);

    g_signal_connect(GTK_BUTTON(ok_button), "clicked", G_CALLBACK(login_event), &pam_data);
    g_signal_connect(GTK_BUTTON(cancel_button), "clicked", G_CALLBACK(cancel_event), NULL);

    g_signal_connect(GTK_BUTTON(halt_button), "clicked", G_CALLBACK(halt_event), halt_command);
    g_signal_connect(GTK_BUTTON(reboot_button), "clicked", G_CALLBACK(reboot_event), reboot_command);

    g_signal_connect(window, "destroy", G_CALLBACK(cancel_event), NULL);
    gtk_main();

    g_message("TCDM GREETER DONE");

    exit(0);
}

// Helper function for print error message and exit
//
void log_error(gchar * message, GError *error) {
	if(error != NULL) {
			g_error ("%s: %s\n",message,  error->message);
			exit (1);
	}
}

int main(int argc, char *argv[]) {
	GError *error = NULL;
	GOptionContext *context;

	context = g_option_context_new ("- Thin Client DM Greeter");
	g_option_context_add_main_entries (context, entries, NULL);
    	g_option_context_parse (context, &argc, &argv, &error);
	log_error("option parsing failed", error);

	g_message("Config: %s", config_path);

	GKeyFile *config = g_key_file_new();
	g_key_file_load_from_file (config, config_path, 0, &error);
	log_error("config error", error);


	
    login_window(config);


    return 0;
}
