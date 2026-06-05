#include "gdk/gdkkeysyms.h"
#include <errno.h>
#include <gtk-4.0/gtk/gtk.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

typedef struct{
    int count;
    GtkTextView *text_view;
}AppData;


bool editable(GtkTextView *text_view, int *count){
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(text_view);
    GtkTextIter iter;
    gtk_text_buffer_get_iter_at_mark(buffer, &iter,gtk_text_buffer_get_insert(buffer));
    int line = gtk_text_iter_get_line(&iter);
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
    printf("the line is : %s \n",command);
    return command;
}


void run_command(GtkTextView *text_view, int *count,char *command){
    GtkTextBuffer *buffer;
    GtkTextIter end, iter;
    printf("the command is : %s\n",command);

    int fd[2];
    if(pipe(fd) == -1){
        perror("pipe");
        return;
    }
    buffer = gtk_text_view_get_buffer(text_view);

    if(strcmp(command, "exit") == 0){
        exit(0);
    }
    else{
        int i = 0;
        char *args[100];
        char *token = strtok(command, " ");
        while(token != NULL){
            args[i++] = token;
            token = strtok(NULL, " ");
        }
        args[i] = NULL;

        pid_t pid = fork();
        if(pid == 0){
            close(fd[0]);
            dup2(fd[1], STDOUT_FILENO);
            dup2(fd[1], STDERR_FILENO);
            close(fd[1]);
            if(execvp(args[0], args) == -1){
                perror("shell");
            }
            exit(EXIT_FAILURE);
        }
        else if(pid < 0){
            perror("fork");
        }
        else {
            close(fd[1]);
            char output[4096];
            int n = read(fd[0], output, sizeof(output)-1);
            gtk_text_buffer_insert_at_cursor(buffer, "\n", -1);

            while(n > 0){
                output[n] = '\0';
                gtk_text_buffer_get_end_iter(buffer, &end);
                gtk_text_buffer_insert(buffer, &end, output, -1);
                n = read(fd[0], output, sizeof(output)-1);
            }
            int status;
            waitpid(pid, &status, 0);
            gtk_text_buffer_get_iter_at_mark(buffer, &iter, gtk_text_buffer_get_insert(buffer));
            gtk_text_buffer_get_iter_at_mark(buffer, &end, gtk_text_buffer_get_insert(buffer));
            *count = gtk_text_iter_get_line(&end);
            printf("the count after command %d \n",*count);
        }
     
    }    
}

gboolean enter_pressed(GtkEventController *controller, guint key_val, guint key_code, GdkModifierType state, gpointer user_data){
    AppData *data = (AppData*) user_data;
    bool edit = editable(GTK_TEXT_VIEW(data->text_view), &data->count);
    
    if(edit && (key_val == GDK_KEY_Return || key_val == GDK_KEY_KP_Enter)){
        char *command = get_command(data->text_view, &data->count);
        run_command(data->text_view, &data->count, command);
        return true;
             
    }
    return false;
}


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
    
    GtkEventController *controller;
    controller = gtk_event_controller_key_new();
    AppData *data = g_malloc(sizeof(AppData));
    data->count = *count;
    data->text_view = GTK_TEXT_VIEW(text_view);
    g_signal_connect(controller, "key-pressed", G_CALLBACK(enter_pressed),data);
    gtk_widget_add_controller(text_view, controller);        

    

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