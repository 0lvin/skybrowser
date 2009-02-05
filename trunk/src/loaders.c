
#include "loaders.h"
#include <gobject/gvaluecollector.h>
#include <gio/gio.h>
#include <libsoup/soup.h>
#include <string.h>
#include <config.h>

struct _loadersPrivate {
	cookies_storage* cookies_save;
	SoupSession* session;
};

#define LOADERS_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), TYPE_LOADERS, loadersPrivate))
enum  {
	LOADERS_DUMMY_PROPERTY
};
static gpointer loaders_parent_class = NULL;
static void loaders_finalize (loaders* obj);



void loaders_init_soap (loaders* self, const cookies_storage* cookies_save,const SoupSession* session) {
	g_return_if_fail (self != NULL);
	g_return_if_fail (session != NULL);
	g_return_if_fail (cookies_save != NULL);	
	if(self->priv->cookies_save != NULL)
		cookies_storage_unref(self->priv->cookies_save);
	self->priv->cookies_save = cookies_storage_ref(cookies_save);
	self->priv->session  = session;
}

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

/*
 * encodind -- params for Get Or Post
 */
gchar* loaders_http_content (loaders* self, 	const gchar * action, gsize * length, gchar ** contentType,
				gchar* method, gchar* encoding, gchar** curr_base) {
	g_return_val_if_fail (self != NULL, NULL);
	g_return_val_if_fail (action != NULL, NULL);
	g_return_val_if_fail (contentType != NULL, NULL);
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
				/*soup_message_headers_append(msg->request_headers, "Cookie",
						cookies_storage_get(self->priv->cookies_save, action));*/
				soup_message_headers_append(msg->request_headers,
					    "Accept-Charset",
					    "UTF-8, unicode-1-1;q=0.8");
				/*may be error but current?*/
				soup_message_headers_append(msg->request_headers, "Referer",
							*curr_base);
			}
			{
				guint status = soup_session_send_message(self->priv->session, msg);
				*curr_base = soup_uri_to_string(soup_message_get_uri(msg), FALSE);
				
				if (status >= 200 && status < 300) {					
		    		*contentType =
						(gchar*)soup_message_headers_get(msg->response_headers,
							"Content-type");
		    		{
						const gchar *cookies =
			    			soup_message_headers_get(msg->response_headers,
								"Set-Cookie");

						/*if (cookies)
			    			cookies_storage_add(self->priv->cookies_save, cookies, *curr_base);*/
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
	return NULL;
}

/*receive content by static data from url*/
gchar* loaders_data_content (loaders* self, const gchar * action, gsize * length, gchar ** contentType) {
	g_return_val_if_fail (self != NULL, NULL);
	g_return_val_if_fail (action != NULL, NULL);
	g_return_val_if_fail (length != NULL, NULL);
	g_return_val_if_fail (contentType != NULL, NULL);
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

/*receive content by default loaders*/
gchar* loaders_default_content (loaders* self, const gchar * action, gsize * length, gchar ** contentType) {
	g_return_val_if_fail (self != NULL, NULL);
	g_return_val_if_fail (action != NULL, NULL);
	g_return_val_if_fail (length != NULL, NULL);
	g_return_val_if_fail (contentType != NULL, NULL);
	
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


loaders* loaders_construct (GType object_type) {
	loaders* self;
	self = (loaders*) g_type_create_instance (object_type);
	return self;
}


loaders* loaders_new (void) {
	return loaders_construct (TYPE_LOADERS);
}


static void value_loaders_init (GValue* value) {
	value->data[0].v_pointer = NULL;
}


static void value_loaders_free_value (GValue* value) {
	if (value->data[0].v_pointer) {
		loaders_unref (value->data[0].v_pointer);
	}
}


static void value_loaders_copy_value (const GValue* src_value, GValue* dest_value) {
	if (src_value->data[0].v_pointer) {
		dest_value->data[0].v_pointer = loaders_ref (src_value->data[0].v_pointer);
	} else {
		dest_value->data[0].v_pointer = NULL;
	}
}


static gpointer value_loaders_peek_pointer (const GValue* value) {
	return value->data[0].v_pointer;
}


static gchar* value_loaders_collect_value (GValue* value, guint n_collect_values, GTypeCValue* collect_values, guint collect_flags) {
	if (collect_values[0].v_pointer) {
		loaders* object;
		object = collect_values[0].v_pointer;
		if (object->parent_instance.g_class == NULL) {
			return g_strconcat ("invalid unclassed object pointer for value type `", G_VALUE_TYPE_NAME (value), "'", NULL);
		} else if (!g_value_type_compatible (G_TYPE_FROM_INSTANCE (object), G_VALUE_TYPE (value))) {
			return g_strconcat ("invalid object type `", g_type_name (G_TYPE_FROM_INSTANCE (object)), "' for value type `", G_VALUE_TYPE_NAME (value), "'", NULL);
		}
		value->data[0].v_pointer = loaders_ref (object);
	} else {
		value->data[0].v_pointer = NULL;
	}
	return NULL;
}


static gchar* value_loaders_lcopy_value (const GValue* value, guint n_collect_values, GTypeCValue* collect_values, guint collect_flags) {
	loaders** object_p;
	object_p = collect_values[0].v_pointer;
	if (!object_p) {
		return g_strdup_printf ("value location for `%s' passed as NULL", G_VALUE_TYPE_NAME (value));
	}
	if (!value->data[0].v_pointer) {
		*object_p = NULL;
	} else if (collect_flags && G_VALUE_NOCOPY_CONTENTS) {
		*object_p = value->data[0].v_pointer;
	} else {
		*object_p = loaders_ref (value->data[0].v_pointer);
	}
	return NULL;
}


GParamSpec* param_spec_loaders (const gchar* name, const gchar* nick, const gchar* blurb, GType object_type, GParamFlags flags) {
	ParamSpecloaders* spec;
	g_return_val_if_fail (g_type_is_a (object_type, TYPE_LOADERS), NULL);
	spec = g_param_spec_internal (G_TYPE_PARAM_OBJECT, name, nick, blurb, flags);
	G_PARAM_SPEC (spec)->value_type = object_type;
	return G_PARAM_SPEC (spec);
}


gpointer value_get_loaders (const GValue* value) {
	g_return_val_if_fail (G_TYPE_CHECK_VALUE_TYPE (value, TYPE_LOADERS), NULL);
	return value->data[0].v_pointer;
}


void value_set_loaders (GValue* value, gpointer v_object) {
	loaders* old;
	g_return_if_fail (G_TYPE_CHECK_VALUE_TYPE (value, TYPE_LOADERS));
	old = value->data[0].v_pointer;
	if (v_object) {
		g_return_if_fail (G_TYPE_CHECK_INSTANCE_TYPE (v_object, TYPE_LOADERS));
		g_return_if_fail (g_value_type_compatible (G_TYPE_FROM_INSTANCE (v_object), G_VALUE_TYPE (value)));
		value->data[0].v_pointer = v_object;
		loaders_ref (value->data[0].v_pointer);
	} else {
		value->data[0].v_pointer = NULL;
	}
	if (old) {
		loaders_unref (old);
	}
}


static void loaders_class_init (loadersClass * klass) {
	loaders_parent_class = g_type_class_peek_parent (klass);
	LOADERS_CLASS (klass)->finalize = loaders_finalize;
	g_type_class_add_private (klass, sizeof (loadersPrivate));
}


static void loaders_instance_init (loaders * self) {
	self->priv = LOADERS_GET_PRIVATE (self);
	self->ref_count = 1;
}


static void loaders_finalize (loaders* obj) {
	loaders * self;
	self = LOADERS (obj);
	if(self->priv->cookies_save != NULL)
		cookies_storage_unref(self->priv->cookies_save);
	self->priv->cookies_save = NULL;
}


GType loaders_get_type (void) {
	static GType loaders_type_id = 0;
	if (loaders_type_id == 0) {
		static const GTypeValueTable g_define_type_value_table = { value_loaders_init, value_loaders_free_value, value_loaders_copy_value, value_loaders_peek_pointer, "p", value_loaders_collect_value, "p", value_loaders_lcopy_value };
		static const GTypeInfo g_define_type_info = { sizeof (loadersClass), (GBaseInitFunc) NULL, (GBaseFinalizeFunc) NULL, (GClassInitFunc) loaders_class_init, (GClassFinalizeFunc) NULL, NULL, sizeof (loaders), 0, (GInstanceInitFunc) loaders_instance_init, &g_define_type_value_table };
		static const GTypeFundamentalInfo g_define_type_fundamental_info = { (G_TYPE_FLAG_CLASSED | G_TYPE_FLAG_INSTANTIATABLE | G_TYPE_FLAG_DERIVABLE | G_TYPE_FLAG_DEEP_DERIVABLE) };
		loaders_type_id = g_type_register_fundamental (g_type_fundamental_next (), "loaders", &g_define_type_info, &g_define_type_fundamental_info, 0);
	}
	return loaders_type_id;
}


gpointer loaders_ref (gpointer instance) {
	loaders* self;
	self = instance;
	g_atomic_int_inc (&self->ref_count);
	return instance;
}


void loaders_unref (gpointer instance) {
	loaders* self;
	self = instance;
	if (g_atomic_int_dec_and_test (&self->ref_count)) {
		LOADERS_GET_CLASS (self)->finalize (self);
		g_type_free_instance ((GTypeInstance *) self);
	}
}
