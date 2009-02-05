
#ifndef __LOADERS_H__
#define __LOADERS_H__

#include <glib.h>
#include <glib-object.h>
#include <stdlib.h>
#include <string.h>
#include "cookies_storage.h"
#include <libsoup/soup.h>

G_BEGIN_DECLS


#define TYPE_LOADERS (loaders_get_type ())
#define LOADERS(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), TYPE_LOADERS, loaders))
#define LOADERS_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), TYPE_LOADERS, loadersClass))
#define IS_LOADERS(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TYPE_LOADERS))
#define IS_LOADERS_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), TYPE_LOADERS))
#define LOADERS_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), TYPE_LOADERS, loadersClass))

typedef struct _loaders loaders;
typedef struct _loadersClass loadersClass;
typedef struct _loadersPrivate loadersPrivate;
typedef struct _ParamSpecloaders ParamSpecloaders;

struct _loaders {
	GTypeInstance parent_instance;
	volatile int ref_count;
	loadersPrivate * priv;
};

struct _loadersClass {
	GTypeClass parent_class;
	void (*finalize) (loaders *self);
};

struct _ParamSpecloaders {
	GParamSpec parent_instance;
};


void loaders_setSoap (loaders* self, const char* Soap);
char* loaders_http_content (loaders* self, 	const gchar * action, gsize * length, gchar ** contentType,
				gchar* method, gchar* encoding, gchar** curr_base,
				cookies_storage* cookies_save, SoupSession* session);
char* loaders_data_content (loaders* self, const gchar * action, gsize * length, gchar ** contentType);
char* loaders_default_content (loaders* self, const gchar * action, gsize * length, gchar ** contentType);
loaders* loaders_construct (GType object_type);
loaders* loaders_new (void);
GParamSpec* param_spec_loaders (const gchar* name, const gchar* nick, const gchar* blurb, GType object_type, GParamFlags flags);
gpointer value_get_loaders (const GValue* value);
void value_set_loaders (GValue* value, gpointer v_object);
GType loaders_get_type (void);
gpointer loaders_ref (gpointer instance);
void loaders_unref (gpointer instance);


G_END_DECLS

#endif
