#ifndef _NI_SEGMENT_FULLSEGMENT_H_
#define _NI_SEGMENT_FULLSEGMENT_H_
/*
  Darts -- Double-ARray Trie System

  $Id: darts.cpp 1674 2008-03-22 11:21:34Z taku $;

  Copyright(C) 2001-2007 Taku Kudo <taku@chasen.org>
  All rights reserved.
*/

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

#include "darts.h"
#include "SegmentStruct.h"
#include "Dictionary.h"
#include "ISegment.h"


using namespace std;


//全切分
class FullSegment:public ISegment
{
    public:
    FullSegment()
    {
        m_pDict = NULL;
    }

    bool cut( string &sentence, vector<string> &words )
    {
        words.clear();

        size_t char_len = sentence.size();
        //std::cout<< "sentence size is "<< char_len << std::endl;
        if ( char_len<1 )
            return false;
    
    
        // cut the sentent to several words
        size_t          pos_s(0), w_len(0);
        //set<string>   word_set;
        STResultPair    result_pair[20];

        vector<char> vec_len;
        Utf8_LetterLen( sentence, vec_len );
    
        while ( pos_s<char_len-1 )
        {
            if ( pos_s>=char_len )
                break;
        
            //std::cout<< "  search word ==>"<< prob_word<< "  "<< std::endl;
            w_len = Utf8_FirstLetterLen( sentence.c_str()+pos_s );
            
            size_t num = m_pDict->commonPrefixSearch( sentence.c_str()+pos_s, result_pair, sizeof(result_pair) );
            if ( num==0 )
            {
                //word_set.insert( sentence.substr(pos_s,w_len) );
                words.push_back( sentence.substr(pos_s,w_len) );
            }
            else
            {
                w_len = result_pair[0].len;
                for ( size_t i=0; i<num; ++i )
                {
                    //word_set.insert( sentence.substr(pos_s, result_pair[i].len) );
                    words.push_back( sentence.substr(pos_s, result_pair[i].len) );
                    w_len = (result_pair[i].len < w_len) ? result_pair[i].len : w_len;
                }
            }

            pos_s += w_len; 
        }

        /*
        set<string>::iterator   itr     = word_set.begin();
        set<string>::iterator   itrEnd  = word_set.end();
        for ( ; itr!=itrEnd; ++itr )
        {
            words.push_back( *itr );
        }
        */      
        return true; 
    }

    bool cut2( string &sentence, vector<STWordInfo> &words )
    {
        return false;
    }
};
#endif // _NI_SEGMENT_FULLSEGMENT_H_
