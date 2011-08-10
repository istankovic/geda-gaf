
SUBDIRS = include src tests docs

pkgconfigdir   = $(libdir)/pkgconfig
pkgconfig_DATA = libeda.pc

libeda-pc-install: libeda.pc
	$(mkinstalldirs) $(DESTDIR)$(pkgconfigdir)
	$(INSTALL_DATA) libeda.pc $(DESTDIR)$(pkgconfigdir)

MOSTLYCLEANFILES = *.log core FILE *~
CLEANFILES = *.log core FILE *~
DISTCLEANFILES = *.log core FILE *~ libeda.pc
MAINTAINERCLEANFILES = *.log core FILE *~ Makefile.in libeda.pc
