
#include "MPSegment.h"


MPSegment::MPSegment()
{
}

bool MPSegment::cut( string &sentence, vector<STWordInfo> &words )
{
    SegmentContext  segContext;

    build_DAG( sentence, segContext );

    #ifdef __DEBUG
    //print_DAG( segContext );
    #endif 
    calc_DP( segContext );

   
    #ifdef __DEBUG
    //print_DP( segContext );
    #endif 


    size_t siz = segContext.size();
    STWordInfo *p = NULL;
    for ( size_t i=0; i<siz; )
    {
        p = segContext[i].pWInfo;
        if( p==NULL )
            break;

        words.push_back( *p );
        i = segContext[i].nextpos;
        //cout << "next pos " << segContext[i].nextpos << endl;
    }

    combine( words );

    return true; 
}

bool MPSegment::build_DAG( string &sentence, SegmentContext &segContext )
{
    // 初始化每个char节点
    vector<char>    vecLen; 
    size_t          char_count;

    Utf8_LetterLen( sentence, vecLen );
    char_count  = vecLen.size();


    // 构建每个节点DAG
    size_t          i(0), j(0), k(0);
    size_t          start_pos(0), next_pos(0), num(0);
    STWordInfo      wInfo;
    STCharNodeInfo  *pNode(NULL);
    STResultPair    result_pair[20];
    const char      *pSz = sentence.c_str();
    

    segContext.clear();
    segContext.resize( char_count );
    for ( i=0; i<char_count; ++i )
    {
        //cout<< "build dag " << i << "\tstart_pos: "<< start_pos<< "\tcharlen: "<< int(vecLen[i]) << endl;
        pNode = &segContext[i];
        pNode->ch = sentence.substr( start_pos, int(vecLen[i]) );

        num = m_pDict->commonPrefixSearch( pSz+start_pos, result_pair, 20 );
        for ( j=0; j<num; ++j )
        {
            //cout << "common search: "<< i << "\t" << result_pair[j].len << "\t" << result_pair[j].val << endl;

            for ( k=i; k<char_count; ++k )
            {
                result_pair[j].len -= vecLen[k]; 
                if ( 0 == result_pair[j].len )
                    break;
            }

            m_pDict->getWordInfo( result_pair[j].val, pNode->dag[k+1] );
        }
        
    
        // single char, assign the min weight
        wInfo.clear();
        wInfo.word      = pNode->ch;
        wInfo.freq      = m_pDict->getMinFreq( );
        wInfo.logFreq   = m_pDict->getMinLogFreq( );
        pNode->dag[i+1] = wInfo;

        start_pos += vecLen[i];
    }


    return true;    
}


bool MPSegment::calc_DP( SegmentContext &segContext )
{
    //cout<< "start calc DP " << std::endl;
    
    size_t siz = segContext.size( );

    DagType::iterator   itr, itrEnd;
    STWordInfo          *pWInfo(NULL);
    uint                nextpos;
    double              val;
    for ( int i=siz-1; i>=0; --i )
    {
        itr     = segContext[i].dag.begin();
        itrEnd  = segContext[i].dag.end();
        for ( ;itr!=itrEnd; ++itr )
        {
            nextpos = itr->first;
            pWInfo  = &(itr->second);

            val = pWInfo->logFreq; 

            if( nextpos < siz-1 )
                val += segContext[nextpos].weight;
        

            if( val > segContext[i].weight )
            {
                segContext[i].pWInfo = pWInfo;
                segContext[i].weight = val;

                segContext[i].nextpos = nextpos;
            }
        }
    }

    return true;
}

bool MPSegment::print_DAG( SegmentContext &segContext )
{
    size_t siz = segContext.size();
    cout<< "共有char字符: " << siz << std::endl;
    
    DagType::iterator   itr, itrEnd;
    for ( size_t i=0; i<siz; ++i )
    {
        cout<< "DAG start char " << i << ":" << endl;

        itr     = segContext[i].dag.begin();
        itrEnd  = segContext[i].dag.end();

        for ( ; itr!=itrEnd; ++itr )
            cout << "----" << itr->first << "    "<< itr->second.word << "    " << itr->second.logFreq << endl;
    }

    return true;
}

bool MPSegment::print_DP( SegmentContext &segContext )
{
    size_t siz = segContext.size();
    cout<< " start to print DP " << siz << std::endl;

    for ( size_t i=0; i<siz; ++i )
    {
        if ( NULL != segContext[i].pWInfo )
            cout<< "start char " << i << ":    " << segContext[i].weight << "    " << segContext[i].pWInfo->word << endl;
    }

    return true;
}
