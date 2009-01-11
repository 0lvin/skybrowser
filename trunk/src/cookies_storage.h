
#ifndef __COOKIES_STORAGE_H__
#define __COOKIES_STORAGE_H__

#include <glib.h>
#include <glib-object.h>
#include <stdlib.h>
#include <string.h>

G_BEGIN_DECLS


#define TYPE_COOKIES_STORAGE (cookies_storage_get_type ())
#define COOKIES_STORAGE(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), TYPE_COOKIES_STORAGE, cookies_storage))
#define COOKIES_STORAGE_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), TYPE_COOKIES_STORAGE, cookies_storageClass))
#define IS_COOKIES_STORAGE(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TYPE_COOKIES_STORAGE))
#define IS_COOKIES_STORAGE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), TYPE_COOKIES_STORAGE))
#define COOKIES_STORAGE_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), TYPE_COOKIES_STORAGE, cookies_storageClass))

typedef struct _cookies_storage cookies_storage;
typedef struct _cookies_storageClass cookies_storageClass;
typedef struct _cookies_storagePrivate cookies_storagePrivate;
typedef struct _ParamSpeccookies_storage ParamSpeccookies_storage;

struct _cookies_storage {
	GTypeInstance parent_instance;
	volatile int ref_count;
	cookies_storagePrivate * priv;
};

struct _cookies_storageClass {
	GTypeClass parent_class;
	void (*finalize) (cookies_storage *self);
};

struct _ParamSpeccookies_storage {
	GParamSpec parent_instance;
};


void cookies_storage_cookies_storage (cookies_storage* self);
void cookies_storage_add (cookies_storage* self, const gchar* cookie, const gchar* uri);
gchar* cookies_storage_get (cookies_storage* self, const gchar* uri);
cookies_storage* cookies_storage_construct (GType object_type);
cookies_storage* cookies_storage_new (void);
GParamSpec* param_spec_cookies_storage (const gchar* name, const gchar* nick, const gchar* blurb, GType object_type, GParamFlags flags);
gpointer value_get_cookies_storage (const GValue* value);
void value_set_cookies_storage (GValue* value, gpointer v_object);
GType cookies_storage_get_type (void);
gpointer cookies_storage_ref (gpointer instance);
void cookies_storage_unref (gpointer instance);


G_END_DECLS

#endif
