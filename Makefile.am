if ENABLE_GATTRIB
GATTRIB_DIR=gattrib
endif
SUBDIRS = intl libeda libgeda gschem ${GATTRIB_DIR} gsymcheck gnetlist utils symbols docs \
	examples

ACLOCAL_AMFLAGS = -I m4

# We need to ensure that `make distcheck' *doesn't* find a git
# repository!
#
# We also should be able to successfully build & install without
# groff.
false=/bin/sh -c '! :'
DISTCHECK_CONFIGURE_FLAGS = \
	GIT="$(false)" \
	GROFF="$(false)" \
	--disable-update-xdg-database

BUILT_SOURCES = version.h
EXTRA_DIST = autogen.sh version.h version.h.in missing.h HACKING \
	COPYING.LGPL

# Generate version.h. This works by looking at any git repository that
# is present in the source directory (detected at configure time). If
# there is a git repository, the HEAD commitid is checked every time
# the package is built to see if it has changed. If so, version.h is
# regenerated. If there isn't a git repository, version.h is just
# assumed to be present (since the generated version.h is distributed
# in the source tarball).
if HAVE_GIT_REPO
version.h: stamp-git $(top_builddir)/configure $(srcdir)/version.h.in
	@if test ! -f version.h; then \
	  rm -f stamp-git; \
	  $(MAKE) $(AM_MAKEFLAGS) stamp-git; \
	else :; fi

	@date_ver=$(DATE_VERSION); dotted_ver=$(DOTTED_VERSION); \
	git_commit=`cd $(srcdir) && $(GIT) rev-parse HEAD`; \
	git_ver=`cd $(srcdir) && $(GIT) describe HEAD`; \
	sed -e"s:^.*\(PACKAGE_DATE_VERSION\).*$$:#define \1 \"$$date_ver\":" \
	    -e"s:^.*\(PACKAGE_DOTTED_VERSION\).*$$:#define \1 \"$$dotted_ver\":" \
	    -e"s:^.*\(PACKAGE_GIT_VERSION\).*$$:#define \1 \"$$git_ver\":" \
	    -e"s:^.*\(PACKAGE_GIT_COMMIT\).*$$:#define \1 \"$$git_commit\":" \
	      < $(srcdir)/version.h.in > version.h.new; \
	if diff version.h version.h.new > /dev/null 2>&1; then \
	  echo "version.h is unchanged"; \
	  rm version.h.new; \
	else \
	  echo "Recreating version.h"; \
	  mv version.h.new version.h; \
	fi
stamp-git:
	@last_version=`if test -f stamp-git; then cat stamp-git; else :; fi`; \
	now_version=`cd $(srcdir) && $(GIT) rev-parse HEAD`; \
	if test "X$$now_version" != "X$$last_version"; then \
	  echo $$now_version > stamp-git; \
	  echo "Now at git commit $$now_version..."; \
	fi
.PHONY: stamp-git
endif HAVE_GIT_REPO

doxygen:
if ENABLE_DOXYGEN
	@for subdir in gattrib/docs gschem/docs gnetlist/docs \
	  libeda/docs libgeda/docs gsymcheck/docs utils/docs ; do \
	  (cd $$subdir; $(MAKE) doxygen) || exit 1; \
        done
else
	@echo 'configure was not run with the --enable-doxygen switch.'
	@echo 'The documentation cannot be built from the sources.'
endif

DISTCLEANFILES = stamp-git
MAINTAINERCLEANFILES=Makefile.in aclocal.m4 config.h.in configure
