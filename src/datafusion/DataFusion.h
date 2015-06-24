#ifndef _NI_DATAFUSION_H_
#define _NI_DATAFUSION_H_

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

#include "CUtil.h"

#include "mysql.h"
#include "sphinxclient.h"

#include "darts.h"
#include "SegmentStruct.h"
#include "Dictionary.h"
#include "MPSegment.h"
#include "FullSegment.h"
#include "FMMSegment.h"

#include "POIInfo.h"
#include "POIFormat.h"


using namespace std;


/************************************************************************/
/* 数据库连接参数                                                       */
/************************************************************************/
typedef struct  _conn_param{
    string      host;
    string      user;
    string      pwd;
    string      dbname;
    int         port;
    string      err;

}stConnParam;

class MySQLConn
{
    public:
    MySQLConn()
    {
        r_sock = NULL;
        w_sock = NULL;
        res    = NULL;
    }
    int init( const char *host, const char *user, const char *pwd, const char *dbname, int port )
    {
        r_sock = create_conn( host, user, pwd, dbname, port );    
        w_sock = create_conn( host, user, pwd, dbname, port );    
    }
    MYSQL *create_conn( const char *host, const char *user, const char *pwd, const char *dbname, int port )
    {
        MYSQL *sock = mysql_init( NULL );
    
        my_bool my_true= 1;
        mysql_options( sock, MYSQL_OPT_RECONNECT, &my_true );
    
    
        // 尝试 10 次连接，不成功则退出
        MYSQL *sock2;
        for ( int i=0; i<10; ++i )
        {
            sock2 = mysql_real_connect( sock, host, user, pwd, dbname, port, NULL, CLIENT_MULTI_STATEMENTS|CLIENT_INTERACTIVE );
            if ( sock == sock2 )
            break;
        }
        
        if ( sock != sock2 )
        {
            fprintf(stderr,"Couldn't connect to engine!\n%s\n\n",mysql_error(sock));
            perror("");
            return NULL;
        }
        
        
        
        mysql_set_character_set ( sock, "utf8" );
        ExecuteSQL( sock, "SET SESSION GROUP_CONCAT_MAX_LEN=1024000", NULL);
        ExecuteSQL( sock, "SET SESSION SQL_MODE='STRICT_ALL_TABLES'", NULL);
        ExecuteSQL( sock, "SET SQL_SAFE_UPDATES=0", NULL);
        ExecuteSQL( sock, "SET WAIT_TIMEOUT=28800", NULL);
        ExecuteSQL( sock, "SET GLOBAL INTERACTIVE_TIMEOUT=28800", NULL);
        ExecuteSQL( sock, "SET CONNECT_TIMEOUT=10000", NULL);
        ExecuteSQL( sock, "SET DELAYED_INSERT_TIMEOUT=300", NULL);
        ExecuteSQL( sock, "SET INTERACTIVE_TIMEOUT=28800", NULL);
        ExecuteSQL( sock, "SET LOCK_WAIT_TIMEOUT=31536000", NULL);
        ExecuteSQL( sock, "SET NET_READ_TIMEOUT=3000", NULL);
        ExecuteSQL( sock, "SET NET_WRITE_TIMEOUT=6000", NULL);
        ExecuteSQL( sock, "SET NAMES \"utf8\"", NULL);
        
        mysql_options( sock, MYSQL_OPT_RECONNECT, &my_true );
        
        printf( "----Database Connect success!\n" );
        
        return sock;
    }

    bool ExecuteSQL( MYSQL *sock, const char *sSQL, string *err )
    {
        if ( NULL != err )
            err->clear();
    
        if( !sSQL )
            return false;
    
        if( 0 != mysql_query(sock, sSQL) ) 
        {
            if ( NULL != err )
            {
                *err = mysql_error(sock);
                *err += "<<<<";
                *err += sSQL;
            }
    
            return false;
        }
    
        return true;
    }

    int buildResults( const char *sql )
    {
        if ( !ExecuteSQL( r_sock, sql, NULL) )
            return -1;
    
        if ( !(res=mysql_use_result(r_sock)) )
            return -1;
            
        return 1;
    }

    int getPOIs( vector<POIInfo> &rows, int num )
    {
        rows.clear();

        if ( num < 1 )
            return 1;


        int         curr_num = 0;
        MYSQL_ROW   row;
        while( row = mysql_fetch_row( res ) )
        {
            rows.resize(curr_num+1);

            POIInfo &poi = rows[curr_num];

            poi.pid         = (row[0] == NULL) ? "" : row[0];
            poi.name        = (row[1] == NULL) ? "" : row[1];
            poi.address     = (row[2] == NULL) ? "" : row[2];
            poi.telephone   = (row[3] == NULL) ? "" : row[3];

            curr_num++;
            if ( num == curr_num )
                break;
        }

        return 1;
    }

    int releaseResults( )
    {
        mysql_free_result(res);
        res = NULL;
    }

    int close( )
    {
        mysql_close( r_sock );
        mysql_close( w_sock );
    }

    public:
    MYSQL_RES    *res;     // 查询结果集，结构类型
    MYSQL        *r_sock;
    MYSQL        *w_sock;
};

#endif  // _NI_DATAFUSION_H_
