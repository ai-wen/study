编译
dpkg -l |grep ***
apt search ***
apt install  ****

 ./autogen.sh
 ./configuer


apt install yelp-tools
apt install gtk-doc-tools  
apt install libglib2.0-dev 

apt-get install libpam0g-dev

apt install libgcrypt20-dev  
apt install libxklavier-dev


apt install libxdmcp-dev  
apt install libx11-xcb-dev  
apt install xcb  
apt install libxklavier-dev




wmctrl
main.c:36:29: fatal error: X11/Xmu/WinUtil.h: No such file or directory
apt-get install libxmu-dev






# Windows平台下GTK 窗口在任务栏的隐藏问题

 分类： C/C++2011-06-24 17:11:55
一口气写上了这么长的一个标题，估计吓到不少人。这个问题并不常见，也不是一个很大很严重的问题。但是确实存在，并且gtk的文档里没有说清楚。这导致了我走了一些弯路。好了，进入正题。

我们通过gtk_window_new新建一个GtkWindow后，一般情况下（不经过我们的特殊处理），当把这个窗口显示出来时，就会在任务栏中显示窗口的图标。但是有些时候我们不想要这个图标，比如，菜单窗口，是不应该有这个图标的。

GtkWindow中提供了gtk_window_set_skip_taskbar这个函数，并且有这样的说明："Windows may set a hint asking the desktop environment not to display the window in the task bar. This function sets this hint.”这不正是我们要的吗？！很遗憾，在Linux上是可以的，但是在Windows上就不行了。

可以通过下面的代码进行验证：

#include

static gboolean flag = TRUE; 
gboolean destroy(GtkWidget *widget, gpointer data) 
{ 
    gtk_main_quit(); 
}

gboolean clicked(GtkButton *button, gpointer data) 
{ 
    GtkWidget *window = (GtkWidget *)data;

    g_print("skip taskbar:%d.\n", 
            gtk_window_get_skip_taskbar_hint(GTK_WINDOW(window)));

    if(flag) 
        gtk_widget_hide_all(window); 
    else 
        gtk_widget_show_all(window);

    flag = !flag; 
    return FALSE;

} 
int main(int argc,char ** argv ) 
{ 
    gtk_init(&argc,&argv); 
    GtkWidget *window, *button,*layout; 
    GtkWidget *parent; 
    parent = gtk_window_new(GTK_WINDOW_TOPLEVEL); 
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL); 
    gtk_window_resize(GTK_WINDOW(window),100,200); 
    gtk_window_set_title(GTK_WINDOW(parent),"parent"); 
    gtk_window_set_title(GTK_WINDOW(window),"child");    
    gtk_window_set_decorated(GTK_WINDOW(window),FALSE);

    gtk_widget_realize(window); 
    gtk_window_set_type_hint(GTK_WINDOW(window),GDK_WINDOW_TYPE_HINT_DROPDOWN_MENU);

    gtk_window_set_skip_taskbar_hint(GTK_WINDOW(window),TRUE); 
    layout = gtk_vbox_new(FALSE,0); 
    button = gtk_button_new_with_label("ok"); 
    gtk_box_pack_start(GTK_BOX(layout),button,TRUE,TRUE,0); 
    gtk_container_add(GTK_CONTAINER(parent),layout); 
    g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(clicked),window);

    g_signal_connect(G_OBJECT(parent),"destroy",G_CALLBACK(destroy),NULL);

    gtk_widget_show_all(window); 
    gtk_widget_show_all(parent); 
    gtk_main();

}

这段代码创建一个窗口，然后用gtk_window_set_skip_taskbar_hint将其在任务栏中隐藏。我们可以发现，在windows下，我们是无法通过gtk_window_set_skip_taskbar_hint函数让窗口在任务栏中隐藏的。

其实，这个问题在gtk的开发人员也是知道的。请看这里，https://bugzilla.gnome.org/show_bug.cgi?id=537183，但几年过去了，问题却没有解决。

通过查看相关源码，可以看到，在gtk_window_set_skip_taskbar_hint中，实际上会调用gdk_window_set_skip_taskbar_hint来完成主要的隐藏工作，而在gdk_window_set_skip_taskbar_hint函数中（在gdkwindow-win32.c文件中），让人很意外，本质上这个函数什么事情都没做。所以，想通过这个函数来实现在任务栏中的隐藏是不可能的。

参考这个链接，我们可以看到，gdk的官方文档里并不支持使用gdk_window_set_skip_taskbar_hint来隐藏任务栏图标，它们建议使用gdk_window_set_type_hint()，由窗口类型来决定是否应该在任务栏中显示图标。

这样做就可以了吗？不是的。在上面的代码中，我们看到里面已经过窗口的类型设置成GDK_WINDOW_TYPE_HINT_DROPDOWN_MENU（也可以使用gdk_window_set_type_hint，结果一样），可是照样在任务栏中可以看到这个下拉菜单的图标。

问题回到最初，什么样的窗口在任务栏中是需要隐藏的？换句话说，就是什么样的窗口在任务栏中是可以显示的。回答这个问题暂时没有找到windows下的明确规定，但是参考metacity的文档中的这段话：

Dialogs which have no transient parent or root window being 
their tranisent parent are the ones which will be visible in 
the tasklist.

我们可以推测，如果窗口不是transient 窗口，或者不是root窗口的transient 窗口，它就可以显示在任务栏中。换言之，如果我们想让一个窗口不显示在任务栏中，就指定它是某一个非root窗口的transient窗口。

在我们的测试代码中，在设置完窗口类型后，加入：

gtk_window_set_transient_for(GTK_WINDOW(window),GTK_WINDOW(parent));

现在，无论窗口是何种类型，都不会在任务栏中显示了。

我们再看一下gdk_window_set_skip_taskbar_hint的实现代码：

void 
gdk_window_set_skip_taskbar_hint (GdkWindow *window, 
                  gboolean   skips_taskbar) 
{ 
  static GdkWindow *owner = NULL; 
  //GdkWindowAttr wa;

  g_return_if_fail (GDK_IS_WINDOW (window));

  GDK_NOTE (MISC, g_print ("gdk_window_set_skip_taskbar_hint: %p: %s, doing nothing\n", 
               GDK_WINDOW_HWND (window), 
               skips_taskbar ? "YES" : "NO"));

  // ### TODO: Need to figure out what to do here. 
  return;

  if (skips_taskbar) 
    { 
#if 0 
      if (owner == NULL) 
        { 
          wa.window_type = GDK_WINDOW_TEMP; 
          wa.wclass = GDK_INPUT_OUTPUT; 
          wa.width = wa.height = 1; 
          wa.event_mask = 0; 
          owner = gdk_window_new_internal (NULL, &wa, 0, TRUE); 
        } 
#endif

      SetWindowLongPtr (GDK_WINDOW_HWND (window), GWLP_HWNDPARENT, (LONG_PTR) GDK_WINDOW_HWND (owner));

#if 0 /* Should we also turn off the minimize and maximize buttons? */ 
      SetWindowLong (GDK_WINDOW_HWND (window), GWL_STYLE, 
             GetWindowLong (GDK_WINDOW_HWND (window), GWL_STYLE) & ~(WS_MINIMIZEBOX|WS_MAXIMIZEBOX|WS_SYSMENU)); 
      SetWindowPos (GDK_WINDOW_HWND (window), NULL, 
            0, 0, 0, 0, 
            SWP_FRAMECHANGED | SWP_NOACTIVATE | SWP_NOMOVE | 
            SWP_NOREPOSITION | SWP_NOSIZE | SWP_NOZORDER); 
#endif 
    } 
  else 
    { 
      SetWindowLongPtr (GDK_WINDOW_HWND (window), GWLP_HWNDPARENT, 0); 
    } 
}

可以看到，作者原先的意思是如果需要skip taskbar，就为这个窗口找一个父窗口。通过下面的语句实现：

SetWindowLongPtr (GDK_WINDOW_HWND (window), GWLP_HWNDPARENT, (LONG_PTR) GDK_WINDOW_HWND (owner));

但是不知作者基于什么样的原因，在类型判断之后就return了。

也有人提出了另外的解决方法，就是通过gdk_win32_drawable_get_handle得到窗口的句柄，然后再用SetWindowLong这个Windows API来设置窗口的属性。这种方法是也是可行的（我只试过简单的），如果做得完善的话，说不定就修正了gtk_window_set_skip_taskbar_hint无法正常使用的问题了。

