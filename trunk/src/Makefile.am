bin_PROGRAMS = dryad
dryad_SOURCES = dryad.cpp database.cpp analyze.cpp dstring.cpp dfilestream.cpp dsarray.cpp conf.cpp network.cpp dqueue.cpp

# set the include path found by configure
INCLUDES = $(all_includes)

# the library search path.
dryad_LDFLAGS = $(all_libraries) 
noinst_HEADERS = database.h dblist.h analyze.h dstring.h dfilestream.h dsarray.h conf.h network.h dqueue.h cache.h
dryad_LDADD = -lpcre
