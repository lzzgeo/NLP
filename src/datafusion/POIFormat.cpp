
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
#include <stdarg.h>

#include "POIFormat.h"


using namespace std;


POIFormat::POIFormat()
{
    pcre_bracket    = pcre_compile( "\\([^()]+?\\)", 0, &error, &errffset, NULL );
    pcre_street     = NULL;
    pcre_hsnm       = pcre_compile( "^[\\da-zA-Z]+?", 0, &error, &errffset, NULL );
    pcre_room       = pcre_compile( "[\\da-zA-Z]+?室", 0, &error, &errffset, NULL );
    pcre_tel        = pcre_compile( "[\\d-]+", 0, &error, &errffset, NULL );

    //if ( NULL == pcre_bracket )
    //    cout << "error regex: " << endl;


    //init tag words
/*    string tags_[] = {"公寓", "青年公寓", "会馆", "商务会馆", "国际商务会馆", 
                        "商务", "快捷", "宾馆", "度假村", "酒店", "大酒店", "客栈", "驿站", "旅馆", "青年旅馆", "饭店", 
                        "休闲会馆", "招待所"};
    for ( int i=0; i<sizeof(tags_); ++i )
        tag_words.push_back( tags_[i] );
        */
}

POIFormat::~POIFormat( )
{
    if ( NULL!=pcre_bracket )   pcre_free( pcre_bracket ); 
    if ( NULL!=pcre_street )    pcre_free( pcre_street ); 
    if ( NULL!=pcre_hsnm )      pcre_free( pcre_hsnm ); 
    if ( NULL!=pcre_room )      pcre_free( pcre_room ); 
    if ( NULL!=pcre_tel )       pcre_free( pcre_tel ); 
}

int POIFormat::init( Dictionary *pDict )
{
    mpSegment.init( pDict );

    return 1;
}

int POIFormat::parse_adminName( string &sentence, POIInfo &poi )
{
    vector<STWordInfo>  words;
    mpSegment.cut( sentence, words );

    for ( size_t i=0; i<2 && i<words.size(); ++i )
    {
        if ( poi.prov.empty() && words[i].tag1=="ns1" )
        {
            poi.prov = words[i].word;
            continue;
        }
        
        if ( poi.city.empty() && words[i].tag1=="ns2" )
        {
            poi.city = words[i].word;
            continue;
        }
        
        if ( poi.county.empty() && words[i].tag1=="ns3" )
        {
            poi.county = words[i].word;
            continue;
        }
        
        if ( poi.town.empty() && words[i].tag1=="ns4" )
        {
            poi.town = words[i].word;
            continue;
        }
    }

    return 1;
}

int POIFormat::format_name ( POIInfo &poi )
{
    string              sentence = poi.name;

    // 转换为小写，全角转半角
    CUtil::Str2Lower( sentence );
    sentence = CUtil::UTF8ToDBC( sentence );

    //括号规范化
    CUtil::strReplace( sentence, "（", "(" ); 
    CUtil::strReplace( sentence, "）", ")" ); 
    CUtil::strReplace( sentence, "[", "(" ); 
    CUtil::strReplace( sentence, "]", ")" ); 
    CUtil::strReplace( sentence, "【", "(" ); 
    CUtil::strReplace( sentence, "】", ")" ); 

    
    // extract info in bracket as suffix
    int         pm[10], z;
    size_t      siz;
    string      temp;
    while( true )
    {
        siz = sentence.size();

        z = pcre_exec( pcre_bracket, NULL, sentence.c_str(), siz, 0, 0, pm, 10 );
        //cout << " z value: " << z << endl;
        if ( z < 0 )
            break;

        temp = sentence.substr( pm[0], pm[1]-pm[0] );
        if ( !poi.name_suffix.empty() )
            poi.name_suffix += " ";
        poi.name_suffix += temp;
        
        CUtil::strReplace( sentence, temp, " " ); 
    }
                                         

    // extract location info, and extract name_core 
    if ( !poi.prov.empty() && (sentence.size()-poi.prov.size())>6 )     CUtil::strReplace( sentence, poi.prov, " " ); 
    if ( !poi.city.empty() && (sentence.size()-poi.city.size())>6 )     CUtil::strReplace( sentence, poi.city, " " ); 
    if ( !poi.county.empty() && (sentence.size()-poi.county.size())>6)  CUtil::strReplace( sentence, poi.county, " " ); 
    if ( !poi.town.empty() && (sentence.size()-poi.town.size())>6 )     CUtil::strReplace( sentence, poi.town, " " ); 

    /*
    if ( Utf8_LetterCount(sentence)>5 )
        parse_adminName( sentence, poi );

    if ( !poi.prov.empty() && (sentence.size()-poi.prov.size())>6 )     CUtil::strReplace( sentence, poi.prov, " " ); 
    if ( !poi.city.empty() && (sentence.size()-poi.city.size())>6 )     CUtil::strReplace( sentence, poi.city, " " ); 
    if ( !poi.county.empty() && (sentence.size()-poi.county.size())>6)  CUtil::strReplace( sentence, poi.county, " " ); 
    if ( !poi.town.empty() && (sentence.size()-poi.town.size())>6 )     CUtil::strReplace( sentence, poi.town, " " ); 
    */

    CUtil::strReplace( sentence, "(", "" ); 
    CUtil::strReplace( sentence, ")", "" ); 
    // CUtil::strReplace( sentence, " ", "" );
    
    vector<STWordInfo>    words; 
    size_t              num, temp_c, index;
    CUtil::strReplace( sentence, " ", "" ); 
    num = Utf8_LetterCount( sentence );
    mpSegment.cut( sentence, words );


    // 去除尾首两端高频词，仅留低频词作为核心词
    bool    bFlag;
    while( num > 4 && words.size() > 1 )
    {
       bFlag = false;


       index = words.size()-1;
       if ( words[index-1].freq < words[index].freq )
       {
           temp_c = Utf8_LetterCount( words[index].word );
           num -= temp_c;
           
           poi.name_suffix += words[index].word;
           words.erase( words.end()-1 );
           bFlag = true;
       }

       if ( (num > 4 && words.size()>1) && words[0].freq>words[1].freq )
       {
           temp_c = Utf8_LetterCount( words[0].word );
           num -= temp_c;

           poi.name_prefix += words.begin()->word;
           words.erase( words.begin() );
           bFlag = true;
       }
        
       if ( !bFlag )
           break;
    }

    poi.name_core.clear();
    num = words.size();
    for ( size_t i=0; i<num; ++i )
        poi.name_core += words[i].word;

    
    // extract location info 
    // sentence = poi.name_suffix;
    // parse_adminName( sentence, poi );
    
    // extract prefix

    return 1;
}

int POIFormat::format_telephone ( POIInfo &poi )
{
    if ( poi.telephone.empty( ) )
        return 1;

    // 转换为小写，全角转半角
    CUtil::Str2Lower( poi.telephone );
    poi.telephone = CUtil::UTF8ToDBC( poi.telephone );

    string  temp;
    int     pm[6];
    int     start(0), z; 
    
    int     siz = poi.telephone.size();
    char    *sz = (char*)poi.telephone.c_str();
    while( start!=siz )
    {
        memset( pm, 0, sizeof(pm) );
        z = pcre_exec( pcre_tel, NULL, sz+start, siz-start, 0, 0, pm, 6 );
        if ( z<=0 || pm[0] == -1 )
           break;

        if ( temp.empty() )
            temp += poi.telephone.substr( start+pm[0], pm[1]-pm[0] );
        else
            temp += ( " " + poi.telephone.substr(start+pm[0], pm[1]-pm[0]) );

        start += pm[1];
    }

    poi.telephone = temp;

    return 1;
}

int POIFormat::format_address ( POIInfo &poi )
{
    if ( poi.address.empty() )
        return 1;

    string addr = poi.address;
    
    // 转换为小写，全角转半角
    CUtil::Str2Lower( addr );
    addr = CUtil::UTF8ToDBC( addr );

    //括号规范化
    CUtil::strReplace( addr, "[", "(" ); 
    CUtil::strReplace( addr, "]", ")" ); 
    CUtil::strReplace( addr, "【", "(" ); 
    CUtil::strReplace( addr, "】", ")" ); 


    // 提取街道和门牌号
    vector<STWordInfo>  words;
    mpSegment.cut( addr, words );

    int i = words.size()-1;
    for ( ; i>-1; --i )
    {
       if ( poi.street.empty() && words[i].tag1=="ns5" )
       {
            poi.street = words[i].word;
            break;
       }
    }


    addr.clear();
    for ( ; i<words.size(); ++i )
        addr += words[i].word;
   
    int         pm[3], z;
    size_t      siz;
    string      temp;

    siz = addr.size();
    z = pcre_exec( pcre_hsnm, NULL, addr.c_str(), siz, 0, 0, pm, 3 );
    if ( z > 0 )
    {
        poi.house_number = addr.substr( pm[0], pm[1]-pm[0] );
        CUtil::strReplace( addr, poi.house_number, " " ); 
    }
    return 1;
}

int POIFormat::format( POIInfo &poi )
{
    format_name( poi );

    format_telephone( poi );

    format_address( poi );

    //poi.print();

    return 1;
}
