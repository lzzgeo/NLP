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
#include "CUtil.h"
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

void trim_suffix( string &name )
{
    CUtil::strReplace(name, "（", "(");
    CUtil::strReplace(name, "）", ")");
    CUtil::strReplace(name, "(一)", "");
    CUtil::strReplace(name, "(二)", "");
    CUtil::strReplace(name, "(", "");
    CUtil::strReplace(name, ")", "");


    CUtil::strReplace(name, "占道", "");

    CUtil::strReplace(name, "停车场1", "");
    CUtil::strReplace(name, "停车场2", "");
    CUtil::strReplace(name, "停车场", "");

    CUtil::strReplace(name, "路面和", "");
    CUtil::strReplace(name, "路面", "");
    CUtil::strReplace(name, "地面", "");
    CUtil::strReplace(name, "地上", "");
    CUtil::strReplace(name, "地下", "");
    CUtil::strReplace(name, "立体", "");

    CUtil::strReplace(name, "停车库", "");
    CUtil::strReplace(name, "车库", "");

    CUtil::strReplace(name, "咪表停车点", "");
}

float name_distance( string name1, string name2 )
{
    vector<uint16_t>    vecName1;
    vector<uint16_t>    vecName2;

    Utf8ToUint16(name1, vecName1);
    Utf8ToUint16(name2, vecName2);

    sort( vecName2.begin(), vecName2.end() );

    int matched = 0;
    for ( size_t i=0; i<vecName1.size(); ++i )
    {
        if ( vecName2.end()!=find( vecName2.begin(), vecName2.end(), vecName1[i] ) )
            matched++;
    }

    return float(matched)/float(vecName1.size());
}

int pickup( POIInfo &src, vector<POIInfo> &res )
{
    if ( res.size() < 1 )
        return -1;


    // 去除名称相似度低的POI    
    vector<POIInfo>::iterator   itr = res.begin();
    while( itr!=res.end() )
    {
       //if ( itr->name!="停车场" && name_distance(src.name, itr->name) < 0.5 )
       if ( name_distance(src.name, itr->name) < 0.5 )
//            || ( (!src.address.empty()) && (!itr->address.empty()) && name_distance(src.address, itr->address) < 0.5 ) )
       {
           //cout << "erase " << itr->name << endl;
           itr = res.erase(itr);

       }
       else
           ++itr;
    }

    if ( res.size() < 1 )
        return -1;

    // 
    double  min(0), temp;
    int     index = 0;
    min = (src.lon-res[0].lon)*(src.lon-res[0].lon) + (src.lat-res[0].lat)*(src.lat-res[0].lat);
    for ( size_t i=1; i<res.size(); ++i )
    {
        temp = (src.lon-res[i].lon)*(src.lon-res[i].lon) + (src.lat-res[i].lat)*(src.lat-res[i].lat);
        if (temp<min)
        {
            min = temp;
            index = i;
        }
    }

    return index;
}

int parkinglot_match( POIMatchWrapper &poi_match, char *p_lots_file )
{
    POIInfo         poi_src;
    vector<string>  vec;
    vector<POIInfo> poi_rs;
    vector<string>  param_val;
    int             num;

    ifstream        dictf(p_lots_file);
    ofstream        out_f("./match_res.txt");
    string          line; 
    vector<string>  vecBuf;
    size_t          split_num=0, matched=0, poi_count=0;


    int cc=0;
    out_f << "src_id" << "\tsrc_name" << "\tsrc_address" << "\tsrc_lon" << "\tsrc_lat" 
        << "\tid" << "\tidcode" << "\tname"  << "\taddress" << "\tlon" << "\tlat" << "\n";
    while( getline(dictf, line) )
    {
        vecBuf.clear();

        str_split( (char*)line.c_str(), "\t", vecBuf );
        split_num = vecBuf.size();
        if ( split_num<4 )
            continue;


        poi_count += 1;
        poi_src.clear( );

        poi_src.geodist = 200;
        poi_src.kindgim = "410";

        poi_src.id      = vecBuf[0];
        poi_src.lon     = atof(vecBuf[3].c_str());
        poi_src.lat     = atof(vecBuf[4].c_str());


        num = poi_match.match( poi_src, poi_rs );

        poi_src.name    = vecBuf[1];
        poi_src.address = vecBuf[2];
        trim_suffix( poi_src.name );
        CUtil::strReplace(poi_src.address, "号", "");
        CUtil::strReplace(poi_src.name, "号", "");


        int pick_index = pickup( poi_src, poi_rs );

        if ( pick_index >- 1 )
        {
            out_f << vecBuf[0] << "\t" << vecBuf[1] << "\t" << vecBuf[2] << "\t" << vecBuf[3] << "\t" << vecBuf[4] << "\t" 
                 << poi_rs[pick_index].id << "\t" << poi_rs[pick_index].idcode << "\t" << poi_rs[pick_index].name  << "\t"
                 << poi_rs[pick_index].address << "\t" << poi_rs[pick_index].lon << "\t" << poi_rs[pick_index].lat << "\n";

            matched += 1;
        }
        else
        {
            out_f << "nomatch:\t" << vecBuf[0] << "\t" << vecBuf[1] << "\t" << vecBuf[2]   << "\t" << vecBuf[3] << "\t" << vecBuf[4] << "\n";
        }
    }

    cout << "count: " << poi_count << "\tmatched: " << matched << endl; 
    out_f << "count: " << poi_count << "\tmatched: " << matched << endl; 

    out_f.close();
    dictf.close();


    return 1;
}

int main(int argc, char **argv) 
{
    if (argc < 2)
    {
      std::cerr << "Usage: " << "name kindgim admincode address tel x y" << std::endl;
      return -1;
    }
    
  
    POIMatchWrapper poi_match;
    //poi_match.init ( argv[1], "192.168.15.106", 9362 );
    poi_match.init ( argv[1], "192.168.15.106", 9382 );


    parkinglot_match( poi_match, argv[2] );

    
    return 1;
}    

