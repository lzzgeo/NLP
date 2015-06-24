#ifndef _NI_POIMATCH_API_H_
#define _NI_POIMATCH_API_H_

#include <sys/types.h>
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


#include "POIInfo.h"


using namespace std;

class POIMatchWrapper
{   
    public:
    POIMatchWrapper( );
    ~POIMatchWrapper( );

    bool init( const char *dict_dir, const char *host, int port );

    // return <0  有错误，无法正确执行
    // return 0   无匹配结果
    // return >0  匹配结果数目
    int match( POIInfo &poi_src, vector<POIInfo> &rs );


    public:
    void *env_h;
};

#endif // _NI_POIMATCH_API_H_
