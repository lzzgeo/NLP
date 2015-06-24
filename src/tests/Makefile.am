AUTOMAKE_OPTIONS=foreign

#bin_PROGRAMS=segment_test segment_test2 datafusion_test POIMatch_test
#bin_PROGRAMS=segment_test datafusion_test POIMatch_test
noinst_PROGRAMS=segment_test datafusion_test POIMatch_test

segment_test_SOURCES=segment_test.cpp
segment_test_LDADD=../darts/.libs/libdarts.la \
				   ../common/.libs/libcommon.la \
				   ../segment/.libs/libsegment.la
segment_test_LDFLAGS=-D_GNU_SOURCE

segment_test2_SOURCES=segment_test2.cpp
MPSegment_test2_LDADD=../darts/.libs/libdarts.la \
				   ../common/.libs/libcommon.la \
				   ../segment/.libs/libsegment.la
segment_test2_LDFLAGS=-D_GNU_SOURCE

datafusion_test_SOURCES=datafusion_test.cpp
datafusion_test_LDADD=../darts/.libs/libdarts.la \
					  ../common/.libs/libcommon.la \
					  ../segment/.libs/libsegment.la \
					  /usr/local/lib/libsphinxclient.la \
					  /usr/local/lib/libpcre.la \
					  ../datafusion/.libs/libdatafusion.la
datafusion_test_LDFLAGS=-D_GNU_SOURCE


POIMatch_test_SOURCES=POIMatch_test.cpp
POIMatch_test_LDADD=../darts/.libs/libdarts.la \
					  ../common/.libs/libcommon.la \
					  ../segment/.libs/libsegment.la \
					  /usr/local/lib/libsphinxclient.la \
					  /usr/local/lib/libpcre.la \
					  ../datafusion/.libs/libdatafusion.la
POIMatch_test_LDFLAGS=-D_GNU_SOURCE


DEFS+=-D_GNU_SOURCE
LIBS= -L/usr/local/lib -L/usr/local/mysql/lib -lpthread -lmysqlclient 

#include_HEADERS=darts.h common.h struct.h
#data_DATA = ../data/jieba_dict.txt

INCLUDES=-I./ -I/usr/local/mysql/include/ \
		 -I./ -I/usr/local/include/ \
		 -I$(top_srcdir)/src/darts \
		 -I$(top_srcdir)/src/common \
		 -I$(top_srcdir)/src/segment \
		 -I$(top_srcdir)/src/datafusion \
		 -I$(top_srcdir)/src/tests
