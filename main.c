#include <gtk-4.0/gtk/gtk.h>
#include <stdio.h>



void activate(GtkApplication *app){
    GtkWidget *window;

    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "mini shell");
    gtk_window_set_default_size(GTK_WINDOW(window), 600, 400);
    
    GtkWidget *scroll;
    GtkWidget *text_view;
    scroll = gtk_scrolled_window_new();
    text_view = gtk_text_view_new();
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), text_view);
    gtk_window_set_child(GTK_WINDOW(window), scroll);

    gtk_window_present(GTK_WINDOW(window));
}



int main(int argc, char **argv){
    GtkApplication *app;
    app = gtk_application_new("com.yacine.shel", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app,"activate",G_CALLBACK(activate),NULL);
    
    int status = g_application_run(G_APPLICATION(app),argc,argv);
    g_object_unref(app);
    return status;
}