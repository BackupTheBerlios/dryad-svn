
# set the include path found by configure
INCLUDES= $(all_includes)

# the library search path.
lib_LTLIBRARIES =  liblrsp.la
liblrsp_la_SOURCES =  lrsp.c

include_HEADERS =  lrsp.h
bin_PROGRAMS =  lrsp-logger
lrsp_logger_SOURCES =  lrsp-logger.c
lrsp_logger_LDADD = $(top_builddir)/src/liblrsp.la
