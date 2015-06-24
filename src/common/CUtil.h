#ifndef _NI_CUTIL_H_
#define _NI_CUTIL_H_

//#include  <io>
#include <sys/types.h>
#include <stdio.h>
#include <stdint.h>
#include <limits>
#include <cmath>
#include <iostream>
#include <fstream>
#include <cctype>
#include <algorithm>

#include <vector>
#include <map>
#include <set>
#include <string>
#include <string.h>


//#define __DEBUG 1


using std::vector;
using std::string;
using std::wstring;

using namespace std;


typedef wchar_t WCHAR;

typedef unsigned char           u1;
typedef unsigned short          u2;


// const double M_PI = 3.141592653;


int progress_bar(size_t current, size_t total);


inline uint16_t twocharToUint16(char high, char low)
{
    return (((uint16_t(high) & 0x00ff ) << 8) | (uint16_t(low) & 0x00ff));
}

//inline bool decode(const string& str, vector<uint16_t>& vec)
inline bool Utf8ToUint16(const string& str, vector<uint16_t>& vec)
{
    char ch1, ch2;
    if(str.empty())
    {
        return false;
    }
    vec.clear();
    size_t siz = str.size();
    for(uint i = 0;i < siz;)
    {
        if(!(str[i] & 0x80)) // 0xxxxxxx
        {
            vec.push_back(str[i]);
            i++;
        }
        else if ((unsigned char)str[i] <= 0xdf && i + 1 < siz) // 110xxxxxx
        {
            ch1 = (str[i] >> 2) & 0x07;
            ch2 = (str[i+1] & 0x3f) | ((str[i] & 0x03) << 6 );
            vec.push_back(twocharToUint16(ch1, ch2));
            i += 2;
        }
        else if((unsigned char)str[i] <= 0xef && i + 2 < siz)
        {
            ch1 = (str[i] << 4) | ((str[i+1] >> 2) & 0x0f );
            ch2 = ((str[i+1]<<6) & 0xc0) | (str[i+2] & 0x3f); 
            vec.push_back(twocharToUint16(ch1, ch2));
            i += 3;
        }
        else
        {
            return false;
        }
    }
    return true;
}

inline uint16_t FirstUtf8ToUint16( const string& str )
//inline uint16_t decode2(const string& str)
{
    char ch1, ch2;

        if(!(str[0] & 0x80)) // 0xxxxxxx
        {
            return uint16_t(str[0]);
        }
        else if ((unsigned char)str[0] <= 0xdf ) // 110xxxxxx
        {
            ch1 = (str[0] >> 2) & 0x07;
            ch2 = (str[1] & 0x3f) | ((str[0] & 0x03) << 6 );
            return twocharToUint16(ch1, ch2);
        }
        else if((unsigned char)str[0] <= 0xef )
        {
            ch1 = (str[0] << 4) | ((str[1] >> 2) & 0x0f );
            ch2 = ((str[1]<<6) & 0xc0) | (str[2] & 0x3f); 
            return twocharToUint16(ch1, ch2);
        }
        else
            return 0;
}

//inline bool encode( uint16_t *pui, size_t siz, string& res)
inline bool Uint16ToUtf8( uint16_t *pui, size_t siz, string& res)
{
    if( siz<1 )
    {
        return false;
    }

    res.clear();

    uint16_t ui;
    for( size_t i=0; i<siz; ++i )
    {
        ui = pui[i];
        if(ui <= 0x7f)
        {
            res += char(ui);
        }
        else if(ui <= 0x7ff)
        {
            res += char(((ui>>6) & 0x1f) | 0xc0);
            res += char((ui & 0x3f) | 0x80);
        }
        else
        {
            res += char(((ui >> 12) & 0x0f )| 0xe0);
            res += char(((ui>>6) & 0x3f )| 0x80 );
            res += char((ui & 0x3f) | 0x80);
        }
    }

    return true;
}

inline size_t Utf8_LetterCount( const string& str )
{
    if(str.empty())
        return 0;

    size_t letter_count = 0;
    size_t siz = str.size();
    for(size_t i= 0;i < siz;)
    {
        if(!(str[i] & 0x80)) // 0xxxxxxx
            i++;
        else if ((unsigned char)str[i] <= 0xdf && i + 1 < siz) // 110xxxxxx
            i += 2;
        else if((unsigned char)str[i] <= 0xef && i + 2 < siz)
            i += 3;
        else
            break;
        
        letter_count++;
    }

    return letter_count;
}

// 获取每个字符占用UTF8字节数
//inline bool decode_len(const string& str, vector<unsigned short> &vec)
inline bool Utf8_LetterLen( const string& str, vector<char> &vec )
{
    vec.clear();

    if(str.empty())
        return false;

    size_t siz = str.size();
    for(size_t i= 0;i < siz;)
    {
        if(!(str[i] & 0x80)) // 0xxxxxxx
        {
            vec.push_back(1);
            i++;
        }
        else if ((unsigned char)str[i] <= 0xdf && i + 1 < siz) // 110xxxxxx
        {
            vec.push_back(2);
            i += 2;
        }
        else if((unsigned char)str[i] <= 0xef && i + 2 < siz)
        {
            vec.push_back(3);
            i += 3;
        }
        else
        {
            return false;
        }
    }

    return true;
}

inline int Utf8_FirstLetterLen( const string& str )
{
    // 0xxxxxxx
    if(!(str[0] & 0x80))                        return 1;
    // 110xxxxxx
    else if ((unsigned char)str[0] <= 0xdf )    return 2;
    else if((unsigned char)str[0] <= 0xef )     return 3;
    else                                        return -1;
}

// 获取一个字的类型
// 1 digital
// 2 Arabic alphabet
// 3 chinese character
// 0 others
inline int get_lettertype( string &str )
{
    uint16_t val = FirstUtf8ToUint16(str);

    // 0~9, ０~ ９
    if ( (val>=48 && val<=57) || (val>=65296 && val<=65305) )
    {
        return 1;
    }
    
    //a~z, A~Z, ａ~ｚ, Ａ~Ｚ
    else if ( (val>=97 && val<=122) || (val>=65 && val<=90) 
            || (val>=65345 && val<=65370) || (val>=65313 && val<=65338) ) 
    {
        return 2;
    }
          
   return 0; 
}

inline bool splitStr( const string& src, vector<string>& res, const string& pattern )
{
    if( src.empty() )
        return false;

    res.clear();

    size_t start(0), end(0);
    while( start < src.size() )
    {
        end = src.find_first_of(pattern, start);
        if( string::npos == end )
        {
            res.push_back(src.substr(start));
            return true;
        }
        
        res.push_back( src.substr(start, end - start) );
        if( end == src.size() - 1 )
        {
            res.push_back("");
            break;
        }
        
        start = end + 1;
    }
    
    return true;
}


class CUtil
{
public:
    static double  minor_semi_axis;
    static double  major_semi_axis;
    static double  EARTH_RADIUS_METER;
    //double  x_pi = 3.14159265358979324 * 3000.0 / 180.0;
public:

	CUtil ();
	virtual ~CUtil();
    
    // 空间相关函数
	static double deg2rad( double deg );
	static double rad2deg( double rad );
    static double spherical_distance( double lon1, double lat1, double lon2, double lat2 );
    
    static long hex2int( string &s );

    static float strDistance( string s1, string s2 );

	static bool isNumber(string& str);

	//大小写转换
	static void Str2Lower(string& str);
	static void Str2Upper(string& str);

	static int	SplitString(const string & src, string & key, string & value);
	static bool split_string( const string& InputString, vector<string>& OutputStrings,	const string& Symbol );

	static bool strReplace(string& src, // 源串
					   const string &dst, // 要被替换的目标串
					   const string &rpl // 替代串
					   ) ;

	static bool trimright(string &s, const string &aa );
	static bool trim(string &s, const string &aa );
	
	//读文件
	static size_t ReadFile (const char *path, char* &ptext);

	//文件存在
	static bool FileExist (const char *path);

	//创建目录
	static bool Mkdir (const char *m_dir);

	//取得某个目录下的文件列表
	static bool GetFiles (const string &dir, vector<string>& files, const string ext=".*");
	static bool GetDirs (const string &path, vector<string>& dirs);
	
	static bool utf82GBK (const string &utf8src, string &strGBK);

	static string GBK2UTF8(const std::string& strGBK);
	static wstring GBK2Unicode(const std::string& strGBK);

	
    // utf8 与 uc2间转换
    static int UTF8ToUC2 ( const u1 * pBuf , u2& length);
    static int UC2ToUTF8 ( int iCode, u1 * pBuf);
    
	//全角转半角 
    static string UTF8ToDBC( string &src );
    static string GBKToDBC(const string &szStr, bool &bHaveillegalChar);

    
    static wstring UTF82Unicode( const std::string& utf8 );
	static string Unicode2UTF8( const std::wstring& ucs );
	
	static bool GetCsvData (const string& InputString, vector<string>& OutputStrings,	const string& Symbol);

	static int ReadCsvFile (const char *szfile_ame, vector<string>& csvdata, const char *spitToken);

	//罗马数字转成英文字母
	static string GetRoma(const string &str);


	static int StrCmpNocase(const string &s1, const string &s2);
};

#endif // _NI_CUTIL_H_
