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
    Darts::DoubleArray::key_type    sentence[1024];
    string                          s_sentence;
    MPSegment                       mpSeg;
    vector<STWordInfo>              words2;
    
    mpSeg.init( pDict );

    while (std::cin.getline(sentence, sizeof(sentence)))
    {
        s_sentence = sentence;
        if ( s_sentence.size() < 1 ) 
            continue;
    

        // cut the sentent to several words
        seg->cut2( s_sentence, words2 );
    

        std::cout << "\tword num:" << words2.size() << endl;
        for ( size_t i=0; i<words2.size(); ++i)
            words2[i].print();
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
    if ( seg.init ( argv[1], "192.168.15.106", 9362 ) < 0 )
    {
        cout << " failed to init...." << end;
        return 0;
    }


    MP_test( &seg );


    return 0;
}
