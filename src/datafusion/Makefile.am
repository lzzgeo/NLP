AUTOMAKE_OPTIONS=foreign


lib_LTLIBRARIES=libdatafusion.la
libdatafusion_la_SOURCES=Dictionary.h \
						 ISegment.h \
						 BiMMSegment.h \
						 BMMSegment.h \
						 FMMSegment.h \
						 MPSegment.h \
						 FullSegment.h \
						 POIInfo.cpp \
						 POIFormat.cpp \
						 DataFusion.h \
						 POIMatch.h \
						 POIMatch.cpp \
						 POIMatch_API.cpp
libdatafusion_la_LIBADD=../darts/.libs/libdarts.la \
						../common/.libs/libcommon.la \
						../segment/.libs/libsegment.la \
						/usr/local/lib/libsphinxclient.la \
						/usr/local/lib/libpcre.la
libdatafusion_la_LDFLAGS=
libdatafusion_la_CFLAGS=-I$(top_srcdir)/src/darts \
						-I$(top_srcdir)/src/common \
						-I$(top_srcdir)/src/segment \
						-I$(top_srdir)/src/datafusion


DEFS+=-D_GNU_SOURCE
LIBS= -L/usr/local/lib -L/usr/local/mysql/lib -lpthread -lmysqlclient

nobase_include_HEADERS=POIInfo.h POIMatch_API.h

#data_DATA = ../data/jieba_dict.txt

INCLUDES=-I./ -I/usr/local/mysql/include/ \
		 -I./ -I/usr/local/include/ \
		 -I$(top_srcdir)/src/darts \
		 -I$(top_srcdir)/src/common \
		 -I$(top_srcdir)/src/segment \
		 -I$(top_srcdir)/src/datafusion

