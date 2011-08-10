
# Build a libtool library for installation in libdir.
lib_LTLIBRARIES = libeda.la

libeda_la_SOURCES = \
		edaconfig.c \
		edaerrors.c \
		edaobject.c \
		edamarshallers.c \
		edanet.c \
		edapage.c \
		edatypes.c \
		util.c

libeda_la_CPPFLAGS = -I$(srcdir)/../include -I$(top_srcdir)
libeda_la_CFLAGS = \
	$(GCC_CFLAGS) $(MINGW_CFLAGS) $(GUILE_CFLAGS) $(GLIB_CFLAGS) \
	$(GDK_PIXBUF_CFLAGS)
libeda_la_LDFLAGS = -version-info $(LIBEDA_SHLIB_VERSION) \
	$(WINDOWS_LIBTOOL_FLAGS) $(MINGW_LDFLAGS) $(GUILE_LIBS) \
	$(GLIB_LIBS) $(GDK_PIXBUF_LIBS)
LIBTOOL=@LIBTOOL@ --silent

MOSTLYCLEANFILES = *.log core FILE *~
CLEANFILES = *.log core FILE *~
DISTCLEANFILES = *.log core FILE *~
MAINTAINERCLEANFILES = *.log core FILE *~ Makefile.in

BUILT_SOURCES = edamarshallers.c edamarshallers.h

edamarshallers.c: edamarshallers.list
	glib-genmarshal --prefix=_eda_marshal edamarshallers.list \
                        --body > edamarshallers.c

edamarshallers.h: edamarshallers.list
	glib-genmarshal --prefix=_eda_marshal edamarshallers.list \
                        --header > edamarshallers.h

# Unfortunately, in order to test libtool thoroughly, we need access
# to its private directory.
objdir = `sed -n -e 's/^objdir=\"\(.*\)\"$$/\1/p' ./libtool`
