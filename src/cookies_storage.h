#ifndef COOKIES_STORAGE_H
#define COOKIES_STORAGE_H
#include <glib.h>
#include <string.h>
#include <stdlib.h>

typedef gchar** cookies_storage;

void cookies_storage_add(cookies_storage storage, gchar* cookie, gchar * uri);

gchar* cookies_storage_get(cookies_storage storage, gchar * uri);

#endif
