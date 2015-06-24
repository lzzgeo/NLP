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

#include "SegmentStruct.h"
#include "Segment_API.h"

using namespace std;


int MP_test( SegmentWrapper *seg ) 
{
    char                 sentence[2048];
    string               s_sentence;
    vector<STWordInfo>   words2;
    
    while (std::cin.getline(sentence, sizeof(sentence)))
    {
        s_sentence = sentence;
        if ( s_sentence.size() < 1 ) 
            continue;
    
        // cut the sentent to several words
        seg->cut( s_sentence, words2 );

        std::cout << "\tword num:" << words2.size() << endl;
        for ( size_t i=0; i<words2.size(); ++i )
        {
            cout << words2[i].word << "\t" 
                << words2[i].freq << "\t" 
                << words2[i].logFreq << "\t"
                << words2[i].tag1 << "\t"
                << words2[i].tag2 << endl;
            //pWInfo->print( );
        }

        std::cout << "-----------"<< std::endl;

        words2.clear();
    }
  
    return 0;
}

int main(int argc, char **argv) 
{
    if (argc < 2)
    {
      std::cerr << "Usage: " << argv[0] << " Dict" << std::endl;
      return -1;
    }
  
    
    // 最大概率匹配
    SegmentWrapper seg;
    if ( seg.init ( argv[1], 0 ) < 0 )
    {
        cout << " failed to init...." << endl;
        return 0;
    }


    MP_test( &seg );


    return 0;
}
