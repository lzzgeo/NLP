AUTOMAKE_OPTIONS=foreign


lib_LTLIBRARIES=libsegment.la
libsegment_la_SOURCES=SegmentStruct.h \
					  Dictionary.h Dictionary.cpp \
					  ISegment.h \
					  BiMMSegment.h \
					  BMMSegment.h \
					  FMMSegment.h \
					  MPSegment.h \
					  MPSegment.cpp \
					  FullSegment.h \
					  Segment_API.h \
					  Segment_API.cpp
libsegment_la_LIBADD=../darts/.libs/libdarts.la \
					 ../common/.libs/libcommon.la
libsegment_la_LDFLAGS=
libsegment_la_CFLAGS=-I$(top_srcdir)/src/darts \
					 -I$(top_srcdir)/src/common \
					 -I$(top_srcdir)/src/segment

DEFS+=-D_GNU_SOURCE
LIBS= -L/usr/local/lib

nobase_include_HEADERS=SegmentStruct.h	Segment_API.h 

INCLUDES=-I./ -I/usr/local/include/ \
		 -I$(top_srcdir)/src/darts \
		 -I$(top_srcdir)/src/common \
		 -I$(top_srcdir)/src/segment
