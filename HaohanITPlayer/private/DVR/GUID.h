#ifndef _SGUID_080808_H

#define _SGUID_080808_H

const char  EMPTY_UNI_ID[] = "00000000-0000-0000-0000-000000000000";

/*	2008-08-08 add
 *	功能: GUID 
 */
struct SGUID
{
protected:
    char    m_idData[40];
	
public:
    // 构造
    SGUID()	
	{ 
		memset( m_idData, 0x00, sizeof(SGUID) ); 
		strncpy( m_idData, EMPTY_UNI_ID, sizeof(SGUID) );
	}
    SGUID(const char* src) 
	{ 
		memset( m_idData, 0x00, sizeof(SGUID) ); 
		strncpy( m_idData, src, sizeof(SGUID) ); 
	}
    SGUID(const SGUID& src) { strncpy( m_idData, src.m_idData, sizeof(SGUID) ); }
	
    //赋值
    SGUID& operator =(const SGUID& rhs)
    {
		if ( &rhs != this )
		{
			memset(this, 0x00, sizeof(SGUID));
			strncpy(m_idData, rhs.m_idData,sizeof(SGUID));
		}
		return *this;
    }
	//赋值
    SGUID& operator =(const char* src )
    {
		memset(this, 0x00, sizeof(SGUID));
		strncpy(m_idData, src,sizeof(SGUID));

		return *this;
    }

	void Clear( void )
	{
		strncpy( m_idData, EMPTY_UNI_ID, sizeof(SGUID) );
	}

	//比较
    bool operator >(const SGUID& rhs) const
    {
        if( strncmp(m_idData, rhs.m_idData, 40) > 0 )
            return true;
        return false;
    }
	//比较
    bool operator <(const SGUID& rhs) const
    {
        if( strncmp(m_idData, rhs.m_idData, 40) < 0 )
            return true;
        return false;
    }
	//比较
    bool operator !=(const SGUID& rhs) const
    {
        if( strcmp(m_idData, rhs.m_idData) != 0 )
            return true;
        return false;
    }
    //比较
    bool operator ==(const SGUID& rhs) const
    {
        if( strcmp(m_idData, rhs.m_idData) == 0 )
            return true;
        return false;
    }
	//比较
    bool operator ==(const char* rs) const
    {
        if( strcmp(m_idData, rs) == 0 )
            return true;
        return false;
    }

	bool Empty()
	{
		if( (strncmp(m_idData, EMPTY_UNI_ID, 36) == 0) || (strncmp(m_idData, "",36) == 0) )
            return true;
        return false;
	}

    //生成唯一编码
    bool GenerateGUID()
    {
		memset(this, 0x00, sizeof(SGUID));
        bool bret = false;
        CoInitialize(NULL);
        GUID guid;
        if(S_OK == ::CoCreateGuid(&guid))
        {
            sprintf( m_idData, "%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X"
				, guid.Data1
				, guid.Data2
				, guid.Data3
				, guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3]
				, guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]
				);
            bret = true;
        }
        CoUninitialize();
        return bret;
    }
	
    char* c_str() { return m_idData;}
    
};

#endif	//end _SGUID_080808_H