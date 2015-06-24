#ifndef _NI_POIFORMAT_H_
#define _NI_POIFORMAT_H_

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

#include "darts.h"
#include "SegmentStruct.h"
#include "Dictionary.h"
#include "MPSegment.h"
#include "FullSegment.h"
#include "FMMSegment.h"

#include "POIInfo.h"



using namespace std;

class POIFormat
{
    public:
    POIFormat();

    ~POIFormat( );

    int init( Dictionary *pDict );

    int parse_adminName( string &sentence, POIInfo &poi );

    int format_name ( POIInfo &poi );

    int format_telephone ( POIInfo &poi );
    
    int format_address ( POIInfo &poi );

    int format( POIInfo &poi );

    public:
    MPSegment       mpSegment;
    
    //regex
    pcre            *pcre_bracket; 
    pcre            *pcre_street; 
    pcre            *pcre_hsnm; 
    pcre            *pcre_room; 
    pcre            *pcre_tel; 

    const char      *error;
    int             errffset;
};

#endif  // _NI_POIFORMAT_H_
