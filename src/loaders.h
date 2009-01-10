#ifndef LOADERS_H
#define LOADERS_H
#include <config.h>
#include <string.h>
#include <gtkhtml/gtkhtml.h>
/*#define DebugLoaders*/
gchar * get_data_content(const gchar * action, gsize * length, gchar ** contentType);
gchar * get_default_content(const gchar * action, gsize * length, gchar * contentType);
#endif
