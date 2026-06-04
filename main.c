#include <gtk-4.0/gtk/gtk.h>
#include <stdbool.h>
#include <stdio.h>


bool editable(GtkTextView *text_view, int *count){
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(text_view);
    GtkTextIter iter;
    gtk_text_buffer_get_iter_at_mark(buffer, &iter,gtk_text_buffer_get_insert(buffer));
    int line = gtk_text_iter_get_line(&iter);
    printf("the line is %d \n",line);
    if(line != *count){
        gtk_text_view_set_editable(text_view, false);
        return false;
    }
    else{
        gtk_text_view_set_editable(text_view, true);
        return true;
    }
}

char  *get_command(GtkTextView *text_view, int *count){
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(text_view);
    GtkTextIter start,end;
    gtk_text_buffer_get_iter_at_line(buffer, &start, *count);
    end = start;
    gtk_text_iter_forward_to_line_end(&end);
    char *command = gtk_text_buffer_get_text(buffer, &start, &end, false);
    return command;
}
/*
void run_command(GtkTextView *text_view, int *count,char *command){
    
}
*/
void activate(GtkApplication *app, gpointer user_data){
    int *count = (int *) user_data;
    GtkWidget *window;

    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "mini shell");
    gtk_window_set_default_size(GTK_WINDOW(window), 600, 400);
    
    GtkWidget *scroll;
    GtkWidget *text_view;
    scroll = gtk_scrolled_window_new();
    text_view = gtk_text_view_new();
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), text_view);
    bool edit = editable(GTK_TEXT_VIEW(text_view), count);
    if(edit == true){
        printf("enter \n");
        char *command = get_command(GTK_TEXT_VIEW(text_view), count);
        /*
        run_command(GTK_TEXT_VIEW(text_view), count);
        */
        printf("%s",command);
        g_free(command);
    }
    gtk_window_set_child(GTK_WINDOW(window), scroll);

    gtk_window_present(GTK_WINDOW(window));
}



int main(int argc, char **argv){
    GtkApplication *app;
    int start = 0;
    app = gtk_application_new("com.yacine.shel", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app,"activate",G_CALLBACK(activate),&start);   
    int status = g_application_run(G_APPLICATION(app),argc,argv);
    g_object_unref(app);
    return status;
}