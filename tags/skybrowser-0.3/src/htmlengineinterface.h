/*
 *      htmlengineinterface.h
 *      
 *      Copyright 2009 Denis Pauk <denis@lunar>
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
#ifndef HTML_ENGINE_INTERFACE
#define HTML_ENGINE_INTERFACE

#include <glib.h>
#include <glib-object.h>
#include <libsoup/soup.h>

/*interface*/
/*Структур для хранения всех переменных*/
struct All_variable
{
    GtkWidget *app;
    GtkWidget *html;
	GtkWidget *textentry;
    /*текущая сесия, может нужно перенести в параметр обратного вызова */
    SoupSession *session;
	GList *list;
};

struct All_variable* html_engine_intreface_construct();

void html_engine_interface_print(struct All_variable * variable);

void html_engine_interface_go(struct All_variable * variable, gchar *go);

void html_engine_interface_back(struct All_variable * variable);

GtkWidget* html_engine_intreface_init(struct All_variable * variable, GtkWidget* app, GtkWidget* textentry);

#endif
