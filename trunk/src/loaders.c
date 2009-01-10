#include "loaders.h"
#include <gio/gio.h>
#include "cookies_storage.h"
#include <libsoup/soup.h>
/*unescape url*/
gchar *
decode(const gchar * token)
{
    const gchar *full_pos;
    gchar *resulted;
    gchar *write_pos;
    const gchar *read_pos;

    if (token == NULL)
		return NULL;

    /*stop pointer */
    full_pos = token + strlen(token);
    resulted = g_new(gchar, strlen(token) + 1);
    write_pos = resulted;
    read_pos = token;
    while (read_pos < full_pos) {
	size_t count_chars = strcspn(read_pos, "%");

	memcpy(write_pos, read_pos, count_chars);
	write_pos += count_chars;
	read_pos += count_chars;
	/*may be end string? */
	if (read_pos < full_pos)
	    if (*read_pos == '%') {
		/*skip not needed % */
		read_pos++;
		if (*(read_pos) != 0)
		    if (*(read_pos + 1) != 0) {
				gchar save[3];
				save[0]=*read_pos;
				save[1]=*(read_pos + 1);
				save[2]=0;
				(*write_pos) = strtol(save, NULL, 16);
				write_pos += 1;
				read_pos += 2;
		    }
	    }
    }
    *write_pos = 0;
    return resulted;
}

/*receive content by default loaders*/
gchar *
get_default_content(const gchar * action, gsize * length, gchar * contentType)
{
    gchar *buf = NULL;
    GFile *fd = NULL;
    GError *result = NULL;

    fd = g_file_new_for_uri(action);

    g_file_load_contents(fd, NULL, &buf, length, NULL, &result);

    if (buf == NULL) {
		static gchar html_source[] = "<html><body>Error while read file</body><html>";
		buf = g_strdup(html_source);
		*length = strlen(html_source);
    }

    g_object_unref(fd);
    return buf;
}

///my crazy world!!!!
/*
 * encodind -- params for Get Or Post
 */
gchar *
get_http_content(const gchar * action, gsize * length, gchar * contentType, gchar* method, gchar* encoding,
				 gchar** curr_base, cookies_storage cookies_save, SoupSession* session)
{
	gchar * buf = NULL;	
	SoupMessage *msg;
    if (!strncmp(action, "http:", strlen("http:"))) {
		/*Know methods!!*/
		if (!strcmp(method, "POST") || !strcmp(method, "GET")) {
	    	if (!strcmp(method, "POST")) {
				msg = soup_message_new("POST", action);
					soup_message_set_request(msg,
						 "application/x-www-form-urlencoded",
						SOUP_MEMORY_TAKE, (char *) action,
					strlen(encoding));
	    	} else if (!strcmp(method, "GET")) {
					gchar *tmpstr = g_new(gchar,strlen(action) + strlen(encoding) + 2);				      
					strcpy(tmpstr, action);
					if (*encoding != 0) {
		    			strcat(tmpstr, "?");
		    			strcat(tmpstr, encoding);
					}
					msg = soup_message_new("GET", tmpstr);
					g_free(tmpstr);
	    	}			
	    	{			
				soup_message_headers_append(msg->request_headers, "Cookie",
						cookies_storage_get(cookies_save, action));
				soup_message_headers_append(msg->request_headers,
					    "Accept-Charset",
					    "UTF-8, unicode-1-1;q=0.8");
		//may be error but current??
				soup_message_headers_append(msg->request_headers, "Referer",
							*curr_base);
			}
			{
				guint status = soup_session_send_message(session, msg);
				*curr_base = soup_uri_to_string(soup_message_get_uri(msg), FALSE);
				if (status >= 200 && status < 300) {
		    		*contentType =
						(gchar*)soup_message_headers_get(msg->response_headers,
							"Content-type");
		    		{
						const gchar *cookies =
			    			soup_message_headers_get(msg->response_headers,
								"Set-Cookie");

						if (cookies)
			    			cookies_storage_add(cookies_save, cookies, *curr_base);
		    		}
		    		buf = (gchar*)msg->response_body->data;
		    		*length = msg->response_body->length;
					return buf;
				} else {
		    		g_print("Status=%d\n", status);
					return NULL;
				}
	    	}
		}
	}
}

gchar * convert_to_correct_name(gchar* base, gchar * url)
{
	return url;
}

/*receive content by static data from url*/
gchar *
get_data_content(const gchar * action, gsize * length, gchar ** contentType)
{
    guchar *buf = NULL;
    if (!strncmp(action, "data:", strlen("data:"))) {
		const gchar *real_action = action + strlen("data:");
		const gchar *start_data = strchr(real_action, ';');
		if (start_data != NULL) {
			gsize ContentType_length = start_data - real_action;
			*contentType = g_new(gchar, ContentType_length + 1);
			memcpy(*contentType, real_action, ContentType_length);
			*(*contentType + ContentType_length) = 0;
#ifdef DebugLoaders
			g_print("internal data query used: Content_type %s\n", *contentType);
#endif
			if (!strncmp(start_data, ";base64,", strlen(";base64,"))) {
				gint state = 0;
				guint save = 0;
				const gchar * toDecode = start_data + strlen(";base64,");
				gchar *result_decode = decode(toDecode);
				buf = g_new(guchar,(strlen(result_decode) * 3) / 4 + 1);
				*length = g_base64_decode_step(result_decode,
					       strlen(result_decode),
					       buf, &state, &save);
#ifdef DebugLoaders						   
				g_print("using base64!!%s", result_decode);
#endif
				g_free(result_decode);
			}
		}
    }
    return (gchar *)buf;
}
