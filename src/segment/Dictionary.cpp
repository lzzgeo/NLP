
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
#include <pthread.h>

#include "CUtil.h"
#include "darts.h"
#include "SegmentStruct.h"
#include "Dictionary.h"

using namespace std;


MapDictInfo     Dictionary::dictInfo_pool;
pthread_mutex_t Dictionary::dict_mutex = PTHREAD_MUTEX_INITIALIZER;
//pthread_mutex_init( &(Dictionary::dict_mutex), NULL );

//Dictionary::Dictionary():m_totalFreq(0),m_minFreq(0),m_minLogFreq(MAX_DOUBLE) 
Dictionary::Dictionary():m_pDInfo(NULL) 
{

}

Dictionary::~Dictionary() 
{
    unloadDict( );
}

bool Dictionary::loadDict( const char *dict_f )
{
    pthread_mutex_lock( &dict_mutex );


    // the same dictionary
    if ( m_dictDir == dict_f )
    {
        pthread_mutex_unlock( &dict_mutex );
        return true;
    }
   
    // find it in the pool
    if ( dictInfo_pool.find(dict_f)!=dictInfo_pool.end() )
    {
        m_pDInfo = dictInfo_pool[dict_f];
        m_pDInfo->refCount++;
        pthread_mutex_unlock( &dict_mutex );
        return true;
    }


    if ( !m_dictDir.empty() )
        unloadDict( );


    m_pDInfo = new STDictInfo();
    m_pDInfo->refCount    = 1;
    m_pDInfo->totalFreq   = 0;
    m_pDInfo->minFreq     = 0;
    m_pDInfo->minLogFreq  = MAX_DOUBLE;
    dictInfo_pool[dict_f] = m_pDInfo;



    ifstream        dictf(dict_f);
    string          line; 
    vector<string>  vecBuf;
    STWordInfo      wInfo;
    size_t          split_num(0);
    size_t          word_len(0);

    // 构建Dictionary
    while( getline(dictf, line) )
    {
        vecBuf.clear();

        splitStr(line, vecBuf, " ");
        split_num = vecBuf.size();
        if ( split_num < 2 )
            continue;

        // 装载词
        wInfo.word = vecBuf[0];
        if ( wInfo.word.empty() )
            continue;

        // 单字不加载
        word_len =  Utf8_LetterCount( wInfo.word );
        if ( word_len<2 )
            continue;

        
        // 装载词频
        if ( vecBuf.size()>1 )
        {
            wInfo.freq = atoi(vecBuf[1].c_str());
            if ( 0==wInfo.freq )
                wInfo.freq = 1;

            m_pDInfo->totalFreq += wInfo.freq;
        }


        // 装载词性
        if ( vecBuf.size()>2 )
            wInfo.tag1 = vecBuf[2];


        // 装载级别(行政区划具体级别等)
        if ( vecBuf.size()>3 )
            wInfo.tag2 = vecBuf[3];

        
        m_pDInfo->words.push_back(wInfo);
    }
    sort( m_pDInfo->words.begin(), m_pDInfo->words.end() );


    // build darts
    size_t   i, siz;
    siz = m_pDInfo->words.size();
    for( i=0; i<m_pDInfo->words.size(); ++i )
    {
        STWordInfo& word = m_pDInfo->words[i];

        if ( m_pDInfo->totalFreq > 0 )
            word.logFreq = log(double(word.freq)/double(m_pDInfo->totalFreq));

        if( word.logFreq < m_pDInfo->minLogFreq )
            m_pDInfo->minLogFreq = word.logFreq; 

        if( word.freq < m_pDInfo->minFreq )
            m_pDInfo->minFreq = word.freq; 
    }

    // 构建DoubleArray
    vector<Darts::DoubleArray::key_type*>   vecKeys;
    vector<Darts::DoubleArray::result_type> vecVals;
    vecKeys.resize(siz); 
    vecVals.resize(siz); 
    for ( i=0; i<siz; ++i )
    {
       vecKeys[i] = (char *)(m_pDInfo->words[i].word.c_str());
       vecVals[i] = i;
    }  

    int error_n = m_pDInfo->da.build( siz, (const char**)(&vecKeys[0]), 0, &vecVals[0], NULL );


    /*printf( "build dartsarray error: %d \n", error_n );
    std::cout << "Done!, Compression Ratio: " << 100.0 * m_Da.nonzero_size() / m_Da.size() << " %" << std::endl;
    printf( "finish to build dartsarray----   words: %ld  totalFreq:%ld    minFreq:%ld    minLogFreq:%lf  \n", siz,  m_totalFreq, m_minFreq, m_minLogFreq );
    */

    pthread_mutex_unlock( &dict_mutex );

    return true;
}

bool Dictionary::unloadDict( )
{
    pthread_mutex_lock( &dict_mutex );
    //cout << "----in unloadDict" << endl;
    
    if ( !m_dictDir.empty() )
    {
        MapDictInfo::iterator   itr = dictInfo_pool.find( m_dictDir );
        if (itr != dictInfo_pool.end() )
        {
            if ( (--(itr->second->refCount)) <= 0 )
            {   
                delete itr->second;
                dictInfo_pool.erase( itr );
            }
        }   
    }

    m_dictDir.clear();
    m_pDInfo = NULL;


    //cout << "----out unloadDict" << endl;
    pthread_mutex_unlock( &dict_mutex );
}

bool Dictionary::save_darts( const char *dict_da )
{
    if ( NULL != m_pDInfo )
        m_pDInfo->da.save( dict_da );

    return true;
}

long Dictionary::getMinFreq( )
{
    if ( NULL != m_pDInfo )
        return m_pDInfo->minFreq;

    return -1;
}

double Dictionary::getMinLogFreq( )
{
    if ( NULL != m_pDInfo )
        return m_pDInfo->minLogFreq;

    return -1;
}

int Dictionary::getWordInfo( long idx, STWordInfo & wInfo )
{
    if ( NULL == m_pDInfo )
        return -1;
        

    if ( idx<0 || idx>=m_pDInfo->words.size() )
        return -1;
    

    wInfo = m_pDInfo->words[idx];

    return 1;
}

int Dictionary::commonPrefixSearch( const char *sentence, STResultPair *res, size_t num )
{
    if ( NULL == m_pDInfo )
        return -1;

    Darts::DoubleArray::result_pair_type *pR = new Darts::DoubleArray::result_pair_type[num];
    int c = m_pDInfo->da.commonPrefixSearch( sentence, pR, num );
    for ( int i=0; i<num; ++i )
    {
        res[i].val = pR[i].value;
        res[i].len = pR[i].length;
    }

    delete []pR;

    return c;
}
