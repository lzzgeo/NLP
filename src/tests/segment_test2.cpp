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
#include "SegmentStruct.h"
#include "darts.h"
#include "Dictionary.h"
#include "MPSegment.h"
#include "FullSegment.h"
#include "FMMSegment.h"

using namespace std;


int MP_test( Dictionary *pDict ) 
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
        mpSeg.cut( s_sentence, words2 );
    

        std::cout << "\tword num:" << words2.size() << endl;
        for ( size_t i=0; i<words2.size(); ++i)
            std::cout << "\t"<< words2[i].word << "\t\t" << words2[i].freq << "\t\t" << words2[i].tag1 << "\t\t" <<  words2[i].tag2 << endl;

        std::cout << "-----------"<< std::endl;

        words2.clear();
    }
  
    return 0;
}

int fmm_test( Dictionary *pDict ) 
{
    Darts::DoubleArray::key_type    sentence[1024];
    vector<string>                  words;
    string                          s_sentence;
    FMMSegment                  fSeg;
  
    fSeg.init( pDict );

    while (std::cin.getline(sentence, sizeof(sentence)))
    {
        s_sentence = sentence;
        size_t char_len = s_sentence.size();
        std::cout<< "sentence size is "<< char_len << std::endl;
        if (char_len<1)
            continue;
    
        // cut the sentent to several words
        fSeg.cut( s_sentence, words );
         
    
        std::cout << " word num:" << words.size() << "====>> ";
        for ( size_t i=0; i<words.size(); ++i)
            std::cout << words[i] << " | ";
        std::cout << std::endl;
        words.clear();
    }
  
    return 0;
}


int full_test( Dictionary *pDict ) 
{
    Darts::DoubleArray::key_type    sentence[1024];
    vector<string>                  words;
    string                          s_sentence;
    FullSegment                     fSeg;
  
    fSeg.init( pDict );

    while (std::cin.getline(sentence, sizeof(sentence)))
    {
        s_sentence = sentence;
        size_t char_len = s_sentence.size();
        std::cout<< "sentence size is "<< char_len << std::endl;
        if (char_len<1)
            continue;
    
        // cut the sentent to several words
        fSeg.cut( s_sentence, words );
         
    
        std::cout << " word num:" << words.size() << "====>> ";
        for ( size_t i=0; i<words.size(); ++i)
            std::cout << words[i] << " | ";
        std::cout << std::endl;
        words.clear();
    }
  
    return 0;
}


int main(int argc, char **argv) 
{
    if (argc < 2)
    {
      std::cerr << "Usage: " << argv[0] << " Index" << std::endl;
      return -1;
    }
  
    
    //构建DoubleArray
    std::string index = argv[argc-1];
    //Darts::DoubleArray da;
    //if (da.open(index.c_str()))
    //{
    //  std::cerr << "Error: cannot open " << index << std::endl;
    //  return -1;
    //}


    //构建Dictionary
    Dictionary  dict;
    dict.loadDict(index.c_str());
    dict.build_darts( );
    //dict.save_darts("./tests/jieba_dict.dar");
    

    // 正向最大匹配算法
    //fmm_test( &dict );
    
    
    // 全切分
    //full_test( &dict );
  
    
    // 最大概率匹配
    MP_test( &dict );


    return 0;
}
