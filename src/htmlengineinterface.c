/*
 *      htmlengineinterface.c
 *      
 *      Copyright 2009 Denis Pauk <pauk.denis@gmail.com>
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
#include <gtkhtml/gtkhtml.h>
#include "loaders.h"
#include "htmlengineinterface.h"

/* declaration */
/*change title*/
static void title_changed_cb(GtkHTML * html, const gchar * title, gpointer data);

/*установка базиса для ссылок*/
static void on_set_base(GtkHTML * html, const gchar * url, gpointer data);

/* page loaded:-) */
static void load_done(GtkHTML * html);

/*on url*/
static void on_url(GtkHTML * html, const gchar * url, gpointer data);

/*Передвинуться на позицию в html*/
static void change_position(GtkHTML * html, const gchar * position, gpointer data);

/*получить даннык по ссылке*/
static void getdata(GtkHTML * html, const gchar * method, const gchar * action,
	const gchar * encoding, GtkHTMLStream * stream, gpointer data,
	gboolean redirect_save);
	
/* request content from url */
static void url_requested(GtkHTML * html, const char *url, GtkHTMLStream * stream,
	      gpointer data);

/* click on link */
static void on_link_clicked(GtkHTML * html, const gchar * url, gpointer data);

/* submit from form */
static void on_submit(GtkHTML * html, const gchar * method, const gchar * action,
	  const gchar * encoding, gpointer data);

/* action redirect */
static void on_redirect(GtkHTML * html, const gchar * url, int delay, gpointer data);

/*print page*/
static void print_preview_cb (GtkHTML * html);

/*draw one page*/
static void
draw_page_cb (GtkPrintOperation *operation, GtkPrintContext *context,
              gint page_nr, gpointer user_data);
			  
/*interface*/

struct All_variable *html_engine_intreface_construct()
{
	struct All_variable  * variable = g_new(struct All_variable, 1); 
	variable->list = g_list_alloc ();
	return variable;
}

void clean(struct All_variable  * variable)
{
	//TODO Clean resurcive
	g_list_free (variable->list);
}

void html_engine_interface_print(struct All_variable * variable)
{
	
	GtkPrintOperation *operation;

	operation = gtk_print_operation_new ();
	gtk_print_operation_set_n_pages (operation, 1);

	g_signal_connect (
		operation, "draw-page",
		G_CALLBACK (draw_page_cb), variable->html);

	gtk_print_operation_run (
		operation, GTK_PRINT_OPERATION_ACTION_PREVIEW, NULL, NULL);

	g_object_unref (operation);
}

void html_engine_interface_back(struct All_variable * variable)
{
	/*current*/
	GList*  last = g_list_last (variable->list);
	if( last != NULL )
	{
		gconstpointer data = last->data;
		if(data != NULL)
		{
			variable->list = g_list_remove (variable->list, data);
			g_free(data);
		}
	}
	/*previous*/
	last = g_list_last (variable->list);
	if( last != NULL )
	{
		gconstpointer data = last->data;
		if(data != NULL)
		{
			variable->list = g_list_remove (variable->list, data);
			html_engine_interface_go(variable, data);
			g_free(data);
		}
	}

}

void html_engine_interface_go(struct All_variable * variable, gchar *go)
{
	if(go == NULL)
		return;
	on_link_clicked(
			GTK_HTML(variable->html),
		    go,
			variable);
}

GtkWidget* html_engine_interface_init(struct All_variable * variable, GtkWidget* app, GtkWidget* textentry)
{
	SoupCookieJar * cookie_jar;
	/* create GtkHTML widget */
    variable->html = gtk_html_new();
	variable->app = app;
	variable->textentry = textentry;
	
	/* init session */
    variable->session = soup_session_async_new ();
	cookie_jar = soup_cookie_jar_text_new("./cookies.txt", FALSE);
	soup_session_add_feature(variable->session, SOUP_SESSION_FEATURE(cookie_jar));
	/* end init session*/

	/*signals connect*/
	    g_signal_connect(variable->html, "url_requested",
		     G_CALLBACK(url_requested), variable);
    g_signal_connect(variable->html, "load_done", G_CALLBACK(load_done),
		     variable);
    g_signal_connect(variable->html, "title_changed",
		     G_CALLBACK(title_changed_cb), variable);
    g_signal_connect(variable->html, "on_url", G_CALLBACK(on_url), variable);
    g_signal_connect(variable->html, "set_base", G_CALLBACK(on_set_base),
		     variable);
    g_signal_connect(variable->html, "link_clicked",
		     G_CALLBACK(on_link_clicked), variable);
    g_signal_connect(variable->html, "redirect", G_CALLBACK(on_redirect),
		     variable);
    g_signal_connect(variable->html, "submit", G_CALLBACK(on_submit),
		     variable);
	return variable->html;
}

/*stop all query*/
void html_engine_interface_stop_query(gpointer data)
{
	    struct All_variable *variable = (struct All_variable *) data;
		if (variable->session != NULL)
			soup_session_abort(variable->session);
}

/*implementation*/

/*change title*/
static void
title_changed_cb(GtkHTML * html, const gchar * title, gpointer data)
{
	struct All_variable *variable = (struct All_variable *) data;
	g_print("title_changed_cb=%s\n", title);
	if( data != NULL )
		gtk_window_set_title (GTK_WINDOW (variable->app), title);
}

/*установка базиса для ссылок*/
static void
on_set_base(GtkHTML * html, const gchar * url, gpointer data)
{
    g_print("on_set_base = %s\n", url);
    gtk_html_set_base(html, url);
}

/* page loaded:-) */
static void
load_done(GtkHTML * html)
{
    g_print("Loaded\n");
}

/*on url*/
static void
on_url(GtkHTML * html, const gchar * url, gpointer data)
{
    g_print("on_url=%s\n", url);
}

/*Передвинуться на позицию в html*/
static void
change_position(GtkHTML * html, const gchar * position, gpointer data)
{
    g_print("Fix Me goto position '%s' in html not implemented\n", position);
}

/*получить даннык по ссылке*/
static void
getdata(GtkHTML * html, const gchar * method, const gchar * action,
	const gchar * encoding, GtkHTMLStream * stream, gpointer data,
	gboolean redirect_save)
{
    gchar *realurl;

    char *gotocharp = NULL;

    struct All_variable *variable = (struct All_variable *) data;

	if (action == NULL){
		g_print("\nBUG:Mot set action\n");
		return;	
	}

    if (!strcmp(method, "GET") || !strcmp(method, "POST")) {
		char *currpos;
		const gchar * baseurl = gtk_html_get_base(html);
		if ( baseurl == NULL)
			baseurl = g_strdup("");
		if (
			!strncmp(action, "http:",  strlen("http:"))  ||
			!strncmp(action, "https:", strlen("https:")) ||
			!strncmp(action, "ftp:",   strlen("ftp:"))   ||
			!strncmp(action, "file:",  strlen("file:"))  ||
			!strncmp(action, "data:",  strlen("data:"))  ||
			strlen(baseurl) == 0 /*unknow base url*/
		) {
			/*all right it's full url*/
			realurl = g_strdup(action);
		} else {			
			realurl =
				(gchar *) g_new0(gchar,
						strlen(action) +
						strlen(baseurl) + 4);
			strncpy(realurl, baseurl,strlen(baseurl));
			/*contein slash - is path from domain*/
			if (*action == '/'){
				/*fing first '//' (as example file://) */
				gchar* startdomain = strstr(realurl,"//");
				if(startdomain != NULL){
					gchar* enddomain;
					/* 2 -- length '//' */
					startdomain += 2 ;
					enddomain = strchr(startdomain,'/');
					if(enddomain == NULL)
						enddomain = startdomain;
					strcpy(enddomain, action);
					g_print("resulted fullurl=%s action=%s baseurl=%s\n", realurl, action, baseurl);
				}
			} else {
				/*TODO Test It*/
				/*not started from slash*/
				gchar * lastn;
				gchar * from;
				gchar * lastqu;
				gchar * lastslash;
				from = realurl;
				/*search '?'*/
				lastqu = strchr(from,'?');
				if(lastqu != NULL)
					from = lastqu;
				/*search '#'*/
				lastn = strchr(from,'#');
				if(lastn != NULL)
					from = lastn;
				/*search '/'*/
				lastslash = strrchr(from,'/');
				if(lastslash == NULL)
				{
					lastslash = from + strlen(from);
					/*add slash on end string*/
					*lastslash = '/';
				}
				strcat(lastslash,action);
			}
		}		
		g_print("submitting '%s' to '%s' using method '%s' by '%s' \n",
			encoding, action, method, realurl);
		currpos = realurl;
		/*split by '?'*/
		while (*currpos != 0) {
			if (*currpos == '?')
				break;
			if (*currpos == '#') {
				*currpos = 0;
				gotocharp = currpos + 1;
				break;
			}
			currpos++;
		}
		{
			loaders *loaders_e = loaders_ref(loaders_new());	
			/* Enable change content type in engine */
			gtk_html_set_default_engine(html, TRUE);
			loaders_init_internal (loaders_e, variable->session, html, stream, redirect_save);
			/* load data*/
			loaders_render(loaders_e, realurl, method, encoding);
			if (gotocharp)
				change_position(html, gotocharp, data);
		}
    } else {
		g_print("Unknow Metod for url '%s'", realurl);
    }
    free(realurl);
}

/* request content from url */
static void
url_requested(GtkHTML * html, const char *url, GtkHTMLStream * stream,
	      gpointer data)
{
    if (url == NULL)
		return;
    getdata(html, "GET", url, "", stream, data, FALSE);
}

/* click on link */
static void
on_link_clicked(GtkHTML * html, const gchar * go, gpointer data)
{
	struct All_variable *variable = (struct All_variable *) data;
	gchar * url = g_strdup(go);
	variable->list = g_list_append ( variable->list, url);

    g_print("on_link_clicked=%s\n", url);
    /*for url-> base_url#id*/ 
    if (gtk_html_get_base(html))
		if (!strncmp(url,
				gtk_html_get_base(html),
				strlen(gtk_html_get_base(html))
				)
			)
				if (*(url + strlen(gtk_html_get_base(html))) == '#') {
					change_position(html,
						url + strlen(gtk_html_get_base(html)) + 1,
						data);
					return;
				}
    {
		GtkHTMLStream *stream;
		html_engine_interface_stop_query(data);
		stream = gtk_html_begin_content(html, "");
		getdata(html, "GET", url, "", stream, data, TRUE);
		gtk_entry_set_text (GTK_ENTRY(variable->textentry),g_strdup(url));
    }
}

/* submit from form */
static void
on_submit(GtkHTML * html, const gchar * method, const gchar * action,
	  const gchar * encoding, gpointer data)
{
	GtkHTMLStream *stream;
	html_engine_interface_stop_query(data);
    stream = gtk_html_begin_content(html, "");
    getdata(html, method, action, encoding, stream, data, TRUE);
}

/* action redirect */
static void
on_redirect(GtkHTML * html, const gchar * url, int delay, gpointer data)
{
	/*TODO make redirect*/
    if (delay == 0) {
		g_print("Redirecting to '%s'\n", url);
		on_link_clicked(html, url, data);
    } else {
		g_print("Redirecting to '%s' in %d seconds. Not realized yet.\n",
			url,
			delay);
    }
}

static void
print_footer (GtkHTML *html, GtkPrintContext *context, gdouble x, gdouble y,
              gdouble width, gdouble height, gpointer user_data)
{
	gchar *text;
	cairo_t *cr;
	PangoLayout *layout = (PangoLayout *)user_data;

	text = g_strdup(gtk_html_get_base(html));
	
	pango_layout_set_width (layout, width * PANGO_SCALE);
	pango_layout_set_text (layout, text, -1);

	cr = gtk_print_context_get_cairo_context (context);

	cairo_save (cr);
	cairo_move_to (cr, x, y);
	pango_cairo_show_layout (cr, layout);
	cairo_restore (cr);

	g_free (text);
}

static void
draw_page_cb (GtkPrintOperation *operation, GtkPrintContext *context,
              gint page_nr, gpointer user_data)
{
	/* XXX GtkHTML's printing API doesn't really fit well with GtkPrint.
	 *     Instead of calling a function for each page, GtkHTML prints
	 *     everything in one shot. */
	PangoLayout *layout;
	PangoFontDescription *desc;
	PangoFontMetrics *metrics;
	gdouble footer_height;
	GtkHTML * html = (GtkHTML *) user_data;

	desc = pango_font_description_from_string ("Helvetica 12");

	layout = gtk_print_context_create_pango_layout (context);
	pango_layout_set_alignment (layout, PANGO_ALIGN_CENTER);
	pango_layout_set_font_description (layout, desc);

	metrics = pango_context_get_metrics (
		pango_layout_get_context (layout),
		desc, gtk_get_default_language ());
	footer_height = (pango_font_metrics_get_ascent (metrics) +
		pango_font_metrics_get_descent (metrics)) / PANGO_SCALE;
	pango_font_metrics_unref (metrics);

	pango_font_description_free (desc);

	gtk_html_print_page_with_header_footer (
		html, context, .0, footer_height,
		NULL, print_footer, layout);

	g_object_unref (layout);
}

