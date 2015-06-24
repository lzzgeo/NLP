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
#include <stdarg.h>
#include <pcre.h>

#include "mysql.h"
#include "sphinxclient.h"

#include "darts.h"
#include "CUtil.h"
#include "SegmentStruct.h"
#include "Dictionary.h"
#include "MPSegment.h"
#include "FullSegment.h"
#include "FMMSegment.h"
#include "DataFusion.h"
#include "POIMatch.h"



using namespace std;

int test_cmd( POIFormat &poi_fmt, POIMatch &poi_match )
{
    vector<string>      vec;
    char                sentence[2048];
    POIInfo             poi;
    vector<POIInfo>     poi_r;
    string              match_res;
    string              s_sentence;


    memset( sentence, 0, sizeof(sentence) );
    while ( std::cin.getline(sentence, sizeof(sentence)) )
    {
        vec.clear();
        poi.clear();

        s_sentence = sentence;
    	if ( s_sentence.size()==0 )
    		continue;
    	
        CUtil::split_string( sentence, vec, " " );
        if ( vec.size() >0 )     poi.name = vec[0];
        if ( vec.size() > 1 )    poi.address = vec[1];
        if ( vec.size() > 2 )    poi.telephone = vec[2];

        cout << "------" << endl;
        
        poi_fmt.format( poi );

        poi_match.match( poi, poi_r );
        
        
        if (poi_r.size()==0)
        	cout << "???? no match" << endl;
        
        cout << poi_r.size() << " matched" << endl;
        for ( int i=0; i<poi_r.size(); ++i )
        	poi_r[i].print();

        memset( sentence, 0, sizeof(sentence) );
    }

    return 1;
}

int test_mysql( POIFormat &poi_fmt, POIMatch &poi_match )
{
    char                sentence[1024];
    MySQLConn           conn;
    string              match_res;
    vector<POIInfo>     pois;
    vector<POIInfo>     poi_r;
    

    conn.init("192.168.15.106", "lzz", "123456", "Web_Crawler", 3306 );
    conn.buildResults( "SELECT Hotel_ID, Name, Address, Telephone FROM CTrip_Hotel_API" );

    while ( std::cin.getline(sentence, sizeof(sentence)) )
    {
        conn.getPOIs( pois, 1 );

        cout << pois[0].pid << "====" << pois[0].name << "====" << pois[0].address << "====" << pois[0].telephone << endl;
        
        poi_fmt.format( pois[0] );
        poi_match.match( pois[0], poi_r );
        
        //poi_match.test_query( pois[0].name.c_str() );
    }


    conn.releaseResults();

    return 1;
}

int test_mysql_all( POIFormat &poi_fmt, POIMatch &poi_match )
{
    char                sentence[1024];
    MySQLConn           conn;
    string              match_res;
    vector<POIInfo>     pois;
    vector<POIInfo>     poi_r;
    int                 match_count = 0;

    conn.init("192.168.15.106", "lzz", "123456", "Web_Crawler", 3306 );
    conn.buildResults( "SELECT Hotel_ID, Name, Address, Telephone FROM CTrip_Hotel_API" );

    while ( true ) 
    {
        conn.getPOIs( pois, 1000 );
        if ( pois.size()<1 )
            break;
        
        for ( size_t i=0; i<pois.size(); ++i )
        {
            poi_fmt.format( pois[i] );
            if ( poi_match.match( pois[i], poi_r ) > 0 )
                match_count++;
        }
        
        //poi_match.test_query( pois[0].name.c_str() );
    }


    cout << "-------------------------" << endl;
    cout << "----matches count:" << match_count << "----" << endl;
    cout << "-------------------------" << endl;


    conn.releaseResults();

    return 1;
}

int main(int argc, char **argv) 
{
    if (argc < 2)
    {
      std::cerr << "Usage: " << "name address tel" << std::endl;
      return -1;
    }
    
    
    //构建Dictionary
    Dictionary  dict;
    dict.loadDict( argv[1] );


    POIFormat    poi_fmt;
    POIMatch     poi_match;

    poi_fmt.init( &dict );
    poi_match.init ( "192.168.15.106",  9312 );

    int type = 0;
    if ( argc > 2 )
        type = atoi( argv[2] );
    switch( type )
    {
        case 0: test_cmd( poi_fmt, poi_match );         break;
        case 1: test_mysql( poi_fmt, poi_match );       break;
        case 2: test_mysql_all( poi_fmt, poi_match );   break;
        default:    break;
    }

    return 1;
}    

