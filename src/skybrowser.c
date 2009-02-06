#include <stdlib.h>
#include <gtkhtml/gtkhtml.h>
#include "loaders.h"
/*#define DebugVariable*/
/*Структур для хранения всех переменных*/
struct All_variable
{
    GtkWidget *app;
    GtkWidget *html;
    GtkWidget *scrolled_window;
#ifdef needAddWindow
    GtkWidget *window;
#endif
    GtkWidget *entry;
    cookies_storage* saved_cookies;
    /*текущая сесия, может нужно перенести в параметр обратного вызова */
    SoupSession *session;
};

/*смена заголовка*/
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
			baseurl == NULL/*unknow base url*/
		) {
			/*all right it's full url*/
			realurl = g_strdup(action);
		} else {			
			g_print("received %s %s \r\n",action,baseurl);
			realurl =
				(gchar *) g_new(gchar,
						strlen(action) +
						strlen(baseurl) + 4);
			strncpy(realurl, baseurl,strlen(baseurl));
			if(0)
			{
			/*contein slash - is path from domain*/
			if (*action == '/' && strlen(realurl) > 1){
				/*fing first '//' (as example file://) */
				gchar* startdomain = strstr(realurl,"//");
				if(startdomain != NULL){
					/* 2 -- length '//' */
					startdomain +=2 ;
					gchar* enddomain = strchr(startdomain,'/');
					if(enddomain == NULL)
						enddomain = startdomain;
					strcat(startdomain, action);
				}
			} else {
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
		loaders_init_internal (loaders_e , variable->saved_cookies, variable->session, html, stream, redirect_save);
		/* load data*/
		loaders_render(loaders_e, realurl, method, encoding);
	
		if (gotocharp)
			change_position(html, gotocharp, data);
    } else {
		g_print("Unknow Metod for url '%s'", realurl);
    }
    free(realurl);
}

/*stop all query*/
static void stop_query(gpointer data)
{
	    struct All_variable *variable = (struct All_variable *) data;
		if (variable->session != NULL)
		/*if( SOUP_IS_SOCKET (variable->session))*/
			soup_session_abort(variable->session);
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
		stop_query(data);
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
	stop_query(data);
    GtkHTMLStream *stream = gtk_html_begin_content(html, "");
    getdata(html, method, action, encoding, stream, data, TRUE);
}

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
	stop_query(data);
    gtk_main_quit();
    return FALSE;
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

static void
on_entry_changed(GtkWidget * widget, gpointer data)
{
    struct All_variable *variable = (struct All_variable *) data;
#ifdef DebugVariable
    g_print("variable=%x \n", variable);
    g_print("variable->session=%x \n", variable->session);
    if (data == NULL)
		g_print("Erorr in file (%s) line (%d)", __FILE__, __LINE__);
#endif
    g_print("on_entry_changed:%s\n", gtk_entry_get_text(GTK_ENTRY(widget)));
    on_link_clicked(GTK_HTML(variable->html),
		    gtk_entry_get_text(GTK_ENTRY(widget)), data);
}

//маин он и в Африке маин:-)
int
main(int argc, char **argv)
{
    struct All_variable *variable = g_new(struct All_variable, 1);

    /* Инициализируем поддержку i18n */
    gtk_set_locale();

    /* Инициализируем установки виджета */
    gtk_init(&argc, &argv);

    /* create GtkHTML widget */
    variable->html = gtk_html_new();

    variable->entry = gtk_entry_new();
#ifdef needAddWindow
    variable->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

    gtk_container_add(GTK_CONTAINER(variable->window), variable->entry);

    g_object_add_weak_pointer(G_OBJECT(variable->window),
			      (gpointer *) variable->window);

    gtk_window_set_default_size(GTK_WINDOW(variable->window), 300, 20);
#endif

    variable->app = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    variable->scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				   (variable->scrolled_window),
				   GTK_POLICY_AUTOMATIC,
				   GTK_POLICY_AUTOMATIC);
    gtk_container_add(GTK_CONTAINER(variable->scrolled_window),
		      variable->html);
    gtk_container_add(GTK_CONTAINER(variable->app),
		      variable->scrolled_window);
    gtk_window_set_default_size(GTK_WINDOW(variable->app), 800, 600);
    variable->session = soup_session_sync_new();
    gtk_widget_show_all(variable->app);
#ifdef needAddWindow
    gtk_widget_show_all(variable->window);
#endif
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
    g_signal_connect(variable->app, "delete-event",
		     G_CALLBACK(on_exit_window), variable);
    g_signal_connect(G_OBJECT(variable->entry), "button-press-event",
		     G_CALLBACK(on_entry_changed), variable);
    {
		gchar *tmpstr = g_new0 (gchar, 255);
		memcpy(tmpstr, "file:", sizeof("file:"));
		getcwd(tmpstr + strlen("file:"), 255 - strlen("file:") - 1);
		strcat(tmpstr, "/");
		gtk_html_set_base(GTK_HTML(variable->html), tmpstr);
		g_free(tmpstr);
    }
#ifdef DebugVariable
    g_print("variable=%x \n", variable);
    g_print("variable->session=%x \n", variable->session);
#endif
    variable->saved_cookies = cookies_storage_new ();
    gtk_entry_set_text((GtkEntry *) (variable->entry),
		       argc > 1 ? argv[1] : "http://mail.ru");
    on_entry_changed(variable->entry, variable);
    /* run the main loop */
    gtk_main();
}
