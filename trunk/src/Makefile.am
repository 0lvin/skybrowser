skybrowserdir = ../

INCLUDES = 				\
	-I$(top_srcdir)			\
	-I$(srcdir)			\
	-I$(includedir)			\
	$(GTKHTML_CFLAGS)		\
	$(SOUP_CFLAGS)

LDADD = $(SOUP_LIBS)                    \
        $(GTKHTML_LIBS)

bin_PROGRAMS = skybrowser

skybrowser_SOURCES = skybrowser.c

