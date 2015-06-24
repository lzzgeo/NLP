
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
#include "DictionaryMgr.h"

using namespace std;


DictionaryMgr::DictionaryMgr() 
{
    pDictMgr = NULL;
    pthread_mutex_init( &dict_mutex, NULL );
}

DictionaryMgr::~DictionaryMgr() 
{
    pthread_mutex_destroy( dict_mutex );
}

DictionaryMgr &DictionaryMgr::getInstance() 
{
    static Dictionary   dictMgr;
    return dictMgr;
/*
    if ( pDictMgr==NULL )
        pDictMgr = new DictionaryMgr();

    return pDictMgr;
    */
}

STDictInfo  *DictionaryMgr::loadDict( const char *dict_f )
{
    pthread_mutex_lock( dict_mutex );

    // check the pool
    MapDict::iterator   itr = mapDict.find(itr);
    if ( itr!=mapDict.end() )
    {
        itr->sencond.refCount++;
        return &(itr->sencond);
    }

    ifstream        dictf(dict_f);
    string          line; 
    vector<string>  vecBuf;
    STWordInfo      wInfo;
    size_t          split_num(0);
    size_t          word_len(0);

    // load words info
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
        word_len =  Utf8_LetterCount( wInfo.word );
        if ( word_len<2 )
            continue;

        
        // 装载词频
        if ( vecBuf.size()>1 )
        {
            wInfo.freq = atoi(vecBuf[1].c_str());
            if ( 0==wInfo.freq )
                wInfo.freq = 1;

            m_totalFreq += wInfo.freq;
        }


        // 装载词性
        if ( vecBuf.size()>2 )
            wInfo.tag1 = vecBuf[2];


        // 装载级别(行政区划具体级别等)
        if ( vecBuf.size()>3 )
            wInfo.tag2 = vecBuf[3];

        
        // status 为 0 是不装载
        /*if ( vecBuf.size()>4 )
        {
            int i_status = atoi(vecBuf[4].c_str());
           // cout << line << endl;
            if ( i_status < 1 )
                continue;
        }
        */


        m_Words.push_back(wInfo);
    }

    sort( m_Words.begin(), m_Words.end() );
    //printf("finish to load dict: %ld words\n", m_Words.size());


    // build darts
    size_t   i, siz;
    siz = m_Words.size();
    for( i=0; i<m_Words.size(); ++i )
    {
        STWordInfo& word = m_Words[i];

        if ( m_totalFreq > 0 )
            word.logFreq = log(double(word.freq)/double(m_totalFreq));

        if( word.logFreq < m_minLogFreq )
            m_minLogFreq = word.logFreq; 

        if( word.freq < m_minFreq )
            m_minFreq = word.freq; 
    }

    // 构建DoubleArray
    vector<Darts::DoubleArray::key_type*>   vecKeys;
    vector<Darts::DoubleArray::result_type> vecVals;
    vecKeys.resize(siz); 
    vecVals.resize(siz); 
    for ( i=0; i<siz; ++i )
    {
       vecKeys[i] = (char *)(m_Words[i].word.c_str());
       vecVals[i] = i;
    }  

    int error_n = m_Da.build( siz, (const char**)(&vecKeys[0]), 0, &vecVals[0], NULL );

    //printf( "build dartsarray error: %d \n", error_n );
    //std::cout << "Done!, Compression Ratio: " << 100.0 * m_Da.nonzero_size() / m_Da.size() << " %" << std::endl;
    //printf( "finish to build dartsarray----   words: %ld  totalFreq:%ld    minFreq:%ld    minLogFreq:%lf  \n", siz,  m_totalFreq, m_minFreq, m_minLogFreq );


    pthread_mutex_unlock( dict_mutex );


    return true;
}

int DictionaryMgr::loadDict( const char *dict_f )
{
    pthread_mutex_lock( dict_mutex );

    MapDict::iterator   itr = mapDict.find(itr);
    if ( itr!=mapDict.end() )
    {
        if ( 0 == (--itr->sencond.refCount) )
            mapDict.erase(itr);
    }

    pthread_mutex_unlock( dict_mutex );

    return 1;
}
