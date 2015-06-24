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

#include "CUtil.h"
#include "darts.h"
#include "Dictionary.h"
#include "Segment_API.h"
#include "ISegment.h"
#include "MPSegment.h"


using namespace std;

class SegmentHandle
{
    public:
    SegmentHandle( ) { /*cout << "SegmentHandle* " << endl;*/ }
    ~SegmentHandle( ) {}

    public:
    Dictionary  dict;
    MPSegment   mpSeg;
};

SegmentWrapper::SegmentWrapper( )
{
    env_h = NULL;
}

SegmentWrapper::~SegmentWrapper( )
{
    SegmentHandle *seg_h = (SegmentHandle *) env_h;
    delete seg_h;
    env_h = NULL;
}

bool SegmentWrapper::init( const char *dict_dir, int type )
{
    SegmentHandle *h_ = new SegmentHandle( );
    //cout << dict_dir << endl;

    try{
        // init dictionary
        if ( h_->dict.loadDict(dict_dir) < 1 )
            return false;
    
        h_->mpSeg.init( &h_->dict );
    
        env_h = (void *)h_; 
    }
    catch(...)
    {
        cout << "segmentwrapper init error" << endl;
        return false;
    }

    return true;
}

int SegmentWrapper::cut( string &sentence, vector<STWordInfo> &words )
{
    words.clear();

    SegmentHandle *seg_h = (SegmentHandle *) env_h;
    seg_h->mpSeg.cut( sentence, words );

    return 1;
}
