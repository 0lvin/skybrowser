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

/*#define DebugVariable*/

static gboolean
on_exit_window(GtkWidget * window, gpointer data)
{
    struct All_variable *variable = (struct All_variable *) data;
#ifdef DebugVariable
    g_print("variable=%x \n", variable);
    g_print("variable->session=%x \n", variable->session);
    if (data == NULL)
		g_print("Eroor in file (%s) line (%d)", __FILE__, __LINE__);
#endif
	html_engine_interface_stop_query(data);
    gtk_main_quit();
    return FALSE;
}

/*маин он и в Африке маин:-)*/
int
main(int argc, char **argv)
{
    struct All_variable *variable = html_engine_intreface_construct();

    /* Инициализируем поддержку i18n */
    gtk_set_locale();

    /* Инициализируем установки виджета */
    gtk_init(&argc, &argv);

    GtkWidget *app = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	
	GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				   (scrolled_window),
				   GTK_POLICY_AUTOMATIC,
				   GTK_POLICY_AUTOMATIC);
    gtk_container_add(GTK_CONTAINER(scrolled_window),
		      html_engine_intreface_init( variable, app));
    gtk_container_add(GTK_CONTAINER(app),
		      scrolled_window);
    gtk_window_set_default_size(GTK_WINDOW(app), 800, 600);
	
    g_signal_connect(app, "delete-event",
		     G_CALLBACK(on_exit_window), variable);
    /*{
		gchar *tmpstr = g_new0 (gchar, 255);
		memcpy(tmpstr, "file:", sizeof("file:"));
		getcwd(tmpstr + strlen("file:"), 255 - strlen("file:") - 1);
		strcat(tmpstr, "/");
		gtk_html_set_base(GTK_HTML(variable->html), tmpstr);
		g_free(tmpstr);
    }*/
#ifdef DebugVariable
    g_print("variable=%x \n", variable);
    g_print("variable->session=%x \n", variable->session);
#endif
    gtk_widget_show_all(app);
	
	html_engine_intreface_go(variable, argc > 1 ? argv[1] : "http://foto.mail.ru");
    /* run the main loop */
    gtk_main();
}
