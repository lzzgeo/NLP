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
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

#include "CUtil.h"
#include "darts.h"
#include "Dictionary.h"
#include "DataFusion.h"
#include "POIMatch.h"

#include "POIMatch_API.h"


using namespace std;

class POIMatchHandle
{
    public:
    POIMatchHandle( ) { /*cout << "POIMatchHandle * " << endl;*/ }
    ~POIMatchHandle( ) {}

    public:
    Dictionary  dict;
    POIFormat   poi_fmt;
    POIMatch    poi_match;
};

POIMatchWrapper::POIMatchWrapper( )
{
    env_h = NULL;
}

POIMatchWrapper::~POIMatchWrapper( )
{
    //cout <<"~in POIMatchWrapper()" << endl;
    POIMatchHandle *poi_match = (POIMatchHandle *) env_h;
    delete poi_match;
    //cout <<"~out POIMatchWrapper()" << endl;
    env_h = NULL;
}

/*
void segfault_sigaction(int signal, siginfo_t *si, void *arg)
{
        printf("Caught segfault at address %p\n", si->si_addr);
        exit(0);
}
*/
bool POIMatchWrapper::init( const char *dict_dir, const char *host, int port )
{
    POIMatchHandle *h_ = new POIMatchHandle( );
    //cout << dict_dir << endl;


    // init dictionary
    if ( h_->dict.loadDict(dict_dir) < 1 )
        return false;
/*
        struct sigaction sa;
        memset(&sa, 0, sizeof(sigaction));
        sigemptyset(&sa.sa_sa_mask);
        sa.sa_sa_sigaction = segfault_sigaction;
        sa.sa_sa_flags   = SA_SIGINFO;
        sigaction(SIGSEGV, &sa, NULL);
 */
    try{
        h_->poi_fmt.init( &(h_->dict) );
        h_->poi_match.init ( host, port );
    }
    catch(std::exception &e)
    {
        std::cerr << "exception what: " << e.what() << endl;
    }


    env_h = (void *)h_; 

    return true;
}

int POIMatchWrapper::match( POIInfo &poi_src, vector<POIInfo> &rs )
{
    POIMatchHandle *h = (POIMatchHandle *) env_h;
    if ( NULL == h )
        return -1;

    //cout << "\t\t" << poi_src.address << endl << endl;

    h->poi_fmt.format( poi_src );
    return h->poi_match.match( poi_src, rs );
}
