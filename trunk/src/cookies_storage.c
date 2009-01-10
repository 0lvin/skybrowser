#include "cookies_storage.h"
/*Save Cookies*/
void cookies_storage_add(cookies_storage storage,gchar* cookie, gchar * url)
{
    char *posend;

    g_print("\n---------------------------\nCookies=%s\n---------------\n", cookie);
    posend = strchr(cookie, ';');
    if (posend != NULL) {
		char *new_cookies;
		char *old_cookies;
		new_cookies =
		    calloc(strlen(*storage) + posend - cookie + 2,
			   sizeof(gchar));
			strcpy(new_cookies, *storage);
			strncat(new_cookies, cookie, posend - cookie + 1);
			old_cookies = *storage;
			*storage = new_cookies;
			free(old_cookies);
			g_print("NewCookies=%s\n", *storage);
    }
}

gchar* cookies_storage_get(cookies_storage storage, gchar * url)
{
	return *storage;
}
