AM_CPPFLAGS = -I../include/ $(GLIB_CPPFLAGS)
AM_CFLAGS = $(GLIB_CFLAGS)

LDADD = ../src/libeda.la signaltracker.o

TEST_PROGS = test-page test-net

noinst_PROGRAMS = ${TEST_PROGS}

test: ${TEST_PROGS}
	@gtester --verbose ${TEST_PROGS}

