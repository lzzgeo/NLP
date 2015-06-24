
#ifndef _NISEGMENT_STRUCT_H_
#define _NISEGMENT_STRUCT_H_

#include <sys/types.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <stdint.h>
#include <algorithm>
#include <limits>
#include <cmath>

using namespace std;

const double MIN_DOUBLE = -3.14e+100;
const double MAX_DOUBLE = 3.14e+100;

class STWordInfo
{
    public:
    string      word;
    string      tag1;
    string      tag2;
    uint32_t    freq;
    float       logFreq;

    STWordInfo( ): freq(0),logFreq(MIN_DOUBLE)
    {
    }
    void clear( )
    {
        word.clear();
        tag1.clear();
        tag2.clear();

        freq    = 0;
        logFreq = MIN_DOUBLE;
    }
    bool operator<(const STWordInfo& w2) const
    {
        return word < w2.word;
    }
    bool operator>(const STWordInfo& w2) const
    {
        return word > w2.word;
    }
};

typedef vector<STWordInfo>          VecWords;

#endif// _NISEGMENT_STRUCT_H_
