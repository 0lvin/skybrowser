
#include "cookies_storage.h"
#include <gobject/gvaluecollector.h>




struct _cookies_storagePrivate {
	GList* cookies_list;
};

#define COOKIES_STORAGE_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), TYPE_COOKIES_STORAGE, cookies_storagePrivate))
enum  {
	COOKIES_STORAGE_DUMMY_PROPERTY
};
static void _g_list_free_g_free (GList* self);
static gpointer cookies_storage_parent_class = NULL;
static void cookies_storage_finalize (cookies_storage* obj);



static void _g_list_free_g_free (GList* self) {
	g_list_foreach (self, (GFunc) g_free, NULL);
	g_list_free (self);
}


void cookies_storage_cookies_storage (cookies_storage* self) {
	GList* _tmp0;
	g_return_if_fail (self != NULL);
	_tmp0 = NULL;
	self->priv->cookies_list = (_tmp0 = NULL, (self->priv->cookies_list == NULL) ? NULL : (self->priv->cookies_list = (_g_list_free_g_free (self->priv->cookies_list), NULL)), _tmp0);
}


void cookies_storage_add (cookies_storage* self, const gchar* cookie, const gchar* uri) {
	const gchar *posend;
	g_return_if_fail (self != NULL);
	g_return_if_fail (cookie != NULL);
	g_return_if_fail (uri != NULL);
	g_print("\n---------------------------\nCookies=%s\n---------------\n", cookie);
	posend = strchr(cookie, ';');
	if (posend != NULL)
			self->priv->cookies_list = g_list_append (self->priv->cookies_list, g_strdup (posend));
}


gchar* cookies_storage_get (cookies_storage* self, const gchar* uri) {
	char* result;
	g_return_val_if_fail (self != NULL, NULL);
	result = g_strdup ("");
	{
		GList* cookies_collection;
		GList* cookies_it;
		cookies_collection = self->priv->cookies_list;
		for (cookies_it = cookies_collection; cookies_it != NULL; cookies_it = cookies_it->next) {
			const char* _tmp2;
			char* cookies;
			_tmp2 = NULL;
			cookies = (_tmp2 = (const char*) cookies_it->data, (_tmp2 == NULL) ? NULL : g_strdup (_tmp2));
			{
				char* _tmp1;
				char* _tmp0;
				_tmp1 = NULL;
				_tmp0 = NULL;
				result = (_tmp1 = g_strconcat (result, _tmp0 = ((g_strconcat (cookies, ";", NULL))), NULL), result = (g_free (result), NULL), _tmp1);
				_tmp0 = (g_free (_tmp0), NULL);
				cookies = (g_free (cookies), NULL);
			}
		}
	}
	return result;
}


cookies_storage* cookies_storage_construct (GType object_type) {
	cookies_storage* self;
	self = (cookies_storage*) g_type_create_instance (object_type);
	return self;
}


cookies_storage* cookies_storage_new (void) {
	return cookies_storage_construct (TYPE_COOKIES_STORAGE);
}


static void value_cookies_storage_init (GValue* value) {
	value->data[0].v_pointer = NULL;
}


static void value_cookies_storage_free_value (GValue* value) {
	if (value->data[0].v_pointer) {
		cookies_storage_unref (value->data[0].v_pointer);
	}
}


static void value_cookies_storage_copy_value (const GValue* src_value, GValue* dest_value) {
	if (src_value->data[0].v_pointer) {
		dest_value->data[0].v_pointer = cookies_storage_ref (src_value->data[0].v_pointer);
	} else {
		dest_value->data[0].v_pointer = NULL;
	}
}


static gpointer value_cookies_storage_peek_pointer (const GValue* value) {
	return value->data[0].v_pointer;
}


static gchar* value_cookies_storage_collect_value (GValue* value, guint n_collect_values, GTypeCValue* collect_values, guint collect_flags) {
	if (collect_values[0].v_pointer) {
		cookies_storage* object;
		object = collect_values[0].v_pointer;
		if (object->parent_instance.g_class == NULL) {
			return g_strconcat ("invalid unclassed object pointer for value type `", G_VALUE_TYPE_NAME (value), "'", NULL);
		} else if (!g_value_type_compatible (G_TYPE_FROM_INSTANCE (object), G_VALUE_TYPE (value))) {
			return g_strconcat ("invalid object type `", g_type_name (G_TYPE_FROM_INSTANCE (object)), "' for value type `", G_VALUE_TYPE_NAME (value), "'", NULL);
		}
		value->data[0].v_pointer = cookies_storage_ref (object);
	} else {
		value->data[0].v_pointer = NULL;
	}
	return NULL;
}


static gchar* value_cookies_storage_lcopy_value (const GValue* value, guint n_collect_values, GTypeCValue* collect_values, guint collect_flags) {
	cookies_storage** object_p;
	object_p = collect_values[0].v_pointer;
	if (!object_p) {
		return g_strdup_printf ("value location for `%s' passed as NULL", G_VALUE_TYPE_NAME (value));
	}
	if (!value->data[0].v_pointer) {
		*object_p = NULL;
	} else if (collect_flags && G_VALUE_NOCOPY_CONTENTS) {
		*object_p = value->data[0].v_pointer;
	} else {
		*object_p = cookies_storage_ref (value->data[0].v_pointer);
	}
	return NULL;
}


GParamSpec* param_spec_cookies_storage (const gchar* name, const gchar* nick, const gchar* blurb, GType object_type, GParamFlags flags) {
	ParamSpeccookies_storage* spec;
	g_return_val_if_fail (g_type_is_a (object_type, TYPE_COOKIES_STORAGE), NULL);
	spec = g_param_spec_internal (G_TYPE_PARAM_OBJECT, name, nick, blurb, flags);
	G_PARAM_SPEC (spec)->value_type = object_type;
	return G_PARAM_SPEC (spec);
}


gpointer value_get_cookies_storage (const GValue* value) {
	g_return_val_if_fail (G_TYPE_CHECK_VALUE_TYPE (value, TYPE_COOKIES_STORAGE), NULL);
	return value->data[0].v_pointer;
}


void value_set_cookies_storage (GValue* value, gpointer v_object) {
	cookies_storage* old;
	g_return_if_fail (G_TYPE_CHECK_VALUE_TYPE (value, TYPE_COOKIES_STORAGE));
	old = value->data[0].v_pointer;
	if (v_object) {
		g_return_if_fail (G_TYPE_CHECK_INSTANCE_TYPE (v_object, TYPE_COOKIES_STORAGE));
		g_return_if_fail (g_value_type_compatible (G_TYPE_FROM_INSTANCE (v_object), G_VALUE_TYPE (value)));
		value->data[0].v_pointer = v_object;
		cookies_storage_ref (value->data[0].v_pointer);
	} else {
		value->data[0].v_pointer = NULL;
	}
	if (old) {
		cookies_storage_unref (old);
	}
}


static void cookies_storage_class_init (cookies_storageClass * klass) {
	cookies_storage_parent_class = g_type_class_peek_parent (klass);
	COOKIES_STORAGE_CLASS (klass)->finalize = cookies_storage_finalize;
	g_type_class_add_private (klass, sizeof (cookies_storagePrivate));
}


static void cookies_storage_instance_init (cookies_storage * self) {
	self->priv = COOKIES_STORAGE_GET_PRIVATE (self);
	self->ref_count = 1;
}


static void cookies_storage_finalize (cookies_storage* obj) {
	cookies_storage * self;
	self = COOKIES_STORAGE (obj);
	(self->priv->cookies_list == NULL) ? NULL : (self->priv->cookies_list = (_g_list_free_g_free (self->priv->cookies_list), NULL));
}


GType cookies_storage_get_type (void) {
	static GType cookies_storage_type_id = 0;
	if (cookies_storage_type_id == 0) {
		static const GTypeValueTable g_define_type_value_table = { value_cookies_storage_init, value_cookies_storage_free_value, value_cookies_storage_copy_value, value_cookies_storage_peek_pointer, "p", value_cookies_storage_collect_value, "p", value_cookies_storage_lcopy_value };
		static const GTypeInfo g_define_type_info = { sizeof (cookies_storageClass), (GBaseInitFunc) NULL, (GBaseFinalizeFunc) NULL, (GClassInitFunc) cookies_storage_class_init, (GClassFinalizeFunc) NULL, NULL, sizeof (cookies_storage), 0, (GInstanceInitFunc) cookies_storage_instance_init, &g_define_type_value_table };
		static const GTypeFundamentalInfo g_define_type_fundamental_info = { (G_TYPE_FLAG_CLASSED | G_TYPE_FLAG_INSTANTIATABLE | G_TYPE_FLAG_DERIVABLE | G_TYPE_FLAG_DEEP_DERIVABLE) };
		cookies_storage_type_id = g_type_register_fundamental (g_type_fundamental_next (), "cookies_storage", &g_define_type_info, &g_define_type_fundamental_info, 0);
	}
	return cookies_storage_type_id;
}


gpointer cookies_storage_ref (gpointer instance) {
	cookies_storage* self;
	self = instance;
	g_atomic_int_inc (&self->ref_count);
	return instance;
}


void cookies_storage_unref (gpointer instance) {
	cookies_storage* self;
	self = instance;
	if (g_atomic_int_dec_and_test (&self->ref_count)) {
		COOKIES_STORAGE_GET_CLASS (self)->finalize (self);
		g_type_free_instance ((GTypeInstance *) self);
	}
}
