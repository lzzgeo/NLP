
#include <sys/types.h>
#include <string.h>
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

#include "POIInfo.h"
#include "POIMatch_API.h"



using namespace std;


void str_split( char *src, const char *delim, vector<string> &vec )
{
    vec.clear();

    char *token;
    while ( token = strsep(&src, delim) )
    {
        vec.push_back( token );
    }

    return;
}

int test_text( POIMatchWrapper &poi_match )
{
    vector<string>  vec;
    char            sentence[4096];
    POIInfo         poi_src;
    vector<POIInfo> poi_rs;
    int             num(0);
    string          line;
    vector<string>  param_val;


    memset( sentence, 0, sizeof(sentence) );
    ifstream        dictf("./param_poimatch.txt");
    while ( getline(dictf, line) )
    //while ( std::cin.getline(sentence, sizeof(sentence)) )
    {
        poi_src.clear( );

        str_split( (char*) line.c_str(), ",", vec );
        
        if ( vec.size() == 2 )
        {
            poi_src.name      = vec[0]; 
            poi_src.address   = vec[1];
        }


        num = poi_match.match( poi_src, poi_rs );
        poi_src.print();
/*        cout << "\n\n" << endl;
        for ( size_t i=0; i<poi_rs.size(); ++i )
        {
            cout << "\n-------------------------" << endl;
            poi_rs[i].print();
        }
*/
        memset( sentence, 0, sizeof(sentence) );

        cout << "\n\n\n\n\n"<< endl;
    }

    return 1;
}

int test_cmd( POIMatchWrapper &poi_match )
{
    vector<string>  vec;
    char            sentence[4096];
    POIInfo         poi_src;
    vector<POIInfo> poi_rs;
    int             num;
    vector<string>  param_val;


    memset( sentence, 0, sizeof(sentence) );
    while ( std::cin.getline(sentence, sizeof(sentence)) )
    {
        poi_src.clear( );

        str_split( &sentence[0], " ", vec );
        
        for ( size_t i=0; i<vec.size(); ++i )
        {
            str_split( (char*)vec[i].c_str(), "=", param_val );
            if ( param_val.size()<2 )
                continue;


            if ( param_val[0] == "name" )           poi_src.name      = param_val[1];
            else if ( param_val[0] == "kindgim" )   poi_src.kindgim   = param_val[1];
            else if ( param_val[0] == "kind" )      poi_src.kind      = param_val[1];
            else if ( param_val[0] == "admincode" ) poi_src.admincode = param_val[1];
            else if ( param_val[0] == "address" )   poi_src.address   = param_val[1];
            else if ( param_val[0] == "telephone" ) poi_src.telephone = param_val[1];
            else if ( param_val[0] == "lon" )       poi_src.lon       = atof(param_val[1].c_str());
            else if ( param_val[0] == "lat" )       poi_src.lat       = atof(param_val[1].c_str());
            else if ( param_val[0] == "dist" )      poi_src.geodist   = atof(param_val[1].c_str());
            
            else if ( param_val[0] == "prov" )      poi_src.prov      = param_val[1].c_str();
            else if ( param_val[0] == "city" )      poi_src.city      = param_val[1].c_str();
            else if ( param_val[0] == "county" )    poi_src.county    = param_val[1].c_str();
        }


        num = poi_match.match( poi_src, poi_rs );
        poi_src.print();
        cout << "\n\n" << endl;
        for ( size_t i=0; i<poi_rs.size(); ++i )
        {
            cout << "\n-------------------------" << endl;
            poi_rs[i].print();
        }

        memset( sentence, 0, sizeof(sentence) );

        cout << "\n\n\n\n\n\n"<< endl;
    }

    return 1;
}

int main(int argc, char **argv) 
{
    if (argc < 2)
    {
      std::cerr << "Usage: " << "name kind admincode address tel x y" << std::endl;
      return -1;
    }
    
  
    POIMatchWrapper poi_match;
    poi_match.init ( argv[1], "192.168.15.106", 9312 );


    //test_cmd( poi_match );
    test_text( poi_match );

    
    return 1;
}    

