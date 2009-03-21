/*
 *      skybrowser.c
 *      
 *      Copyright 2008,2009 Denis Pauk <pauk.denis@gmail.com>
 *      
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation; either version 2 of the License, or
 *      (at your option) any later version.
 *      
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *      
 *      You should have received a copy of the GNU General Public License
 *      along with this program; if not, write to the Free Software
 *      Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 *      MA 02110-1301, USA.
 */
#include <stdlib.h>
#include <glib.h>
#include <gtk/gtk.h>
#include "htmlengineinterface.h"

/*#define DebugVariable*/

static gboolean
on_exit_window(GtkWidget * window, gpointer data)
{
	html_engine_interface_stop_query(data);
    gtk_main_quit();
    return FALSE;
}

static void
print_action (GtkWidget *widget, gpointer data)
{
  html_engine_interface_print(data);
}

static void
quit_action (GtkWidget *widget, gpointer data)
{
  html_engine_interface_stop_query(data);
  gtk_main_quit();
}

static void
forward_action(GtkWidget *widget, gpointer data)
{
	struct All_variable *variable = (struct All_variable *) data;
	gchar* text = g_strdup(
						gtk_entry_get_text(GTK_ENTRY(variable->textentry))
				);
	html_engine_interface_go(variable, text);
}

static void
back_action(GtkWidget *widget, gpointer data)
{
	struct All_variable *variable = (struct All_variable *) data;
	html_engine_interface_back(variable);
}

/*standart main function*/
int
main(int argc, char **argv)
{
	GtkWidget *scrolled_window;
	GtkWidget *textentry;
	GtkWidget *app;
	GtkWidget *table;
	GtkWidget *action_table;
	GtkWidget *button_quit;
	GtkWidget *button_forward;
	GtkWidget *button_back;
	GtkWidget *button_print;
	struct All_variable *variable;
    /* initialization support i18n */
    gtk_set_locale();

    /* init gtk */
    gtk_init(&argc, &argv);

    /*main window*/
    app = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size(GTK_WINDOW(app), 640, 480);
	
	variable = html_engine_intreface_construct();	
    g_signal_connect(app, "delete-event",
		     G_CALLBACK(on_exit_window), variable);
			 
	/*main table*/
	table = gtk_table_new (1, 2, FALSE);
	gtk_container_add (GTK_CONTAINER (app), table);
	/*action group*/
	action_table = gtk_table_new (5, 1, FALSE);
	/*back*/
	button_back = GTK_WIDGET (gtk_tool_button_new_from_stock(GTK_STOCK_GO_BACK));
	gtk_table_attach (GTK_TABLE (action_table),
                        button_back,
                        /* X direction */       /* Y direction */
                        0, 1,                   0, 1,
                        GTK_SHRINK,  			GTK_SHRINK,
                        0,                      0);
	g_signal_connect (G_OBJECT (button_back), "clicked",
						G_CALLBACK (back_action), variable);
	/*entry*/
	textentry = gtk_entry_new ();
	gtk_table_attach (GTK_TABLE (action_table),
                        textentry,
                        /* X direction */       /* Y direction */
                        1, 2,                   0, 1,
                        GTK_EXPAND | GTK_FILL,  GTK_EXPAND | GTK_FILL,
                        0,                      0);
	/*forward*/
	button_forward = GTK_WIDGET (gtk_tool_button_new_from_stock(GTK_STOCK_GO_FORWARD));
	gtk_table_attach (GTK_TABLE (action_table),
                        button_forward,
                        /* X direction */       /* Y direction */
                        2, 3,                   0, 1,
                        GTK_SHRINK,  			GTK_SHRINK,
                        0,                      0);
	g_signal_connect (G_OBJECT (button_forward), "clicked",
						G_CALLBACK (forward_action), variable);
	/*print*/
	button_print = GTK_WIDGET (gtk_tool_button_new_from_stock(GTK_STOCK_PRINT));
	gtk_table_attach (GTK_TABLE (action_table),
                        button_print,
                        /* X direction */       /* Y direction */
                        3, 4,                   0, 1,
                        GTK_SHRINK,  			GTK_SHRINK,
                        0,                      0);
	g_signal_connect (G_OBJECT (button_print), "clicked",
						G_CALLBACK (print_action), variable);
	/*quit*/
	button_quit = GTK_WIDGET (gtk_tool_button_new_from_stock(GTK_STOCK_QUIT));
	gtk_table_attach (GTK_TABLE (action_table),
                        button_quit,
                        /* X direction */       /* Y direction */
                        4, 5,                   0, 1,
                        GTK_SHRINK,  			GTK_SHRINK,
                        0,                      0);
	g_signal_connect (G_OBJECT (button_quit), "clicked",
						G_CALLBACK (quit_action), variable);
	/*add action table to main table*/
	gtk_table_attach (GTK_TABLE (table),
                        action_table,
                        /* X direction */       /* Y direction */
                        0, 1,                   0, 1,
                        GTK_EXPAND | GTK_FILL,	GTK_SHRINK,
                        0,                      0);
    /*scrolled content window*/
	scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				   (scrolled_window),
				   GTK_POLICY_AUTOMATIC,
				   GTK_POLICY_AUTOMATIC);
	
	gtk_table_attach (GTK_TABLE (table),
                        scrolled_window,
                        /* X direction */       /* Y direction */
                        0, 1,                   1, 2,
                        GTK_EXPAND | GTK_FILL,  GTK_EXPAND | GTK_FILL,
                        0,                      0);
						
	/* add html render*/

    gtk_container_add(GTK_CONTAINER(scrolled_window),
			GTK_WIDGET(
				html_engine_interface_init( variable, app, textentry)
			));
	/*show new window*/
    gtk_widget_show_all(app);
	
	html_engine_interface_go(variable, argc > 1 ? argv[1] : "http://foto.mail.ru");
    /* run the main loop */
    gtk_main();
}
