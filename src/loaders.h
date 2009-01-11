#ifndef LOADERS_H
#define LOADERS_H
#include "cookies_storage.h"
#include <libsoup/soup.h>
/*#define DebugLoaders*/
gchar * get_http_content(const gchar * action, gsize * length, gchar ** contentType,
				gchar* method, gchar* encoding, gchar** curr_base,
				cookies_storage* cookies_save, SoupSession* session);
gchar * get_data_content	(const gchar * action, gsize * length, gchar ** contentType);
gchar * get_default_content	(const gchar * action, gsize * length, gchar ** contentType);
#endif
