#include <loaders.h>
#include <stdlib.h>
#include <gtkhtml/gtkhtml.h>
//#define DebugVariable
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


/*Разобрать и установить путь по умолчанию*/
/*static char *
parsecurrent(const gchar * url, gchar ** current)
{
    char *path;

    int len;

    char *tmpstr;

    g_print("current='%s' url='%s'->\n", *current, url);
    path = calloc(strlen(*current) + strlen(url) + 2, sizeof(char));
    if (
		!strncmp(url, "http:", strlen("http:")) ||
		!strncmp(url, "https:", strlen("https:")) ||
		!strncmp(url, "ftp:", strlen("ftp:")) ||
		!strncmp(url, "file:", strlen("file:"))
		) {
			strncpy(path, url, strlen(url));
		} else {
			strcpy(path, *current);
	if (*url == '/' && strlen(path) > 1)
	    if (*(path + strlen(path) - 1) == '/') {
		char *search = strchr(path + strlen("http://"), '/');

		if (search != NULL) {
		    g_print("searc=%s\n", search);
		    *search = 0;
		}
	    }
		strcat(path, url);
    }

    if (strchr(path + strlen("http://"), '/') == NULL)
		strcat(path, "/");
		g_print("full url=%s\n", path);
		len = strcspn(path, "?#");
		while (*(path + len) != '/') {
			if (len == 0)
				break;
			len--;
		}
		free(*current);

		*current = calloc(len + 1 + 1, sizeof(char));
		strncpy(*current, path, len + 1);
		(*current)[len + 1] = 0;
		tmpstr = calloc(strlen(path + len + 1) + 1, sizeof(char));
		strcpy(tmpstr, path + len + 1);
		free(path);
		g_print("->current='%s' url='%s'\n", *current, tmpstr);
		return tmpstr;
}
*/
static void
loadData(GtkHTML * html, const gchar *action, const gchar * method, 
		const gchar * encoding, GtkHTMLStream * stream,
		gpointer data, gboolean redirect_save)
{
    struct All_variable *variable = (struct All_variable *) data;

    gchar *ContentType = NULL;

    gchar *buf = NULL;

    size_t length = 0;
	gchar* curr_base = g_strdup(gtk_html_get_base(html));
	
	if (!strncmp(action, "data:", strlen("data:")))
		buf = get_data_content(action, &length, &ContentType);
		
	if (!strncmp(action, "http:", strlen("http:")))
		if(buf == NULL)
		{
			gchar* curr_base_save = curr_base;
			buf = get_http_content(action, &length, &ContentType, method,
				 encoding, &curr_base, variable->saved_cookies, variable->session);
			if( buf != NULL)
			{
				if (redirect_save == TRUE)
					gtk_html_set_base (html, curr_base);
				if(curr_base_save != curr_base)
					free(curr_base_save);
			}
		}
		
    if (buf == NULL)
		buf = get_default_content(action, &length, &ContentType);
		
    /* Enable change content type in engine */
    gtk_html_set_default_engine(html, TRUE);

    if (ContentType != NULL)
		gtk_html_set_default_content_type(html, ContentType);

    if (buf != NULL) {
		gtk_html_stream_write(stream, buf, length);
		gtk_html_stream_close(stream, GTK_HTML_STREAM_OK);
    }

    if (buf != NULL)
		g_free(buf);
}


/*получить даннык по ссылке*/
static void
getdata(GtkHTML * html, const gchar * method, const gchar * action,
	const gchar * encoding, GtkHTMLStream * stream, gpointer data,
	gboolean redirect_save)
{
    char *realurl;

    char *gotocharp = NULL;

    struct All_variable *variable = (struct All_variable *) data;
#ifdef DebugVariable
    g_print("variable=%x \n", variable);
	
    if (data == NULL)
		g_print("Erorr in file (%s) line (%d)", __FILE__, __LINE__);
#endif
    if (!strcmp(method, "GET") || !strcmp(method, "POST")) {
		char *currpos;
		/*!strncmp(url, "http:", strlen("http:")) ||
		!strncmp(url, "https:", strlen("https:")) ||
		!strncmp(url, "ftp:", strlen("ftp:")) ||
		!strncmp(url, "file:", strlen("file:"))*/
		if (!strncmp(action, "file:", strlen("file:"))
			|| !strncmp(action, "http:", strlen("http:"))) {
			realurl = calloc(strlen(action) + 1, sizeof(char));
			strcpy(realurl, action);
		} else {
			realurl =
			(char *) calloc(strlen(action) +
					strlen(gtk_html_get_base(html)) + 2,
					sizeof(char));
			strcpy(realurl, gtk_html_get_base(html));
			if (*action == '/' && strlen(realurl) > 1)
				if (*(realurl + strlen(realurl) - 1) == '/') {
					char *search = strchr(realurl + strlen("http://"), '/');

					g_print("search=%s\n", search);
					*search = 0;
					/*realurl[strlen(realurl)-1]=0; */
				}
				strcat(realurl, action);
		}
		g_print("submitting '%s' to '%s' using method '%s' by '%s' \n",
			encoding, action, method, realurl);
		currpos = realurl;
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
		loadData(html, realurl, method, encoding, stream, data,
			redirect_save);
	
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
	if( variable->session != NULL)
		soup_session_abort(variable->session);
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
		       argc > 1 ? argv[1] : "http://google.com");
    on_entry_changed(variable->entry, variable);
    /* run the main loop */
    gtk_main();
}
