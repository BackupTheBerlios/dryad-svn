INCLUDES = $(all_includes)
METASOURCES = AUTO
noinst_HEADERS =  plugin.h

pkglib_LTLIBRARIES = @PKGLIBS@
EXTRA_LTLIBRARIES = libemail.la libnull.la libfile.la libexternal.la libpython.la

if EMAILPLUG
libemail_la_LDFLAGS = -module 
libemail_la_SOURCES = email.cpp
# pkglib_LTLIBRARIES += libemail.la
endif

if NULLPLUG
libnull_la_LDFLAGS = -module 
libnull_la_SOURCES =  null.cpp
# pkglib_LTLIBRARIES += libnull.la
endif

if FILEPLUG
libfile_la_LDFLAGS = -module
libfile_la_SOURCES =  file.cpp
# pkglib_LTLIBRARIES += libfile.la
endif

if EXTERNALPLUG
libexternal_la_LDFLAGS = -module 
libexternal_la_SOURCES =  external.cpp
# pkglib_LTLIBRARIES += libexternal.la
endif

if PYTHONPLUG
libpython_la_LDFLAGS = -module 
libpython_la_SOURCES =  python.cpp
endif

