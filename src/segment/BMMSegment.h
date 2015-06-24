
#ifndef _NISEGMENT_BMMSEGMENT_H_
#define _NISEGMENT_BMMSEGMENT_H_


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


using namespace std;

//逆向最大匹配
class BMMSegment
{
    public:
    BMMSegment()
    {
        m_pDa   = NULL;
        m_pDict = NULL;
    }

    bool init( Dictionary *pDict )
    {
        m_pDa   = &(pDict->m_Da);
        m_pDict = pDict;

        return true;
    }

    bool cut( string &sentence, vector<string> &words )
    {
        words.clear();

        size_t char_len = sentence.size();
        //std::cout<< "sentence size is "<< char_len << std::endl;
        if ( char_len<1 )
            return false;
    
    
        // cut the sentent to several words
        size_t      pos_s(0), w_len(0);
        Darts::DoubleArray::result_pair_type  result_pair[100];

        vector<unsigned short> vec_len;
        decode_len(sentence, vec_len);
    
        while ( pos_s<char_len-1 )
        {
            if ( pos_s>=char_len )
                break;
        
            //std::cout<< "  search word ==>"<< prob_word<< "  "<< std::endl;
            
            size_t num = m_pDa->commonPrefixSearch( sentence.c_str()+pos_s, result_pair, sizeof(result_pair) );
    
            if (num==0)
            {
                decode_len2( sentence.c_str()+pos_s, w_len );
                words.push_back( sentence.substr(pos_s,w_len) );
                pos_s += w_len; 
            }
            else
            {
                words.push_back(sentence.substr(pos_s, result_pair[num-1].length));
                pos_s += result_pair[num-1].length;
            }
        }
        
        return true; 
    }


    bool cut( string &sentence, vector<STWordInfo> &words )
    {
        return false;
    }
};



#endif // _NISEGMENT_BMMSEGMENT_H_
