bin_PROGRAMS = dryad
dryad_SOURCES = dryad.cpp database.cpp analyze.cpp dstring.cpp dfilestream.cpp

# set the include path found by configure
INCLUDES= $(all_includes)

# the library search path.
dryad_LDFLAGS = $(all_libraries) 
noinst_HEADERS = database.h dblist.h analyze.h dstring.h dfilestream.h dsarray.h
dryad_LDADD = -lpcre
