#ifndef _NI_DATAFUSION_H_
#define _NI_DATAFUSION_H_

#include <sys/types.h>
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
#include <signal.h>

#include "CUtil.h"
#include "sphinxclient.h"

#include "darts.h"
#include "SegmentStruct.h"
#include "Dictionary.h"
#include "MPSegment.h"
#include "FullSegment.h"
#include "FMMSegment.h"

#include "POIInfo.h"
#include "POIFormat.h"
#include "POIMatch.h"


using namespace std;

void die ( const char * tmpl, ... )
{
	va_list ap;
	va_start ( ap, tmpl );
	printf ( "FATAL: " );
	vprintf ( tmpl, ap );
	printf ( "\n" );
	va_end ( ap );
	//exit ( 1 );
}

POIMatch::POIMatch( )
{
    client = NULL;
    index = "*";
    minor_semi_axis = 6356.755*1000*M_PI*2.0/360.0;
    major_semi_axis = 6378.113649*1000*M_PI*2.0/360.0;

    signal(SIGPIPE,SIG_IGN);

    //printf("poimatch construct\n");
}

POIMatch::~POIMatch( )
{
    if ( NULL != client )
    {
        sphinx_destroy( client );
        client = NULL;
    }

    //printf("poimatch destroy\n");
}

int POIMatch::init( const char *host, int port )
{
    if ( NULL == host )
        return -1;


	client = sphinx_create( SPH_TRUE );
	if ( !client )
        return -1;


    cout << "host:\t"<< host << "\t\tport:\t" << port << endl;
	sphinx_set_server ( client, host,  port );

	sphinx_set_match_mode ( client, SPH_MATCH_EXTENDED2 );
    sphinx_set_limits( client, 0, 500, 1000, 50000 ); 
    //sphinx_set_sort_mode ( client, SPH_SORT_RELEVANCE, NULL );
    sphinx_set_sort_mode( client, SPH_SORT_EXPR, "ln(@weight + 10) / (ln(@weight + 10) + 10) * 0.59 + 100 / (100 + name_wc) * 0.4 + 100/(100 + name_len) * 0.01" );

	const char      *field_names[8];
	int             field_weights[8];
    vector<string>  vecName;
    vecName.push_back("name");
    vecName.push_back("address");
    vecName.push_back("telephone");
    vecName.push_back("kindidx");
    vecName.push_back("admincode");
    vecName.push_back("vadmincode");
    vecName.push_back("aliasnamech");
	field_names[0] = vecName[0].c_str();
	field_names[1] = vecName[1].c_str();
	field_names[2] = vecName[2].c_str();
	field_names[3] = vecName[3].c_str();
	field_names[4] = vecName[4].c_str();
	field_names[5] = vecName[5].c_str();
	field_names[6] = vecName[6].c_str();
	field_weights[0] = 1000;
	field_weights[1] = 200;
	field_weights[2] = 1000;
	field_weights[3] = 1000;
	field_weights[4] = 1000;
	field_weights[5] = 1000;
	field_weights[6] = 1000;
    sphinx_set_field_weights( client, 7, field_names, field_weights );

	//sphinx_set_select ( client, "pid, poi_id, flag, name, aliasch, admincode, vadmincode, kind, zipcode, address, telephone, geo_display, x, y" );
    //sphinx_set_select ( client, "admincode");//, flag, name, aliasch, admincode, vadmincode");//, kind, zipcode, address, telephone, geo_display, x, y" );



    return 1;
}

int POIMatch::prepare_query_loc( double lon, double lat, double geodist )
{
    if ( lon < 10 || lat < 10 || geodist <= 0 )
        return -1;


    double xdelta, ydelta;
    calcGeoDelta( geodist, lat, xdelta, ydelta );
#ifdef __DEBUG
    //cout<< "lon delta" << xdelta << endl;
    //cout<< "lat delta" << ydelta << endl;
#endif

    sphinx_add_filter_float_range( client, "x", CUtil::deg2rad(lon-xdelta), CUtil::deg2rad(lon+xdelta), SPH_FALSE ); 
    sphinx_add_filter_float_range( client, "y", CUtil::deg2rad(lat-ydelta), CUtil::deg2rad(lat+ydelta), SPH_FALSE ); 
#ifdef __DEBUG
    //cout<< "x min:" << CUtil::deg2rad(poi.lon-xdelta) << " max :" << CUtil::deg2rad(poi.lon+xdelta) << endl;
    //cout<< "y min:" << CUtil::deg2rad(poi.lat-ydelta) << " max :" << CUtil::deg2rad(poi.lat+ydelta) << endl;
#endif

    return 1;
}

int POIMatch::prepare_query_kind( string kind )
{
    if ( kind.empty() )
        return 1;
   
    bool b;
    string temp = kind;
    CUtil::Str2Lower(temp);
    temp = CUtil::UTF8ToDBC(temp);

    string max_kind(temp), min_kind(temp);
    for ( int i=0; i<4-temp.length(); ++i )
    {
        min_kind += '0'; 
        max_kind += 'f'; 
    }

    //cout << "max kind: " << max_kind << "\t\tmin kind: " << min_kind << endl;

    sphinx_add_filter_range( client, "kindidx", CUtil::hex2int(min_kind), CUtil::hex2int(max_kind), SPH_FALSE ); 
    

    return 1;
}

int POIMatch::prepare_query_admincode( POIInfo &poi )
{
    if ( poi.admincode.length()<2 )
        return -1;

    string          temp;
    sphinx_int64_t  min, max;


    temp = poi.admincode;
    if ( temp.size() > 6 ) temp = temp.substr( 0, 6 );

    if ( temp.size()==6 )
    { 
        min = max = atoi( temp.c_str() );
    }
    else
    {
        max = min = atoi(temp.c_str()) * pow( 10, 6-temp.size());
        max += (pow(10, 6-temp.size())-1);
    }
   

    sphinx_add_filter_range( client, "admincode", min, max, SPH_FALSE ); 
    //sphinx_add_filter_range( client, "vadmincode", min, max, SPH_FALSE ); 


    return 1;
}

sphinx_result *POIMatch::query_POI_common( POIInfo &poi, int &array_num )
{
    string    query, temp;

    vector<string>  kinds;
    CUtil::split_string( poi.kind, kinds, "|");
    if ( kinds.size()<1 )
        kinds.push_back("");

    array_num = kinds.size();

    for ( int i=0; i<kinds.size(); ++i )
    {
        sphinx_reset_filters( client );
        int loc_err = prepare_query_loc( poi.lon, poi.lat, poi.geodist );
        int adm_err = prepare_query_admincode( poi );
        prepare_query_kind( kinds[i] );
    
        // query admin + name
        if ( loc_err < 0 )
            query = poi.city + " " + poi.county + " " + poi.name_core;
        
        int re_i = sphinx_add_query( client, query.c_str(), index.c_str(), NULL );
       
        //cout << "===============\tloc_err:" << loc_err << "\tadmin_err: " << adm_err << "\tquery: " << query << endl;
    }


	sphinx_result *res = sphinx_run_queries ( client );
    if ( NULL == res )
    { 
        //cout << " ===query error----" <<  sphinx_error(client) << endl;
        //cout << " ===query warning----" <<  sphinx_warning(client) << endl;
        return NULL; 
    }
    else
    {
        //printf("----------%d\t%d\t%d\t%d \n", res->num_matches, res->total, res->total_found, res->num_fields );
        //print_res(res);
        if ( res->num_matches < 1 || res->total < 1 || res->total_found < 1 || res->num_fields < 1
                || NULL == res->attr_names )
            return NULL;
        else
            return res;
    }
}

int POIMatch::filter_POI( POIInfo &poi, vector<POIInfo> &pois )
{
    vector<string>  vecTel, vecTel2;
    float           score, score_name, score_name2, score_tel, score_loc, score_addr;
    string          temp, tel, addr1, addr2;
    bool            bHas;
    double          dist;


    for ( int i=0; i<pois.size(); ++i )
    {
        score = score_name = score_name2 = score_tel = score_loc = score_addr = 0.0;

        // name
        score_name  = CUtil::strDistance( poi.name_core, pois[i].name );
        score_name2 = CUtil::strDistance( poi.name_core, pois[i].name_alias );
        score_name  = (score_name<score_name2) ? score_name2 : score_name;

        
        // geo distance
        if ( poi.lon > 0 && poi.lat > 0 )
        {
            dist = CUtil::spherical_distance(poi.lon, poi.lat, pois[i].lon, pois[i].lat);
            pois[i].geodist = dist;

            if ( poi.geodist > 0 )
                score_loc = 1.0-dist/poi.geodist;
        }

        
        // telephone
        if ( poi.telephone.length()>6 && pois[i].telephone.length()>6 )
        {
            tel = pois[i].telephone;
            CUtil::Str2Lower(tel);
            tel = CUtil::UTF8ToDBC(tel);
               
            CUtil::split_string( poi.telephone, vecTel, " " );
            for ( int j=0; j<vecTel.size(); ++j )
            {
               if ( vecTel[j].length()<7 ) 
                   continue;
                
               temp = vecTel[j].substr( vecTel[j].length()-7, 7);
               if ( tel.find(temp) != tel.npos )
               {
                   score_tel = 1.0;
                   break;
               }
            }
        }


        // address
        addr1 = poi.address;
        addr2 = pois[i].address;
        CUtil::Str2Lower( addr1 );
        addr1 = CUtil::UTF8ToDBC( addr1 );
        CUtil::strReplace(addr1, "号", "");
        CUtil::strReplace(addr1, "路", "");
        CUtil::strReplace(addr1, "街", "");
        score_addr = CUtil::strDistance( addr1, addr2 );



        // calc score
        if ( score_tel>0 )     
        {
            if ( score_addr > 0.0 )
                score = 80.0 + score_name*10.0 + score_addr*10.0;
            else
                score = 80.0 + score_name*15.0;
        }
        else if ( pois[i].geodist < 1000 )
        {
            score = score_name*80.0 + score_addr*10.0 + score_loc*10.0;
        }
        else if ( !addr1.empty() && !addr2.empty() )
        {
            score = score_name*50.0 + score_addr*40.0 + score_loc*10;
        }
        else
        {
            score = score_name*80.0 + score_loc*20.0;
        }


        //if ( score_name > 0.8 )
            //cout << "======\t" << poi.address << "\t===="<< addr1 << "\t==== " << addr2 << endl; 
            //cout << pois[i].name << "\tscore:" << score << "\t---name:"<< score_name << "\t----addr:" << score_addr << "\t----loc:" << score_loc << "\t----tel:" << score_tel << endl;

        pois[i].confidence = score;
    }


    //remove POI with the lower score(less than 60)
    vector<POIInfo>::iterator   itr = pois.begin(); 
    while ( itr!=pois.end() )
    {
        if ( itr->confidence < 70.0 )
            itr = pois.erase(itr);
        else
            ++itr;
    }
    
    return pois.size();
}

int POIMatch::match( POIInfo &poi, vector<POIInfo> &pois )
{
    pois.clear();


    int confidence = 0;
    sphinx_result *res = NULL;
    int array_num = 0;
    res = query_POI_common( poi, array_num );
    if ( NULL==res ) 
    {
        //cout << "no matches have been returned!" << endl;
        return 0;
    }
   

    // fill return value
	int             i, j, k, mva_len;
    int             pid_idx(-1), poi_id_idx(-1), flag_idx(-1), kind_idx(-1), name_idx(-1), aliasch_idx(-1), address_idx(-1), telephone_idx(-1), 
                    zipcode_idx(-1), admincode_idx(-1), vadmincode_idx(-1), geo_display_idx(-1), x_idx(-1), y_idx(-1);
	unsigned int    *mva;
    string          val_;
    char     buf[512];


    //cout << "================ 1 match\t\t num_attrs: " << res->num_attrs << "\t" << res->attr_names << endl;

    //cout << " ===query error------" <<  sphinx_error(client) << endl;
    //cout << " ===query warning----" <<  sphinx_warning(client) << endl;
    // 标定字段对应关系
    for ( int i=0; i<res->num_attrs; ++i )
    {
        //cout << res->attr_names[i] << "\t";
        if ( 0==strcmp(res->attr_names[i], "oid") )                 pid_idx         = i;
        else if ( 0==strcmp(res->attr_names[i], "src_id") )         poi_id_idx      = i;
        else if ( 0==strcmp(res->attr_names[i], "src_type") )       flag_idx        = i;
        else if ( 0==strcmp(res->attr_names[i], "kind_code") )      kind_idx        = i;
        else if ( 0==strcmp(res->attr_names[i], "name") )           name_idx        = i;
        else if ( 0==strcmp(res->attr_names[i], "aliasname") )      aliasch_idx     = i;
        else if ( 0==strcmp(res->attr_names[i], "address") )        address_idx     = i;
        else if ( 0==strcmp(res->attr_names[i], "telephone") )      telephone_idx   = i;
        else if ( 0==strcmp(res->attr_names[i], "postcode") )       zipcode_idx     = i;
        else if ( 0==strcmp(res->attr_names[i], "admincode") )      admincode_idx   = i;
        else if ( 0==strcmp(res->attr_names[i], "vadmincode") )     vadmincode_idx  = i;
        else if ( 0==strcmp(res->attr_names[i], "geo_display") )    geo_display_idx = i;
        else if ( 0==strcmp(res->attr_names[i], "x") )              x_idx           = i;
        else if ( 0==strcmp(res->attr_names[i], "y") )              y_idx           = i;
    }

    for ( int r_i=0; r_i<array_num; ++r_i )
    {
        for ( i=0; i<res->num_matches; ++i )
        {
            POIInfo poi_r;
    
    		for ( j=0; j<res->num_attrs; ++j )
    		{
    			switch ( res->attr_types[j] )
    			{
    			case SPH_ATTR_MULTI64:
    			case SPH_ATTR_MULTI:
    				mva = sphinx_get_mva ( res, i, j );
    				mva_len = *mva++;
    
    				val_ = "(";
    				for ( k=0; k<mva_len; k++ )
                    {
    					sprintf ( buf, k ? ",%u" : "%u", ( res->attr_types[j]==SPH_ATTR_MULTI ? mva[k] : sphinx_get_mva64_value ( mva, k ) ) );
                        val_ += buf;
                    }
    				val_ +=  ")";
    				break;
    
    			case SPH_ATTR_FLOAT:
                    sprintf ( buf, "%f", sphinx_get_float ( res, i, j ) );
                    val_ = buf;
                    break;
    			case SPH_ATTR_STRING:	
                    val_ = sphinx_get_string ( res, i, j );
                    break;
    			case SPH_ATTR_BIGINT:	
    				sprintf ( buf, "%ld", sphinx_get_int ( res, i, j ) );
                    val_ = buf;
                    break;                    
    			default:
                    sprintf ( buf, "%u", sphinx_get_int ( res, i, j ) );
                    val_ = buf;
                    break;
    			}
    			
    			//cout <<res->attr_names[j] << "\t\t"<< val_ << endl;
    
                if (j == pid_idx)            poi_r.pid           = val_;
                else if (j == poi_id_idx)    poi_r.poi_id        = val_;
                else if (j == name_idx)      poi_r.name          = val_;
                else if (j == aliasch_idx)   poi_r.name_alias    = val_;
                else if (j == address_idx)   poi_r.address       = val_;
                else if (j == zipcode_idx)   poi_r.zipcode       = val_;
                else if (j == telephone_idx) poi_r.telephone     = val_;
                else if (j == kind_idx)      poi_r.kind          = val_;
                else if (j == admincode_idx) poi_r.admincode     = val_;
                //vadmincode_idx: poi_r.vadmincode = val_;
                else if (j == x_idx) poi_r.lon   = CUtil::rad2deg(atof(val_.c_str()));
                else if (j == y_idx) poi_r.lat   = CUtil::rad2deg(atof(val_.c_str()));
            }
    
        //    poi_r.confidence = confidence;
            pois.push_back( poi_r );
        }

        res++;
    }

    return filter_POI(poi, pois); 
}

int POIMatch::print_res( sphinx_result *res )
{
	int             i, j, k, mva_len;
	unsigned int    *mva;

	if ( !res )
	{
		g_failed += ( res==NULL );

		if ( !g_smoke )
			die ( "query failed: %s", sphinx_error(client) );
	}
   
	printf ( "query status:\t%d \n", res->status );

	if ( g_smoke )
		printf ( "retrieved %d of %d matches.\n", res->total, res->total_found );
	else
		printf ( "retrieved %d of %d matches in %d.%03d sec.\n", res->total, res->total_found, res->time_msec/1000, res->time_msec%1000 );
     
	printf ( "Query stats:\n" );
	for ( i=0; i<res->num_words; i++ )
		printf ( "\t'%s' found %d times in %d documents\n",
		res->words[i].word, res->words[i].hits, res->words[i].docs );
    
	printf ( "Matches:\n" );
	for ( i=0; i<res->num_matches; i++ )
	{
		printf ( "%d. doc_id=%d, weight=%d", 1+i, (int)sphinx_get_id ( res, i ), sphinx_get_weight ( res, i ) );

		for ( j=0; j<res->num_attrs; j++ )
		{
			printf ( ", %s=", res->attr_names[j] );
			switch ( res->attr_types[j] )
			{
			case SPH_ATTR_MULTI64:
			case SPH_ATTR_MULTI:
				mva = sphinx_get_mva ( res, i, j );
				mva_len = *mva++;
				printf ( "(" );
				for ( k=0; k<mva_len; k++ )
					printf ( k ? ",%u" : "%u", ( res->attr_types[j]==SPH_ATTR_MULTI ? mva[k] : (unsigned int)sphinx_get_mva64_value ( mva, k ) ) );
				printf ( ")" );
				break;

			case SPH_ATTR_FLOAT:	printf ( "%f", sphinx_get_float ( res, i, j ) ); break;
			case SPH_ATTR_STRING:	printf ( "%s", sphinx_get_string ( res, i, j ) ); break;
			default:				printf ( "%u", (unsigned int)sphinx_get_int ( res, i, j ) ); break;
			}
		}

		printf ( "\n" );
	}

	printf ( "\n" );

    return 1;
}

int POIMatch::close()
{
    if ( NULL != client )
    {
        sphinx_destroy( client );
        client = NULL;
    }
    
    return 1;
}

int POIMatch::calcGeoDelta( double geodist, double lat, double &lon_delta, double &lat_delta )
{
    lon_delta = geodist/abs(cos(CUtil::deg2rad(lat))*minor_semi_axis), 
    lat_delta = geodist/major_semi_axis;
}

#endif  // _NI_DATAFUSION_H_
