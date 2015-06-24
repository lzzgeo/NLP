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

#include "POIInfo.h"

using namespace std;

POIInfo::POIInfo( )
{
    clear();
}

void POIInfo::print( )
{
    cout << "pid: " << pid << "\tpoi_id: " << poi_id << "\tconfidence: " << confidence << "\tkind: " << kind << "\tkindgim: " << kindgim
        << "\tgeodist :" << geodist << "\tlon :" << lon  << "\tlat :" << lat  << endl;
        
    cout << "name: " << name 
        << "\tcore: " << name_core 
        << "\tprefix: " << name_prefix << "\tsuffix: " << name_suffix 
        << "\ttag: " << name_tag << "\talias: " << name_alias << endl;



    cout << "address: " << address <<"\tadmincode: "<< admincode <<  "\tprov: " << prov << "\tcity: " << city << "\tcounty: " << county 
        << "\ttown: " << town << "\tstreet: " << street << "\thouse_number: " << house_number << endl;

    cout << "community:" << community << "\tbuilding:" << building << "\tfloor:" << floor << "\troom:" << room << endl;

    cout << "zipcode:" << zipcode << "\tzones:" << zones << endl;
    

    cout << "telephone:" << telephone << "\tfax:" << fax << "\temail:" << email << "\twebsite:" << website << endl;
    cout << "others:" << endl;
    for ( int i=0; i<others.size(); ++i )
        cout << "----: " << others[i] << endl;
}

void POIInfo::clear( )
{
    pid.clear();
    poi_id.clear();
    confidence = 0;
    geodist = -1;
                  
    kind.clear();
    kindgim.clear();
        
    name.clear();
    name_core.clear();
    name_prefix.clear();
    name_suffix.clear();
    name_tag.clear();
    name_alias.clear();
    
    address.clear();
    admincode.clear();
    prov.clear();
    city.clear();
    county.clear();
    town.clear();
    street.clear();
    house_number.clear();

    community.clear();
    building.clear();
    floor.clear();
    room.clear();
    
    zipcode.clear();
    
    lon = -1;
    lat = -1;
        
    zones.clear();
    
    telephone.clear();
                                        
    fax.clear();
    email.clear();
    website.clear();
    
    others.clear();
}
