#ifndef _NISEGMENT_ISEGMENT_H_
#define _NISEGMENT_ISEGMENT_H_

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
#include <regex.h>

#include "darts.h"
#include "CUtil.h"
#include "SegmentStruct.h"
#include "Dictionary.h"

using namespace std;


typedef map<size_t, STWordInfo>     DagType;
typedef vector<unsigned int>        VecUnicode4;
typedef vector<uint16_t>            VecUnicode2;

// sentence 中每个字节节点( char 类型)
typedef struct st_char_node_info{
    string          ch;
    size_t          nextpos;  // next word
    //size_t          len;
    DagType         dag;
    double          weight;
    STWordInfo      *pWInfo;

    //st_char_node_info():nextpos(0),len(0),weight(MIN_DOUBLE),pWInfo(NULL)
    st_char_node_info():nextpos(0),weight(MIN_DOUBLE),pWInfo(NULL)
    {
    }
    //st_char_node_info(string s):ch(s),nextpos(0),len(0),weight(MIN_DOUBLE),pWInfo(NULL)
    st_char_node_info(string s):ch(s),nextpos(0),weight(MIN_DOUBLE),pWInfo(NULL)
    {
    }
    void clear()
    {
        ch.clear();
        dag.clear();
        //nextpos = len = 0;
        nextpos = 0;
        weight  = MIN_DOUBLE;
        pWInfo  = NULL;
    }
}STCharNodeInfo;

// sentence 中每个字节点(unicode2 类型)
typedef struct st_unicode2_node_info{
    uint16_t        ch;
    DagType         dag;
    double          weight;
    STWordInfo      *pWInfo;

    st_unicode2_node_info(uint16_t ch_v):ch(ch_v),weight(MIN_DOUBLE),pWInfo(NULL)
    {
    }
}STUnicode2NodeInfo;



typedef vector<STCharNodeInfo>      SegmentContext;
//typedef vector<STUnicode2NodeInfo>  SegmentContext2;





class ISegment
{
    public:
    ISegment()
    {
        //m_pDa   = NULL;
        m_pDict = NULL;
        /*sp_char ={ '０', '0', '１', '1', '２', '2', '３', '3', '４', '4','５', '5', '６', '6', '７', '7', '８', '8', '９', '9',
                    'Ａ', 'A', 'Ｂ', 'B', 'Ｃ', 'C', 'Ｄ', 'D', 'Ｅ', 'E','Ｆ', 'F', 'Ｇ', 'G', 'Ｈ', 'H', 'Ｉ', 'I', 'Ｊ', 'J','Ｋ', 'K', 'Ｌ', 'L', 'Ｍ', 'M', 'Ｎ', 'N', 'Ｏ', 'O',      
                    'Ｐ', 'P', 'Ｑ', 'Q', 'Ｒ', 'R', 'Ｓ', 'S', 'Ｔ', 'T','Ｕ', 'U', 'Ｖ', 'V', 'Ｗ', 'W', 'Ｘ', 'X', 'Ｙ', 'Y',      
                    'Ｚ', 'Z', 'ａ', 'a', 'ｂ', 'b', 'ｃ', 'c', 'ｄ', 'd','ｅ', 'e', 'ｆ', 'f', 'ｇ', 'g', 'ｈ', 'h', 'ｉ', 'i',      
                    'ｊ', 'j', 'ｋ', 'k', 'ｌ', 'l', 'ｍ', 'm', 'ｎ', 'n','ｏ', 'o', 'ｐ', 'p', 'ｑ', 'q', 'ｒ', 'r', 'ｓ', 's',      
                    'ｔ', 't', 'ｕ', 'u', 'ｖ', 'v', 'ｗ', 'w', 'ｘ', 'x','ｙ', 'y', 'ｚ', 'z',      
                    '（', ' ', '）', ' ', '〔', '[', '〕', ']', '【', '[','】', ']', '〖', '[', '〗', ']', '“', '[', '”', ']',      
                    '‘', '[', '’', ']', '｛', '{', '｝', '}', '《', '<', '》', '>',  '％', '%', '＋', '+', '—', '-', '－', '-', '～', '-',      
                    '：', ':', '。', '.', '、', ',', '，', '.', '、', '.', '；', ',', '？', '?', '！', '!', '…', '-', '‖', '|' 
                    */
    };

    bool init( Dictionary *pDict )
    {
        if ( NULL == pDict )
            return false;

        m_pDict = pDict;

        return true;
    }


    virtual bool cut( string &sentence, vector<STWordInfo> &words )
    {
        return false;
    }
    
    int combine( VecWords &words )
    {
        VecWords            words2; 
        STWordInfo          wInfo;
        int                 word_type(0), temp_type;
        

        for ( size_t i=0; i<words.size(); ++i )
        {
           // cout << words[i].word << "\t" << words[i].freq << endl;

            CUtil::strReplace( words[i].word, " ", "");
            if ( Utf8_LetterCount( words[i].word ) < 1 )
                continue;

            if ( words[i].freq>0 )
            {
                if ( 0!=word_type )
                {
                    words2.push_back( wInfo );
                    word_type = 0;
                    wInfo.clear();
                }

                words2.push_back(words[i]);
                continue;
            }

            
            temp_type = get_lettertype( words[i].word ); 
            //cout << words[i].word << "\ttemp_type " << "\t" << temp_type << endl;
            
            // 非数字或英文字母 
            if ( 1!=temp_type && 2!=temp_type  )
            {
                if ( 0!=word_type )
                {
                    words2.push_back( wInfo );
                    word_type = 0;
                    wInfo.clear();
                }

                words2.push_back(words[i]);

                continue;
            }

            //英文字母或数字时，将临近字符组成单词
            if ( 0==word_type )
            {
                word_type   = temp_type;
                wInfo.word  = words[i].word;
            }
            else if ( word_type==temp_type )
            {
                wInfo.word += words[i].word;
            }
            else
            {
                words2.push_back( wInfo );
                
                wInfo.clear();
                word_type   = temp_type;
                wInfo.word  = words[i].word;
            }
        }


        if ( 0!=word_type )
            words2.push_back( wInfo );

   
        words = words2;


        return true;
    }

    public:
    //reference 
    Dictionary          *m_pDict;
};
#endif //ISEGMENT_ISEGMENT_H_
