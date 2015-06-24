#ifndef _NI_POIMATCH_H_
#define _NI_POIMATCH_H_


#include <sys/types.h>
#include <darts.h>
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <set>
#include <vector>
#include <stdint.h>
#include <algorithm>
#include <limits>
#include <cmath>
#include <stdarg.h>
#include <pcre.h>

#include "CUtil.h"

#include "mysql.h"
#include "sphinxclient.h"

#include "darts.h"
#include "SegmentStruct.h"
#include "Dictionary.h"
#include "MPSegment.h"
#include "FullSegment.h"
#include "FMMSegment.h"

#include "POIInfo.h"
#include "POIFormat.h"


using namespace std;

static sphinx_bool g_smoke = SPH_FALSE;
static int g_failed = 0;

class POIMatch
{
    public:
    POIMatch( );
    ~POIMatch( );

    int init( const char *host, int port );
   
    // build base query: admincode, kind, core name, spatial location 
    int prepare_query_loc( double lon, double lat, double geodist );
    int prepare_query_admincode( POIInfo &poi );
    int prepare_query_kind( string kind );
    
    sphinx_result *query_POI_common( POIInfo &poi, int &array_num );

    // full match
    sphinx_result *query_POI_100( POIInfo &poi );

    // (name | address) && tel 
    sphinx_result *query_POI_95( POIInfo &poi );
  
    // city, tel
    sphinx_result *query_POI_90( POIInfo &poi );

    // name && address
    sphinx_result *query_POI_90_2( POIInfo &poi );

    // name_core && street && house_number
    sphinx_result *query_POI_90_3( POIInfo &poi );
   
    //(loc || city) && core_name
    sphinx_result *query_POI_80( POIInfo &poi );

    // name  
    sphinx_result *query_POI_60( POIInfo &poi );
   
    // loc && kind
    sphinx_result *query_POI_60_2( POIInfo &poi );

    int filter_POI( POIInfo &poi, vector<POIInfo> &pois );

    int match( POIInfo &poi, vector<POIInfo> &pois );
    int match2( POIInfo &poi, vector<POIInfo> &pois );

    int print_res( sphinx_result *res );

    int close();

    int calcGeoDelta( double geodist, double lat, double &lon_delta, double &lat_delta );

    public:
	sphinx_client   *client;
    string          index;

    // to calc delta of POI
    double          m_geodist; //meter
    double          minor_semi_axis; // meter
    double          major_semi_axis; // meter
};

#endif  // _NI_POIMATCH_H_
