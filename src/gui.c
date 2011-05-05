/*
 * File: gui.c
 *
 * Author: Mike Swift
 * Email: theycallmeswift@gmail.com
 * Date Created: May 5th, 2011
 * Date Modified: May 5th, 2011
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <gtk/gtk.h>
#include "gui.h"

/************************
 * 1. Globals           *
 ************************/

char* indexdir;
Cache cache;
TokenizerT tok;
Filelist files;
GtkWidget *textview;

/************************
 * 2. Search Functions  *
 ************************/

void destroySearch()
{
    /* Ok, now we're done. Burn it down */
    destroyCache(cache);
    cache = NULL;
    
    destroyFilelist(files);
    files = NULL;
    
    TKDestroy(tok);
    tok = NULL;
}

void createSearch()
{
    char* cachesize = "0KB";
        
    /* Create a tokenizer */
    tok = TKCreate(FILE_CHARS, "myindex.txt");
    if(tok == NULL)
    {
        fprintf(stderr, "Error: Could not allocate space for Tokenizer.\n");
        return;
    }
    
    /* Get the file list */
    files = getFilelist(tok);
    if(files == NULL)
    {
        return;
    }
    
    /* Create a cache */
    cache = createCache(cachesize);
    if(cache == NULL)
    {
        fprintf(stderr, "Error: Could not allocate space for Cache.\n");
        return;
    }
    
    /* Update the allowed characters */
    adjustAllowedChars(tok, STRING_CHARS);
}

void reindex(GtkWidget *widget, gpointer data)
{
    char command[1024];
    
    destroySearch();
    createSearch();
    
    sprintf(command, "./bin/index myindex.txt %s", indexdir);
    
    system(command);
}

void orsearch(GtkWidget *widget, gpointer data)
{
    GtkTextBuffer *gbuffer;
    GtkTextMark *mark;
    GtkTextIter iter;
    
    gchar *search_text;
    char buffer[1024];
    Result result;
    
    search_text = gtk_entry_get_text(GTK_ENTRY(data));
    
    if(strlen(search_text) <= 0)
    {
        return;
    }
    
    sprintf(buffer, "so %s", (char*)search_text);
    
    search(buffer, tok, files, cache);
    
    gbuffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (textview));
    
    mark = gtk_text_buffer_get_insert (gbuffer);
    gtk_text_buffer_get_iter_at_mark (gbuffer, &iter, mark);

    gtk_text_buffer_insert (gbuffer, &iter, "**** RESULTS ****\n", -1);
    gtk_text_buffer_insert (gbuffer, &iter, "Query: ", -1);
    gtk_text_buffer_insert (gbuffer, &iter, buffer, -1);
    gtk_text_buffer_insert (gbuffer, &iter, "\n", -1);
    result = files->results;
    while(result != NULL)
    {
        if(result->frequency > 0)
        {
            gtk_text_buffer_insert (gbuffer, &iter, files->list[result->filenum], -1);
            gtk_text_buffer_insert (gbuffer, &iter, "\n", -1);
            printf("%s\n", files->list[result->filenum]);
        }
        result = result->next;
    }
    
    resetResults(files);
}

void andsearch(GtkWidget *widget, gpointer data)
{
    GtkTextBuffer *gbuffer;
    GtkTextMark *mark;
    GtkTextIter iter;
    
    
    gchar *search_text;
    char buffer[1024];
    Result result;
    
    search_text = gtk_entry_get_text(GTK_ENTRY(data));
    
    if(strlen(search_text) <= 0)
    {
        return;
    }
    
    sprintf(buffer, "sa %s", (char*)search_text);
    
    search(buffer, tok, files, cache);
    
    gbuffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (textview));
    
    mark = gtk_text_buffer_get_insert (gbuffer);
    gtk_text_buffer_get_iter_at_mark (gbuffer, &iter, mark);

    gtk_text_buffer_insert (gbuffer, &iter, "**** RESULTS ****\n", -1);
    gtk_text_buffer_insert (gbuffer, &iter, "Query: ", -1);
    gtk_text_buffer_insert (gbuffer, &iter, buffer, -1);
    gtk_text_buffer_insert (gbuffer, &iter, "\n", -1);
    result = files->results;
    while(result != NULL)
    {
        if(result->frequency > 0)
        {
            gtk_text_buffer_insert (gbuffer, &iter, files->list[result->filenum], -1);
            gtk_text_buffer_insert (gbuffer, &iter, "\n", -1);
            printf("%s\n", files->list[result->filenum]);
        }
        result = result->next;
    }
    
    resetResults(files);
}

/************************
 * 3. GUI Functions     *
 ************************/

static void destroy(GtkWidget *widget, gpointer data)
{
    destroySearch();
    gtk_main_quit ();
}

static void initialize_window(GtkWidget* window)
{
  gtk_window_set_title(GTK_WINDOW(window),"GUI Search"); 
  gtk_window_set_default_size (GTK_WINDOW (window), 500, 500);
  g_signal_connect (window, "destroy", G_CALLBACK (destroy), NULL);

}

int main (int argc, char *argv[])
{
    /* Vars */
    GtkWidget *vbox;
    GtkWidget *window;
    GtkWidget *table;
    GtkWidget *label;
    GtkWidget *entry;
    GtkWidget *button;
    GtkWidget* scrolled;
    GtkWidget *menubar;
    GtkWidget *filemenu;
    GtkWidget *file;
    GtkWidget *quit;
    GtkWidget *reindexer;
    
    /* Set default directory to index */
    indexdir = ".";
    
    /* Run the indexer */
    reindex(NULL, NULL);
    
    /* Create the search objects */
    createSearch();

    /* Make the GUI */
    gtk_init(&argc, &argv);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    initialize_window(window);

    /* Create a 10x5 table */
    table = gtk_table_new (10, 5, FALSE);
    gtk_container_add (GTK_CONTAINER (window), table);

    /* create a new label. */
    label = gtk_label_new ("Search Terms:" );

    gtk_table_set_homogeneous(GTK_TABLE (table), TRUE);
    gtk_table_attach_defaults (GTK_TABLE (table), label, 0, 1, 1, 2);

    entry = gtk_entry_new ();
    gtk_entry_set_max_length (GTK_ENTRY (entry),1000);
    gtk_table_attach_defaults (GTK_TABLE (table), entry, 1, 3, 1, 2);
    
    button = gtk_button_new_with_label ("OR Search");
    gtk_signal_connect (GTK_OBJECT (button), "clicked", GTK_SIGNAL_FUNC (orsearch), (gpointer) entry);
    gtk_table_attach_defaults (GTK_TABLE (table), button, 3, 4, 1, 2);
    
    button = gtk_button_new_with_label ("AND Search");
    gtk_signal_connect (GTK_OBJECT (button), "clicked", GTK_SIGNAL_FUNC (andsearch), (gpointer) entry);
    gtk_table_attach_defaults (GTK_TABLE (table), button, 4, 5, 1, 2);
    
    textview  = gtk_text_view_new();
    gtk_text_view_set_editable( GTK_TEXT_VIEW (textview), 0);
    gtk_text_view_set_cursor_visible (GTK_TEXT_VIEW (textview), 0);
    scrolled = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add (GTK_CONTAINER (scrolled), textview);
    gtk_table_attach_defaults (GTK_TABLE (table), scrolled, 0, 5, 2, 10);


    vbox = gtk_vbox_new(FALSE, 0);
    gtk_table_attach_defaults (GTK_TABLE (table), vbox, 0, 5, 0, 1);

    menubar = gtk_menu_bar_new();
    filemenu = gtk_menu_new();

    file = gtk_menu_item_new_with_label("File");
    reindexer = gtk_menu_item_new_with_label("Reindex");
    gtk_signal_connect (GTK_OBJECT (reindexer), "activate", G_CALLBACK(reindex), NULL);
    quit = gtk_menu_item_new_with_label("Quit");
    gtk_signal_connect (GTK_OBJECT (quit), "activate", G_CALLBACK(destroy), NULL);

    gtk_menu_item_set_submenu(GTK_MENU_ITEM(file), filemenu);
    gtk_menu_shell_append(GTK_MENU_SHELL(filemenu), reindexer);
    gtk_menu_shell_append(GTK_MENU_SHELL(filemenu), quit);
    gtk_menu_shell_append(GTK_MENU_SHELL(menubar), file);
    gtk_box_pack_start(GTK_BOX(vbox), menubar, FALSE, FALSE, 3);


    gtk_widget_show_all(window);

    gtk_main ();
    return 0;
}

