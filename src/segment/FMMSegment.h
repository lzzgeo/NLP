

#ifndef _NISEGMENT_FMMSEGMENT_H_
#define _NISEGMENT_FMMSEGMENT_H_

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


//正向最大匹配
class FMMSegment:public ISegment
{
    public:
    FMMSegment()
    {
    }

    bool cut( string &sentence, vector<string> &words )
    {
        words.clear();

        size_t char_len = sentence.size();
        //std::cout<< "sentence size is "<< char_len << std::endl;
        if ( char_len<1 )
            return false;
    
    
        // cut the sentent to several words
        size_t       pos_s(0), w_len(0);
        STResultPair result_pair[20];

        vector<char> vec_len;
        Utf8_LetterLen( sentence, vec_len );
    
        while ( pos_s<char_len-1 )
        {
            if ( pos_s>=char_len )
                break;
        
            //std::cout<< "  search word ==>"<< prob_word<< "  "<< std::endl;
            
            size_t num = m_pDict->commonPrefixSearch( sentence.c_str()+pos_s, result_pair, sizeof(result_pair) );
    
            if (num==0)
            {
                w_len = Utf8_FirstLetterLen( sentence.c_str()+pos_s );
                words.push_back( sentence.substr(pos_s,w_len) );
                pos_s += w_len; 
            }
            else
            {
                words.push_back(sentence.substr(pos_s, result_pair[num-1].len));
                pos_s += result_pair[num-1].len;
            }
        }
        
        return true; 
    }
    
    bool cut( string &sentence, vector<STWordInfo> &words )
    {
        return false;
    }
};

#endif // _NISEGMENT_FMMSEGMENT_H_
