INCLUDES = $(all_includes)
METASOURCES = AUTO
noinst_HEADERS =  plugin.h

pkglib_LTLIBRARIES = @PKGLIBS@
EXTRA_LTLIBRARIES = libemail.la libnull.la libfile.la libexternal.la libpython.la

if EMAILPLUG
libemail_la_LDFLAGS = -module -rpath '$(pkglibdir)'
libemail_la_SOURCES = email.cpp
endif

if NULLPLUG
libnull_la_LDFLAGS = -module -rpath '$(pkglibdir)'
libnull_la_SOURCES =  null.cpp
endif

if FILEPLUG
libfile_la_LDFLAGS = -module -rpath '$(pkglibdir)'
libfile_la_SOURCES =  file.cpp
endif

if EXTERNALPLUG
libexternal_la_LDFLAGS = -module -rpath '$(pkglibdir)'
libexternal_la_SOURCES =  external.cpp
endif

if PYTHONPLUG
libpython_la_LDFLAGS = -module -rpath '$(pkglibdir)'
libpython_la_SOURCES =  python.cpp
endif

