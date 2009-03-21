/*
 *      loaders.h
 *      
 *      Copyright 2009 Denis Pauk <pauk.denis@gmail.com>
 *      
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation; either version 2 of the License, or
 *      (at your option) any later version.
 *      
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *      
 *      You should have received a copy of the GNU General Public License
 *      along with this program; if not, write to the Free Software
 *      Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 *      MA 02110-1301, USA.
 */
#ifndef __LOADERS_H__
#define __LOADERS_H__

#include <glib.h>
#include <glib-object.h>
#include <stdlib.h>
#include <string.h>
#include <libsoup/soup.h>
#include <libsoup/soup-cookie-jar.h>
#include <gtkhtml/gtkhtml.h>

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

/*init render*/
void loaders_init_internal (	loaders* self, const SoupSession* session,
								GtkHTML * html, GtkHTMLStream *stream,
								gboolean redirect_save);

/*get content and Render*/
void loaders_render(	loaders *self, const gchar *action,
						const gchar * method, const gchar * encoding);
						
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
