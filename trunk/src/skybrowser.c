#include <loaders.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <libsoup/soup.h>
#include <sys/mman.h>
#include <iconv.h>

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
    gchar *saved_cookies;
    /*текущая сесия, может нужно перенести в параметр обратного вызова */
    SoupSession *session;
};

/*смена заголовка*/
static void
title_changed_cb(GtkHTML * html, const gchar * title, gpointer data)
{
    g_print("title_changed_cb=%s\n", title);
	struct All_variable *variable = (struct All_variable *) data;
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

//Разобрать и установить путь по умолчанию
static char *
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

//установка текущей html base
static char *
change_html_base(GtkHTML * html, const gchar * url)
{
    char *tmpstr;

    char *current;

    current = calloc(strlen(gtk_html_get_base(html)) + 1, sizeof(char));
    strcpy(current, gtk_html_get_base(html));
    tmpstr = parsecurrent(url, &current);
	g_print("change htmlbase(change_html_base) to %s\n",current);
    gtk_html_set_base(html, current);
    free(current);
    return tmpstr;
}

static void
loadData(GtkHTML * html, char *realurl, const gchar * method,
	 const gchar * action, const gchar * encoding, GtkHTMLStream * stream,
	 gpointer data, gboolean redirect_save, char *gotocharp)
{
    struct All_variable *variable = (struct All_variable *) data;

    gchar *ContentType = NULL;

    SoupMessage *msg;

    gchar *buf = NULL;

    size_t length = 0;

    buf = get_data_content(action, &length, &ContentType);

    if (!strncmp(realurl, "http:", strlen("http:"))) {
	if (!strcmp(method, "POST") || !strcmp(method, "GET")) {
	    if (!strcmp(method, "POST")) {
		msg = soup_message_new("POST", realurl);
		soup_message_set_request(msg,
					 "application/x-www-form-urlencoded",
					 SOUP_MEMORY_TAKE, (char *) encoding,
					 strlen(encoding));
	    }
	    else if (!strcmp(method, "GET")) {
		char *tmpstr = calloc(strlen(realurl) + strlen(encoding) + 2,
				      sizeof(char));

		strcpy(tmpstr, realurl);
		if (*encoding != 0) {
		    strcat(tmpstr, "?");
		    strcat(tmpstr, encoding);
		}
		msg = soup_message_new("GET", tmpstr);
		free(tmpstr);
	    }
	    {
		guint status;

		soup_message_headers_append(msg->request_headers, "Cookie",
					    variable->saved_cookies);
		soup_message_headers_append(msg->request_headers,
					    "Accept-Charset",
					    "UTF-8, unicode-1-1;q=0.8");
		//may be error but current??
		soup_message_headers_append(msg->request_headers, "Referer",
					    gtk_html_get_base(html));
		status = soup_session_send_message(variable->session, msg);
		if (redirect_save == TRUE) {
		    char *redirect_url =
			soup_uri_to_string(soup_message_get_uri(msg), FALSE);
		    if (strcmp(redirect_url, realurl)) {
			g_print("Redirect %s\n", redirect_url);
			free(change_html_base(html, redirect_url));	//url not need
		    }
		    free(redirect_url);
		}
		if (status >= 200 && status < 300) {
		    ContentType =
			(gchar*)soup_message_headers_get(msg->response_headers,
						"Content-type");
		    {
			const gchar *cookies =
			    soup_message_headers_get(msg->response_headers,
						"Set-Cookie");

			if (cookies)
			    cookies_storage_add(&(variable->saved_cookies), cookies, gtk_html_get_base(html));
		    }
		    buf = (gchar*)msg->response_body->data;
		    length = msg->response_body->length;
		}
		else {
		    g_print("Status=%d\n", status);
		}
	    }
	}
    }
    if (buf == NULL)
		buf = get_default_content(realurl, &length, &ContentType);
    if (ContentType != NULL)
	if (!strcmp(ContentType, "text/html"))
	    ContentType = NULL;	//not correct encoding
    if (ContentType == NULL)
	/*check html */
	if (*buf == '<' || *buf == '\r' || *buf == '\n') {
	    char *temp = strstr(buf, "text/html; ");

	    if (temp == NULL) {
		ContentType = "text/html; charset=utf8";
	    }
	    else {
		char *end = strchr(temp, '"');

		if (end == NULL) {
		    ContentType = "text/html; charset=utf8";
		}
		else {
		    ContentType = calloc(end - temp + 1, sizeof(char));
		    /*it not correct ContentType is const!!! */
		    strncpy((char *) ContentType, temp, end - temp);
		    g_print("new %s =%s\n", realurl, ContentType);
		}
	    }
	}
	
    /* Enable change content type in engine */
    gtk_html_set_default_engine(html, TRUE);

    if (ContentType != NULL)
	gtk_html_set_default_content_type(html, ContentType);

    if (buf != NULL) {
		gtk_html_stream_write(stream, buf, length);
		gtk_html_stream_close(stream, GTK_HTML_STREAM_OK);
    }

    if (gotocharp)
		change_position(html, gotocharp, data);

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

    g_print("variable=%x \n", variable);
    if (data == NULL)
	g_print("Eroor in file (%s) line (%d)", __FILE__, __LINE__);
    if (!strcmp(method, "GET") || !strcmp(method, "POST")) {
	char *currpos;

	if (!strncmp(action, "file:", strlen("file:"))
	    || !strncmp(action, "http:", strlen("http:"))) {
	    realurl = calloc(strlen(action) + 1, sizeof(char));
	    strcpy(realurl, action);
	}
	else {
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
	loadData(html, realurl, method, action, encoding, stream, data,
		 redirect_save, gotocharp);
    }
    else {
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

		char *tmpstr = change_html_base(html, url);

		getdata(html, "GET", tmpstr, "", stream, data, TRUE);
		free(tmpstr);
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
    g_print("variable=%x \n", variable);
    g_print("variable->session=%x \n", variable->session);
    if (data == NULL)
		g_print("Eroor in file (%s) line (%d)", __FILE__, __LINE__);
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

    g_print("variable=%x \n", variable);
    g_print("variable->session=%x \n", variable->session);
    if (data == NULL)
	g_print("Erorr in file (%s) line (%d)", __FILE__, __LINE__);
    g_print("%s\n", gtk_entry_get_text(GTK_ENTRY(widget)));
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

    g_print("variable=%x \n", variable);
    g_print("variable->session=%x \n", variable->session);
    variable->saved_cookies = calloc(1, sizeof(char));
    gtk_entry_set_text((GtkEntry *) (variable->entry),
		       argc > 1 ? argv[1] : "http://gnome.org");
    on_entry_changed(variable->entry, variable);
    /* run the main loop */
    gtk_main();
}
