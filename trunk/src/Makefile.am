bin_PROGRAMS = dryad
dryad_SOURCES = dryad.cpp database.cpp analyze.cpp dstring.cpp dfilestream.cpp conf.cpp network.cpp dqueue.cpp darray.cpp drarray.cpp cache.cpp rfc3164.cpp

# set the include path found by configure
INCLUDES = $(all_includes)

# the library search path.
dryad_LDFLAGS = $(all_libraries) 
noinst_HEADERS = database.h dblist.h analyze.h dstring.h dfilestream.h conf.h network.h dqueue.h cache.h darray.h drarray.h rfc3164.h syslog.h
dryad_LDADD = -lpcre
