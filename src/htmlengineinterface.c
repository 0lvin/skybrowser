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
	
//запросить данные
static void url_requested(GtkHTML * html, const char *url, GtkHTMLStream * stream,
	      gpointer data);

//нажали на ссылку
static void on_link_clicked(GtkHTML * html, const gchar * url, gpointer data);

//послали с формы
static void on_submit(GtkHTML * html, const gchar * method, const gchar * action,
	  const gchar * encoding, gpointer data);

//перенаправление
static void on_redirect(GtkHTML * html, const gchar * url, int delay, gpointer data);
/*interface*/

struct All_variable *html_engine_intreface_construct()
{
	return g_new(struct All_variable, 1);
}

void html_engine_intreface_go(struct All_variable * variable, char *go)
{
	on_link_clicked(
			GTK_HTML(variable->html),
		    go,
			variable);
}

GtkWidget* html_engine_intreface_init(struct All_variable * variable, GtkWidget* app)
{
	/* create GtkHTML widget */
    variable->html = gtk_html_new();
	variable->app = app;
	
	/* init session */
    variable->session = soup_session_async_new ();
	SoupCookieJar * cookie_jar = soup_cookie_jar_text_new("./cookies.txt", FALSE);
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
#ifdef DebugVariable
    g_print("variable=%x \n", variable);
	
    if (data == NULL)
		g_print("Erorr in file (%s) line (%d)\n", __FILE__, __LINE__);
	if (action == NULL)
		g_print("Erorr in file (%s) line (%d)\n", __FILE__, __LINE__);
#endif	

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
			g_print("received %s %s \r\n",action,baseurl);
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
					/* 2 -- length '//' */
					startdomain +=2 ;
					gchar* enddomain = strchr(startdomain,'/');
					if(enddomain == NULL)
						enddomain = startdomain;
					strcpy(enddomain, action);
					g_print("resulted fullurl=%s action=%s baseurl=%s\n", realurl, action, baseurl);
				}
			} else {
				/*TODO Test It*/
				/*not started from slash*/
				gchar * from = realurl;
				/*search '?'*/
				gchar * lastqu = strchr(from,'?');
				if(lastqu != NULL)
					from = lastqu;
				/*search '#'*/
				gchar * lastn = strchr(from,'#');
				if(lastn != NULL)
					from = lastn;
				/*search '/'*/
				gchar * lastslash = strrchr(from,'/');
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
		loaders *loaders_e = loaders_ref(loaders_new());	
		/* Enable change content type in engine */
    	gtk_html_set_default_engine(html, TRUE);
		loaders_init_internal (loaders_e, variable->session, html, stream, redirect_save);
		/* load data*/
		loaders_render(loaders_e, realurl, method, encoding);
	
		if (gotocharp)
			change_position(html, gotocharp, data);
    } else {
		g_print("Unknow Metod for url '%s'", realurl);
    }
    free(realurl);
}

//запросить данные
static void
url_requested(GtkHTML * html, const char *url, GtkHTMLStream * stream,
	      gpointer data)
{
    if (url == NULL)
		return;
    getdata(html, "GET", url, "", stream, data, FALSE);
}

//нажали на ссылку
static void
on_link_clicked(GtkHTML * html, const gchar * url, gpointer data)
{
    g_print("on_link_clicked=%s\n", url);
    //for url-> base_url#id 
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
		html_engine_interface_stop_query(data);
		GtkHTMLStream *stream = gtk_html_begin_content(html, "");
		gtk_html_set_base (html, url);
		getdata(html, "GET", url, "", stream, data, TRUE);
    }
}

//послали с формы
static void
on_submit(GtkHTML * html, const gchar * method, const gchar * action,
	  const gchar * encoding, gpointer data)
{
	html_engine_interface_stop_query(data);
    GtkHTMLStream *stream = gtk_html_begin_content(html, "");
    getdata(html, method, action, encoding, stream, data, TRUE);
}

//перенаправление
static void
on_redirect(GtkHTML * html, const gchar * url, int delay, gpointer data)
{
    if (delay == 0) {
		g_print("Redirecting to '%s'\n", url);
		on_link_clicked(html, url, data);
    } else {
		g_print("Redirecting to '%s' in %d seconds. Not realized yet.\n",
			url,
			delay);
    }
}
