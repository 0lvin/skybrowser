skybrowserdir = ../

INCLUDES = 				\
	-I$(top_srcdir)			\
	-I$(srcdir)			\
	-I$(includedir)			\
	$(GTKHTML_CFLAGS)		\
	$(SOUP_CFLAGS)			\
	-pedantic

LDADD = $(SOUP_LIBS)                    \
        $(GTKHTML_LIBS)

bin_PROGRAMS = skybrowser

skybrowser_SOURCES = skybrowser.c loaders.c

