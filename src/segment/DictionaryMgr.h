
#ifndef _NI_SEGMENT_DICTIONARY_MGR_H_
#define _NI_SEGMENT_DICTIONARY_MGR_H_



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

using namespace std;


typedef struct st_dictionaroy_info{
    int                 refCount;
    long                m_totalFreq;
    long                m_minFreq;
    double              m_minLogFreq;
    VecWords            m_Words; 
    Darts::DoubleArray  m_Da;
}STDictInfo;

typedef map<string, STDictInfo>     MapDict;


class DictionaryMgr
{
    private:
    DictionaryMgr(); 
    DictionaryMgr(Dictionary const &dict); 
    DictionaryMgr& operator=(Dictionary const &dict); 

    public:
    static DicionaryMgr &getInstance( );
    static STDictInfo *loadDict( const char *dict_f );
    static int unloadDict( const char *dict_f );
    
    private:
    static DicionaryMgr     *pDictMgr;

    static pthread_mutex_t  dict_mutex;
    static MapDict          mapDict;
};

#endif // _NI_SEGMENT_DICTIONARY_MGR_H_
