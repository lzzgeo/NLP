
#ifndef _NI_SEGMENT_DICTIONARY_H_
#define _NI_SEGMENT_DICTIONARY_H_


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


#include "CUtil.h"
#include "darts.h"
#include "SegmentStruct.h"

using namespace std;

typedef struct st_result_pair{
    int val;
    int len;
}STResultPair;


// 字典基本信息
typedef struct st_dict_info{
    int                 refCount;
    long                totalFreq;
    long                minFreq;
    double              minLogFreq;
    VecWords            words; 
    Darts::DoubleArray  da;
}STDictInfo;


typedef map<string, STDictInfo*>     MapDictInfo;    //key-->dir, value-->dictionary

/**
 *
 * @details 在load, unload过程中增加锁定处理，其他操作未进行锁定处理，未避免效率下降。
 *          在进行其他操作时，请不要调用load, unload操作。
 */
class Dictionary
{
    public:
    Dictionary(); 
    ~Dictionary(); 

    bool    loadDict( const char *dict_f );
    bool    unloadDict( );

    bool    save_darts( const char *dict_da );
    int     commonPrefixSearch( const char *sentence, STResultPair *res, size_t num );

    long    getMinFreq( );
    double  getMinLogFreq( );
    int     getWordInfo( long idx, STWordInfo &wInfo );
    
    private:
    string                  m_dictDir;
    STDictInfo              *m_pDInfo;

    static pthread_mutex_t  dict_mutex; 
    static MapDictInfo      dictInfo_pool;
};





#endif // _NI_SEGMENT_DICTIONARY_H_
