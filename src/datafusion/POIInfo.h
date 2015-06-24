#ifndef _NI_POIINFO_H_
#define _NI_POIINFO_H_

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


using namespace std;


// POI infos
class POIInfo
{
    public:
        POIInfo();


    public:
    string              pid;
    string              poi_id;
    int                 confidence; // 匹配结果的可信度
    double              geodist;    // 匹配时使用的距离范围(unit: meter)。 为零时，不使用距离范围限制


    string              kind;
    string              kindgim;
    
    // name info
    string              name;       // 原始完整字符串
    string              name_core;  // 匹配时主要使用, 用于区分于其他POI的核心词汇。 例如: 品牌词，无品牌词时可能为地名词等
    string              name_prefix;// 多为地址等
    string              name_suffix;// 可能为类别词
    string              name_tag;   // 标定商圈、分店、特色等词汇

    string              name_alias; // 是否需要？？？？


    // location info
    string              address; // 原始完整字符串

    string              admincode;
    string              prov;
    string              city;
    string              county;
    string              town;
    string              street;
    string              house_number;

    string              community;
    string              building;
    string              floor;
    string              room;

    string              zipcode;

    double              lon;
    double              lat;
    
    string              zones; // 其他商圈等位置标定词


    // contact info
    string              telephone;  // 存在多个时，使用 空格 进行分割
                                    // 区号、分机，使用 - 分割
    string              fax;
    string              email;      // 多个时，使用 | 进行分割
    string              website;    // 多个时，使用 | 进行分割
    

    // other infos
    vector<string>      others;

    void print( );

    void clear( );
};

#endif  // _NI_POIINFO_H_
