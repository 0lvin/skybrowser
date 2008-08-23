#!/bin/bash
if [ ! -e Makefile.am ]; then 
cat >Makefile.am<<EOF
SUBDIRS=src
EOF
nano Makefile.am
fi

if [ ! -e src/Makefile.am ]; then 
cat >src/Makefile.am<<EOF
skybrowserdir = ../
bin_PROGRAMS = skybrowser
skybrowser_SOURCES = skybrowser.c
EOF
nano src/Makefile.am
fi;

test ! -e INSTALL &&touch INSTALL
test ! -e NEWS &&touch NEWS
test ! -e README &&touch README
test ! -e AUTHORS &&touch AUTHORS
test ! -e ChangeLog &&touch ChangeLog
test ! -e COPYING &&touch COPYING

test ! -e configure.ac && autoscan && echo AM_INIT_AUTOMAKE >>configure.scan && nano configure.scan && mv configure.scan configure.ac 
aclocal
autoconf
autoupdate
autoheader
automake --add-missing
autoreconf


