
#include <stdio.h>
/*
gcc -o simple simple.c `pkg-config --libs --cflags gtk+-2.0`

pkg-config --libs --cflags gtk+-2.0
-pthread 
-I/usr/include/gtk-2.0 
-I/usr/lib/x86_64-linux-gnu/gtk-2.0/include 
-I/usr/include/gio-unix-2.0/ 
-I/usr/include/cairo 
-I/usr/include/pango-1.0 
-I/usr/include/atk-1.0 
-I/usr/include/cairo 
-I/usr/include/pixman-1 
-I/usr/include/libpng12 
-I/usr/include/gdk-pixbuf-2.0 
-I/usr/include/libpng12 
-I/usr/include/pango-1.0 
-I/usr/include/harfbuzz 
-I/usr/include/pango-1.0 
-I/usr/include/glib-2.0 
-I/usr/lib/x86_64-linux-gnu/glib-2.0/include 
-I/usr/include/freetype2 
-lgtk-x11-2.0 -lgdk-x11-2.0 -lpangocairo-1.0 -latk-1.0 -lcairo -lgdk_pixbuf-2.0 -lgio-2.0 
-lpangoft2-1.0 -lpango-1.0 -lgobject-2.0 -lglib-2.0 -lfontconfig -lfreetype

附加包含目录
/usr/include/gtk-2.0;/usr/lib/x86_64-linux-gnu/gtk-2.0/include;/usr/include/gio-unix-2.0/;/usr/include/cairo;/usr/include/pango-1.0;/usr/include/atk-1.0;/usr/include/pixman-1;/usr/include/libpng12;/usr/include/gdk-pixbuf-2.0;/usr/include/harfbuzz;/usr/include/glib-2.0;/usr/lib/x86_64-linux-gnu/glib-2.0/include;/usr/include/freetype2;

库依赖
pthread
gtk-x11-2.0
gdk-x11-2.0
pangocairo-1.0
atk-1.0
cairo
gdk_pixbuf-2.0
gio-2.0
pangoft2-1.0
pango-1.0
gobject-2.0
glib-2.0
fontconfig
freetype
*/

#include <gtk-2.0/gtk/gtk.h>
#include <gtk-2.0/gtk/gtkmain.h>
#include <glib-2.0/glib.h>
#include <glib-2.0/glib/gprintf.h>

//教程 https://zetcode.com/gui/gtk2/

//gtk界面设计
//https://blog.csdn.net/drsonxu/article/details/86608123
//https://blog.csdn.net/ztguang/article/details/53180000

void print_msg(GtkWidget *widget, gpointer window) {
	g_printf("Button clicked\n");
}
int show_popup(GtkWidget *widget, GdkEvent *event) {

	const gint RIGHT_CLICK = 3;

	if (event->type == GDK_BUTTON_PRESS) {

		GdkEventButton *bevent = (GdkEventButton *)event;

		if (bevent->button == RIGHT_CLICK) {

			gtk_menu_popup(GTK_MENU(widget), NULL, NULL, NULL, NULL,
				bevent->button, bevent->time);
		}

		return TRUE;
	}

	return FALSE;
}
int main(int argc ,char** argv)
{
#if 0
	gtk_init(&argc, &argv);
	g_printf("GTK+ version: %d.%d.%d\n", gtk_major_version, gtk_minor_version, gtk_micro_version);
	g_printf("Glib version: %d.%d.%d\n", glib_major_version, glib_minor_version, glib_micro_version);

//#endif

	GtkWidget *window;
	GdkPixbuf *icon;
	GError *error = NULL;

	GtkWidget *button;
	GtkWidget *button_mnemonic;
	GtkWidget *halign;
	GtkWidget *halign_mnemonic;

	gtk_init(&argc, &argv);

	//居中
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window), "UserTool");
	gtk_window_set_default_size(GTK_WINDOW(window), 230, 150);
	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
	
	//主窗口四周占宽
	gtk_container_set_border_width(GTK_CONTAINER(window), 15);

	//设置任务栏图标
	icon = gdk_pixbuf_new_from_file("/home/longmai/icon.png", &error);
	if (!icon) {
		fprintf(stderr, "%s\n", error->message);
		g_error_free(error);
	}
	else
	{
		gtk_window_set_icon(GTK_WINDOW(window), icon);
	}

	//提示
/*
	button = gtk_button_new_with_label("Button");
	gtk_widget_set_tooltip_text(button, "Button widget");
	//GtkAlignment是一个基本容器，可用于将其子容器与窗口的侧面对齐。
	//在我们的例子中，按钮放在窗口的左上角。函数的第一个参数是xalign和yalign。xalign的值为0表示左对齐；yalign的值为0表示顶部对齐。第三和第四个参数是缩放值。向两个参数传递0表示小部件不会在两个方向上展开。
	halign = gtk_alignment_new(0, 0, 0, 0);
	gtk_container_add(GTK_CONTAINER(halign), button);
	gtk_container_add(GTK_CONTAINER(window), halign);
*/
	//Mnemonic助记键 使用Alt+首字母 快速执行事件,要求必须以下划线开头
	button_mnemonic = gtk_button_new_with_mnemonic("_Button");
	gtk_widget_set_tooltip_text(button_mnemonic, "_Button");
	g_signal_connect(button_mnemonic, "clicked", G_CALLBACK(print_msg), NULL);

	halign_mnemonic = gtk_alignment_new(0, 0, 0, 0);
	gtk_container_add(GTK_CONTAINER(halign_mnemonic), button_mnemonic);
	gtk_container_add(GTK_CONTAINER(window), halign_mnemonic);
	//显示窗口
	//gtk_widget_show(window);
	//当我们处理多个小部件时，在容器上调用gtk_widget_show_all()比单独显示所有小部件更容易。在我们的例子中，窗口和按钮都显示在一个镜头中。
	gtk_widget_show_all(window);
	//绑定退出信号
	g_signal_connect(G_OBJECT(window), "destroy",G_CALLBACK(gtk_main_quit), NULL);

	g_object_unref(icon);

	gtk_main();
//#endif

	//Popup menu
	GtkWidget *window;
	GtkWidget *ebox;
	GtkWidget *pmenu;
	GtkWidget *hideMi;
	GtkWidget *quitMi;

	gtk_init(&argc, &argv);

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
	gtk_window_set_default_size(GTK_WINDOW(window), 300, 200);
	gtk_window_set_title(GTK_WINDOW(window), "Popup menu");

	ebox = gtk_event_box_new();
	gtk_container_add(GTK_CONTAINER(window), ebox);

	pmenu = gtk_menu_new();

	hideMi = gtk_menu_item_new_with_label("Minimize");
	gtk_widget_show(hideMi);
	gtk_menu_shell_append(GTK_MENU_SHELL(pmenu), hideMi);

	quitMi = gtk_menu_item_new_with_label("Quit");
	gtk_widget_show(quitMi);
	gtk_menu_shell_append(GTK_MENU_SHELL(pmenu), quitMi);

	g_signal_connect_swapped(G_OBJECT(hideMi), "activate",
		G_CALLBACK(gtk_window_iconify), GTK_WINDOW(window));

	g_signal_connect(G_OBJECT(quitMi), "activate",
		G_CALLBACK(gtk_main_quit), NULL);

	g_signal_connect(G_OBJECT(window), "destroy",
		G_CALLBACK(gtk_main_quit), NULL);

	g_signal_connect_swapped(G_OBJECT(ebox), "button-press-event",
		G_CALLBACK(show_popup), pmenu);

	gtk_widget_show_all(window);

	gtk_main();
//#endif

	//布局
	GtkWidget *window;
	GtkWidget *fixed;
	GtkWidget *vbox;

	GtkWidget *btn1;
	GtkWidget *btn2;
	GtkWidget *btn3;

	gtk_init(&argc, &argv);

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window), "GtkFixed");
	gtk_window_set_default_size(GTK_WINDOW(window), 300, 200);
	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);

	vbox = gtk_vbox_new(TRUE, 1);
	gtk_container_add(GTK_CONTAINER(window), vbox);

	btn1 = gtk_button_new_with_label("Button");
	btn2 = gtk_button_new_with_label("Button");
	btn3 = gtk_button_new_with_label("Button");
	gtk_box_pack_start(GTK_BOX(vbox), btn1, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), btn2, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), btn3, TRUE, TRUE, 0);

	/*
	fixed = gtk_fixed_new();
	gtk_container_add(GTK_CONTAINER(window), fixed);

	btn1 = gtk_button_new_with_label("Button");
	gtk_fixed_put(GTK_FIXED(fixed), btn1, 150, 50);
	gtk_widget_set_size_request(btn1, 80, 30);

	btn2 = gtk_button_new_with_label("Button");
	gtk_fixed_put(GTK_FIXED(fixed), btn2, 15, 15);
	gtk_widget_set_size_request(btn2, 80, 30);

	btn3 = gtk_button_new_with_label("Button");
	gtk_fixed_put(GTK_FIXED(fixed), btn3, 100, 100);
	gtk_widget_set_size_request(btn3, 80, 30);
	*/
	g_signal_connect(G_OBJECT(window), "destroy",
		G_CALLBACK(gtk_main_quit), NULL);

	gtk_widget_show_all(window);

	gtk_main();

#endif


	GtkWidget *window;
	GtkWidget *okBtn;
	GtkWidget *clsBtn;

	GtkWidget *vbox;
	GtkWidget *hbox;
	GtkWidget *halign;
	GtkWidget *valign;

	gtk_init(&argc, &argv);

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
	gtk_window_set_default_size(GTK_WINDOW(window), 350, 200);
	gtk_window_set_title(GTK_WINDOW(window), "Corner buttons");
	gtk_container_set_border_width(GTK_CONTAINER(window), 10);

	vbox = gtk_vbox_new(FALSE, 5);

	valign = gtk_alignment_new(0, 1, 0, 0);
	gtk_container_add(GTK_CONTAINER(vbox), valign);
	gtk_container_add(GTK_CONTAINER(window), vbox);

	hbox = gtk_hbox_new(TRUE, 3);

	okBtn = gtk_button_new_with_label("OK");
	gtk_widget_set_size_request(okBtn, 70, 30);
	gtk_container_add(GTK_CONTAINER(hbox), okBtn);
	clsBtn = gtk_button_new_with_label("Close");
	gtk_container_add(GTK_CONTAINER(hbox), clsBtn);

	halign = gtk_alignment_new(1, 0, 0, 0);
	gtk_container_add(GTK_CONTAINER(halign), hbox);

	gtk_box_pack_start(GTK_BOX(vbox), halign, FALSE, FALSE, 0);

	g_signal_connect(G_OBJECT(window), "destroy",
		G_CALLBACK(gtk_main_quit), G_OBJECT(window));

	gtk_widget_show_all(window);

	gtk_main();
	return 0;
}







