#ifndef _NI_POIMATCH_API_H_
#define _NI_POIMATCH_API_H_

#include <sys/types.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <stdint.h>
#include <algorithm>
#include <limits>
#include <cmath>
#include <stdarg.h>


using namespace std;

class SegmentWrapper
{   
    public:
    SegmentWrapper( );
    ~SegmentWrapper( );

    bool init( const char *dict_dir, int type );
    int cut( string &sentence, vector<STWordInfo> &words );

    public:
    void *env_h;
};

#endif // _NI_POIMATCH_API_H_
