#include "CUtil.h"

#ifdef WIN32
#include <io.h>
#include <Windows.h>
#include <direct.h>
#else
#include <dirent.h> 
#include <sys/types.h>
#include <sys/stat.h>
#include <iconv.h>
/*
class CodeConverter {
private:
	iconv_t cd;
public:
	CodeConverter(const char *from_charset,const char *to_charset)
	{
		cd = iconv_open(to_charset,from_charset);
		int one = 1;
		iconvctl(cd, ICONV_SET_DISCARD_ILSEQ, &one);
	}
	~CodeConverter() 
	{
		iconv_close(cd);
	}
	int convert(char *inbuf,int inlen,char *outbuf, int &outlen) 
	{
		char **pin = &inbuf;
		char **pout = &outbuf;

		memset(outbuf,0,outlen);
		return iconv(cd,pin,(size_t *)&inlen,pout,(size_t *)&outlen);
	}
};
static CodeConverter codeConvert ("utf-8", "gbk");
*/
#endif

using namespace std;


int progress_bar(size_t current, size_t total)
{
    static char bar[] = "*******************************************";
    static int scale = sizeof(bar) - 1;
    static int prev = 0;

    int cur_percentage  = static_cast<int>(100.0 * current/total);
    int bar_len         = static_cast<int>(1.0   * current*scale/total);

    if (prev != cur_percentage)
    {
        printf("Making Double Array: %3d%% |%.*s%*s| ", cur_percentage, bar_len, bar, scale - bar_len, "");
        
        if (cur_percentage == 100)  printf("\n");
        else                        printf("\r");
        
        fflush(stdout);
    }

    prev = cur_percentage;

    return 1;
}

double CUtil::minor_semi_axis = 6356.755 * 1000.0 * M_PI * 2.0/360.0;
double CUtil::major_semi_axis = 6378.113649 * 1000.0 * M_PI  * 2.0/360.0;
double CUtil::EARTH_RADIUS_METER = 6378137.0;

CUtil::CUtil()
{
    //double  x_pi = 3.14159265358979324 * 3000.0 / 180.0;
}

CUtil::~CUtil()
{

}

double CUtil::rad2deg( double rad )
{
   return 180.0 * rad/M_PI;
}

double CUtil::deg2rad( double deg )
{
   return M_PI * deg/180.0;
}

double CUtil::spherical_distance( double lon1, double lat1, double lon2, double lat2 )
{
    double flon = deg2rad(lon1);
    double flat = deg2rad(lat1);
    double tlon = deg2rad(lon2);
    double tlat = deg2rad(lat2);
    double con = sin(flat)*sin(tlat);
    con += cos(flat)*cos(tlat)*cos(flon - tlon);

    return double(acos(con)*EARTH_RADIUS_METER);
}

long CUtil::hex2int( string &s )
{
    long val  = 0;
    int  base = 0;
    int  c=s.length();
    for ( int i=0; i<c; ++i )
    {   
        if ( s.at(i)=='f' )         base = 15;
        else if ( s.at(i)=='e' )    base = 14;
        else if ( s.at(i)=='d' )    base = 13;
        else if ( s.at(i)=='c' )    base = 12;
        else if ( s.at(i)=='b' )    base = 11;
        else if ( s.at(i)=='a' )    base = 10;
        else 
        {   
            string temp = s.substr(i, 1);
            base = atoi( temp.c_str() );

        }
        val = val + base * pow(16, c-i-1);
    }

    return val;
}

float CUtil::strDistance(string s1, string s2)
{
    if ( s1.size()<1 || s2.size()<1 )
        return 0.0;


    CUtil::Str2Lower(s1);
    CUtil::Str2Lower(s2);
    
    s1 = CUtil::UTF8ToDBC(s1);
    s2 = CUtil::UTF8ToDBC(s2);

    vector<uint16_t> vec1, vec2;
    Utf8ToUint16( s1, vec1 );
    Utf8ToUint16( s2, vec2 );
    sort( vec2.begin(), vec2.end() );

    float score = 0;
    for ( int i=0; i<vec1.size(); ++i )
    {
        if (binary_search( vec2.begin(), vec2.end(), vec1[i] ))
            score += 1;
    }

    return score/float(vec1.size());
}

bool CUtil::isNumber(string& str)
{
	int nLen = str.size();
	for( int i=0; i<nLen; ++i )
		if  ( str[i]<='0' && str[i]>='9' )
			return false;

	return true;
}

void CUtil::Str2Lower(string& str)
{
    transform( str.begin(), str.end(), str.begin(), ::tolower );
}

void CUtil::Str2Upper(string& str)
{
	int nLen = str.size();
	char distance='a'-'A' ;
	for( int i=0 ; i<nLen; i++ )
		if  ( str[i]>='a' && str[i]<='z' )
			str[i] -= distance ;
}

int	CUtil::SplitString(const string & src, string & key, string & value)
{
	size_t pos = src.find("=");
	if(pos == string::npos)
		return 1;
	const char* pignore = " \t\a\n";
	string str1 = src.substr(0, pos);
	string str2 = src.substr(pos+1);
	size_t pos1 = str1.find_first_not_of(pignore);
	size_t pos2 = str1.find_last_not_of(pignore);
	if(pos1 == string::npos || pos2 == string::npos)
		return 2;
	//	if(str1[0]=='#' || (str1.size() >1 && str1[0]=='/' && str1[1]=='/'))
	//		return 4;
	key=str1.substr(pos1, pos2-pos1+1);

	size_t pos3 = str2.find_first_not_of(pignore);
	size_t pos4 = str2.find_last_not_of(pignore);
	if(pos3 == string::npos || pos4 == string::npos)
		value = "";
	else 
		value=str2.substr(pos3, pos4-pos3+1);
	return 0;
}

bool CUtil::strReplace(string& src, // 源串
					   const string &dst, // 要被替换的目标串
					   const string &rpl // 替代串
					   )  
{
	
	string::size_type pos=0;
	while( (pos=src.find(dst, pos) ) != string::npos )
	{
		src.replace(pos, dst.size(), rpl);
		pos = pos + rpl.size();
	}
	return true;
	
}
bool CUtil::trim(string &s, const string &aa )
{
	bool flag = false;
	string::size_type pos;
	pos = s.find_first_not_of (aa);
	s.erase(0, pos);
	if(pos != string::npos)
		flag = true;
	pos = s.find_last_not_of (aa);
	s.erase(pos+1);
	if(pos != string::npos)
		flag = true;
	return flag;
}
bool CUtil::split_string( const string& instr,
						 vector<string>& outstr,
						 const string& Symbol )
{
    outstr.clear();

	if( instr.empty() )
		return false;

	string::size_type pos1 = 0, pos2;
	pos2 = instr.find( Symbol, pos1 );
	while(pos2 != string::npos)
	{
		outstr.push_back(instr.substr(pos1, pos2 - pos1) );

		pos1 = pos2 + 1;
		pos2 = instr.find( Symbol, pos1 );
	}
	outstr.push_back(instr.substr(pos1));

	return true;
}

size_t CUtil::ReadFile(const char *path, char* &ptext)
{
	if(!path)
		return 0;

	FILE *fp=fopen(path, "rb");
	if(!fp)
		return 0;
	fseek(fp, 0, SEEK_END);
	size_t len = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	ptext = new char[len+1];
	fread(ptext, sizeof(char), len, fp);
	ptext[len] = '\0';
	fclose(fp);

	return len;
}

bool CUtil::FileExist(const char *path)
{
	if(!path)
		return false;

	return access (path, 0) == 0;
}

//创建一级目录，上级目录必须存在
bool CUtil::Mkdir(const char *m_dir)
{
	if(FileExist (m_dir))
		return true;
	bool flag = true;

#ifdef WIN32
	int status = ::mkdir (m_dir);
	if(status != 0)
		flag = false;
#else
	int status = ::mkdir (m_dir, 755);
	if(status < 0)
		flag = false;
	else
		chmod (m_dir, 0755);
#endif
	return flag;
}

bool CUtil::GetFiles(const string &dir, vector<string>& files, const string ext)
{

#ifdef WIN32
	//文件句柄  
	long   hFile   =   0;  
	//文件信息  
	struct _finddata_t fileFind;;  
	string path = dir + "/" + ext;
	if((hFile = _findfirst(path.c_str(), &fileFind)) != -1)  
	{  
		do  
		{  
			//如果是目录,迭代之
			//如果不是,加入列表
			if   ((fileFind.attrib & _A_SUBDIR)) 
			{  
				if   (strcmp(fileFind.name, ".")   !=   0 && strcmp(fileFind.name, "..") != 0)  
				{
					string subPath = dir + string("/") + fileFind.name;
					GetFiles(subPath, files, ext);
				}
			}
			else  
			{
				string fullPath = dir + string("/") + fileFind.name;
				files.push_back(fullPath);
			}  
		}while(_findnext(   hFile,   &fileFind   ) == 0);  

		_findclose(hFile);  
	}
#else

	DIR *dirptr = NULL;
	struct dirent *entry;

	if((dirptr = opendir(dir.c_str())) == NULL)
	{
		printf("open dir error!\n");
		return false;
	}
	else
	{
		while (entry = readdir(dirptr))
		{
			//是文件就加入
			if(entry->d_type == DT_REG)
			{
				//判断扩展名是否正确
				char *p = strrchr (entry->d_name, '.');
				if(!p)
					continue;
				if(ext == ".*" || strcmp (p, ext.c_str()) == 0)
					files.push_back(entry->d_name);
			}
		}
		closedir(dirptr);
	} 
#endif

	return true;
} 

bool CUtil::GetDirs(const string &path, vector<string>& dirs)
{
#ifdef WIN32
	//文件句柄  
	long   hFile   =   0;  
	//文件信息  
	struct _finddata_t fileinfo;  
	string spath = path + "/*";

	if((hFile = _findfirst(spath.c_str(), &fileinfo)) != -1)  
	{  
		do  
		{  
			//如果是目录,迭代之
			//如果不是,加入列表
			if   ((fileinfo.attrib & _A_SUBDIR)) 
			{
				if(strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)  
					dirs.push_back(fileinfo.name);
			}
		}while(_findnext(   hFile,   &fileinfo   ) == 0);  

		_findclose(hFile);  
	}
#else

	DIR *dirptr = NULL;
	struct dirent *entry;

	if((dirptr = opendir(path.c_str())) == NULL)
	{
		printf("open dir error!\n");
		return false;
	}
	else
	{
		while (entry = readdir(dirptr))
		{
			//是目录才加入
			if(entry->d_type == DT_DIR)
			{
				if(strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)  
					dirs.push_back(entry->d_name);
			}
		}
		closedir(dirptr);
	} 
#endif

	return true;
}

bool CUtil::GetCsvData (const string& InputString, vector<string>& OutputStrings,	const string& Symbol)
{
		if(!CUtil::split_string (InputString, OutputStrings, Symbol))
				return false;
		for(size_t i=0;i<OutputStrings.size();i++)
				CUtil::trim (OutputStrings[i], "\"");
		
		return true;
}

bool CUtil::utf82GBK (const string &utf8src, string &strGBK)
{
#ifdef WIN32
	int len = MultiByteToWideChar(CP_UTF8, 0, utf8src.c_str(), -1, NULL, 0); 
	unsigned short * wszGBK = new unsigned short[len + 1];
	memset(wszGBK, 0, len * 2 + 2);  
	MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)utf8src.c_str(), -1, (LPWSTR)wszGBK, len);

	len = WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)wszGBK, -1, NULL, 0, NULL, NULL);  
	char *szGBK = new char[len + 1];
	memset(szGBK, 0, len + 1);  
	WideCharToMultiByte(CP_ACP,0, (LPCWSTR)wszGBK, -1, szGBK, len, NULL, NULL);
	strGBK = szGBK;

	delete wszGBK;
	delete szGBK;
#else
	strGBK = "";
	int OUTLEN = utf8src.size() + 1;
	char *out = new char[OUTLEN];
	memset(out, 0, OUTLEN);
	//codeConvert.convert((char*)utf8src.c_str(), utf8src.size(), out, OUTLEN);
	strGBK = out;
	delete out;
#endif	

	return true;
}

string CUtil::GBK2UTF8(const std::string& strGBK)
{  
	string strOutUTF8 = "";  
/*	WCHAR * str1;  
	int n = MultiByteToWideChar(CP_ACP, 0, strGBK.c_str(), -1, NULL, 0);  
	str1 = new WCHAR[n];  
	MultiByteToWideChar(CP_ACP, 0, strGBK.c_str(), -1, str1, n);  
	n = WideCharToMultiByte(CP_UTF8, 0, str1, -1, NULL, 0, NULL, NULL);  
	char * str2 = new char[n];  
	WideCharToMultiByte(CP_UTF8, 0, str1, -1, str2, n, NULL, NULL);  
	strOutUTF8 = str2;  

	delete[]str1;  str1 = NULL;  
	delete[]str2;  str2 = NULL;  
*/
	return strOutUTF8;
}

wstring CUtil::GBK2Unicode(const std::string& strGBK)
{
	wstring strUnicode = L"";  
	/*
    WCHAR * str1;  
	int n = MultiByteToWideChar(CP_ACP, 0, strGBK.c_str(), -1, NULL, 0);  
	str1 = new WCHAR[n];
	MultiByteToWideChar(CP_ACP, 0, strGBK.c_str(), -1, str1, n);  
	strUnicode = str1;

	delete[]str1;  str1 = NULL;  
	*/
	return strUnicode;
}


wstring CUtil::UTF82Unicode( const std::string& utf8src )
{
/*
#ifdef WIN32
	int len = MultiByteToWideChar(CP_UTF8, 0, utf8src.c_str(), -1, NULL, 0); 
	wchar_t * wszUnicode = new wchar_t[len + 1];
	memset(wszUnicode, 0, len * 2 + 2);  
	MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)utf8src.c_str(), -1, (LPWSTR)wszUnicode, len);

	wstring wstr = wszUnicode;
	delete wszUnicode;

	return wstr;
#else
	strGBK = "";
	int OUTLEN = utf8src.size() + 1;
	char *out = new char[OUTLEN];
	memset(out, 0, OUTLEN);
	codeConvert.convert((char*)utf8src.c_str(), utf8src.size(), out, OUTLEN);
	strGBK = out;
	delete out;
#endif
*/
    return L"";
}

int CUtil::UC2ToUTF8 ( int iCode, u1 * pBuf) 
{
	if ( iCode<0x80 )
	{
		pBuf[0] = (u1)( iCode & 0x7F );
		return 1;

	} else if ( iCode<0x800 )
	{
		pBuf[0] = (u1)( ( (iCode>>6) & 0x1F ) | 0xC0 );
		pBuf[1] = (u1)( ( iCode & 0x3F ) | 0x80 );
		return 2;

	} else
	{
		pBuf[0] = (u1)( ( (iCode>>12) & 0x0F ) | 0xE0 );
		pBuf[1] = (u1)( ( (iCode>>6) & 0x3F ) | 0x80 );
		pBuf[2] = (u1)( ( iCode & 0x3F ) | 0x80 );
		return 3;
	}
}

string CUtil::UTF8ToDBC( string &src )
{
    string dest;
	u2  len, len2;
    u1  val[6]; 
    u2  iCode, i=0;

    dest.clear();
    const u1 *p = (const u1*)(src.c_str());

	while(*p != '\0')
	{
		len = 0;
		iCode = UTF8ToUC2( p, len );

		if(iCode >= 0xFF01 and iCode <= 0xFF5E)//unicode编码在全角范围内
		{
			iCode -= 65248;
		}
		else if(iCode == 0x3000)//空格，特殊处理,半角内码：0x20
		{
            iCode = 0x20;
		}

        len2 = UC2ToUTF8( iCode, val );
        for (i=0; i<len2; ++i)
            dest.push_back(val[i]); 

		p += len;
	}

    return dest;
}

int CUtil::UTF8ToUC2 ( const u1 * pBuf , u2& length)
{
    length = 0;

	u1 v = *pBuf;
	if ( !v )
		return 0;

	pBuf++;

	// check for 7-bit case
    if ( v<128 )
    {
		length = 1;
		return v;
	}

	// get number of bytes
	int iBytes = 0;
	while ( v & 0x80 )
	{
		iBytes++;
		v <<= 1;
	}

	// check for valid number of bytes
	if ( iBytes<2 || iBytes>4 )
		return -1;

	
	length = iBytes;
	int iCode = ( v>>iBytes );
	iBytes--;
	do
	{
		if ( !(*pBuf) )
			return 0; // unexpected eof

		if ( ((*pBuf) & 0xC0)!=0x80 )
			return -1; // invalid code

		iCode = ( iCode<<6 ) + ( (*pBuf) & 0x3F );
		iBytes--;
		pBuf++;
	} while ( iBytes );

	// all good
	return iCode;
}

string CUtil::Unicode2UTF8( const std::wstring& szUnicode )
{
	string strUtf8;  

#ifdef _WIN32
	/*
    int n = WideCharToMultiByte(CP_UTF8, 0, szUnicode.c_str(), -1, NULL, 0, NULL, NULL);  
	char * str2 = new char[n];  
	WideCharToMultiByte(CP_UTF8, 0, szUnicode.c_str(), -1, str2, n, NULL, NULL);  
	strUtf8 = str2;  

	delete []str2;  
	str2 = NULL;  
*/
#else

#endif

	return strUtf8;
}


string CUtil::GetRoma(const string &str)
{
	if(str == "Ⅰ")			return "I";
	else if(str == "Ⅱ")	return "II";
	else if(str == "Ⅲ")	return "III";
	else if(str == "Ⅳ")	return "IV";
	else if(str == "Ⅴ")	return "V";
	else if(str == "Ⅵ")	return "VI";
	else if(str == "Ⅶ")	return "VII";
	else if(str == "Ⅷ")	return "VIII";
	else if(str == "Ⅸ")	return "IX";
	else if(str == "Ⅹ")	return "X";
	else if(str == "Ⅺ")	return "XI";
	else if(str == "Ⅻ")	return "XII";
	else if(str == "ⅰ")	return "i";
	else if(str == "ⅱ")	return "ii";
	else if(str == "ⅲ")	return "iii";
	else if(str == "ⅳ")	return "iv";
	else if(str == "ⅴ")	return "v";
	else if(str == "ⅵ")	return "vi";
	else if(str == "ⅶ")	return "vii";
	else if(str == "ⅷ")	return "viii";
	else if(str == "ⅸ")	return "ix";
	else if(str == "ⅹ")	return "x";
	else					return "";

	return "";
}

//全角转半角 
string CUtil::GBKToDBC(const string &szStr, bool &bHaveillegalChar)
{
	string str = "";
	
	unsigned char c1,c2;

	size_t nBytesX = szStr.size();
	size_t i;
	for( i=0; i < nBytesX ;++i )
	{
		if (szStr[i] > 0 )          //判断是否为汉字
		{
			str +=szStr[i];
		}
		else
		{
			c1 = szStr[i];
			c2 = szStr[i+1];
			
			//   if(c1 < 127)    
			//   {
			//    //strlog += "[错误] \"" + szStr + "\"中有半角字符\n";
			//    //return "";
			//    AfxMessageBox("!");
			//   }   
			/*
			if((c1>169) || (c1<161 && c1>128))//汉字
			{
			int aaa = 0;
			}
			*/
			
			
			if(c1 == 163)
			{
				if((c2>=193 && c2<=218)
					|| (c2>=224 && c2<=250)
					|| (c2>=176 && c2<=185))//全角字母和数字
				{
					c2 = c2 - 128;   //!
					str += c2;
				}   
				
				else if((c2>=161 && c2<=175 && c2!=163 && c2!=164 /*&& c2!=171*/)
					|| (c2>=186 && c2<=191 && c2!=189)
					|| (c2==222 || c2==223))//有半角对应的允许的特殊字符
				{  
					c2 = c2 - 128;   //!
					str += c2;
				}
				else{
					str +=szStr[i];
					str +=szStr[i+1];
					bHaveillegalChar = true;
				}    
			}
			else if(c1 == 161 && c2 == 161)
			{
				str += " ";    //!
			}
			else if(c1 == 161 && c2 == 231)
			{
				str += "$";
			}
			else if(c1 == 161 && c2 == 171)
			{
				str += "~";
			}
			//else if((c1==162 && c2>=161 && c2<=170)
			//	|| (c1==162 && c2>=241 && c2<=252))//罗马数字
			//{
			//	string strRoma = "";    //!
			//	strRoma += c1;
			//	strRoma += c2;
			//	str += GetRoma(strRoma);
			//}
			else   //原样转出
			{
				str +=szStr[i];
				str +=szStr[i+1];
			}
			i++;
		}
	}
	return str;
}



int CUtil::ReadCsvFile (const char *szfile_ame, vector<string>& csvdata, const char *split_token)
{
	ifstream inFile(szfile_ame);
	if (!inFile)  
		return false;

	
	csvdata.clear();

	
	string			strLine;
	vector<string>	vecValue;
	while ( getline(inFile, strLine) )
	{  
		split_string(strLine, vecValue, split_token);
		
		for ( size_t i = 0; i < vecValue.size(); ++i )
		csvdata.push_back(vecValue[i]);
	}


	return -1;
}

int CUtil::StrCmpNocase(const string &s1, const string &s2)
{
	string t1(s1), t2(s2);

	Str2Lower( t1 );
	Str2Lower( t2 );

	return (t1 == t2);
}
