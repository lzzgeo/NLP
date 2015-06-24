
#ifndef _NISEGMENT_MPSEGMENT_H_
#define _NISEGMENT_MPSEGMENT_H_

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

#include "CUtil.h"
#include "darts.h"
#include "SegmentStruct.h"
#include "Dictionary.h"
#include "ISegment.h"



using namespace std;

class MPSegment : public ISegment
{
    public:
    MPSegment();

    bool cut( string &sentence, vector<STWordInfo> &words );

    bool build_DAG( string &sentence, SegmentContext &segContext );

    bool calc_DP( SegmentContext &segContext );
    
    bool print_DAG( SegmentContext &segContext );

    bool print_DP( SegmentContext &segContext );
};

#endif // _NISEGMENT_MPSEGMENT_H_
