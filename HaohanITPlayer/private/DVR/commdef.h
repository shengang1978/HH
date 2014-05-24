#ifndef _COMMDEF_H

#define _COMMDEF_H

#define _In
#define _Out
#define _InOut
#define _Ref
#define _RefAdded
#define _Own
#define _InterFace //接口

#define _IPLen	16

typedef __int64 S64;
typedef int S32;
typedef short S16;
typedef char S8;

typedef unsigned int U32;
typedef unsigned short U16;
typedef unsigned char U8;

typedef unsigned int VPOINT;

#define SAFE_DELETE(pObj) { if( NULL != (pObj) ) { delete (pObj); (pObj) = NULL; } }
#define SAFE_DELETEBUF(pObj) { if( NULL != (pObj) ) { delete [](pObj); (pObj) = NULL; } }
#define SAFE_CLOSESOCKET(pObj) { if( INVALID_SOCKET != (pObj) ) { closesocket(pObj); (pObj) = INVALID_SOCKET; } }
#define SAFE_CLOSEHANDLE(pObj) { if( NULL != (pObj) ) { CloseHandle(pObj); (pObj) = NULL; } }

#define SAFE_DELETE_ARRAY(p)	{ if(p) { delete[] (p);     (p)=NULL;	} }
#define SAFE_RELEASE(p)			{ if(p) { (p)->Release();	(p)=NULL;	} }           
#define SAFE_CLOSE(p)			{ if(p) { CloseHandle((p));	(p)=NULL;	} }

#define ARRAY_LENGTH(a)			(sizeof(a)/sizeof(a[0]))

#define _ClassNameLen 256
class CClassType 
{
public:
	CClassType( const S8* ps8_ClassName )
	{
		memset( m_s8_ClassName, 0x00, _ClassNameLen );
		if ( NULL != ps8_ClassName )
			strncpy( m_s8_ClassName, ps8_ClassName, _ClassNameLen );
	}

	BOOL IfSame(const CClassType& rhs) const
	{
		return ( 0 == strncmp( m_s8_ClassName, rhs.m_s8_ClassName, _ClassNameLen ) )? TRUE : FALSE;
	}

	BOOL IfSame(const S8* ps8_ClassName) const
	{
		return ( 0 == strncmp( m_s8_ClassName, ps8_ClassName, _ClassNameLen ) )? TRUE : FALSE;
	}

	S8* GetName( VOID ) const 
	{
		return (S8*)m_s8_ClassName;
	}
	
private:
	S8 m_s8_ClassName[_ClassNameLen];
};

//定义一个 CClassType类型的变量， 变量名是由字符串"class"与所指定的类的类名组成
#define _Declare_ClassType(class_name) \
public: \
	static const CClassType class##class_name; \
	virtual const CClassType *GetClassType( VOID )\
	{ \
		return &class##class_name; \
	} \


//定义CClassType类型变量，变量名由"class"和指定类的类名组成，变量值为该指定类型的名字
//比如是CMyView，那么定义的就是CClassType CMyView::classCMyView="CMyView";

#define _Implement_ClassType(class_name) \
	const CClassType class_name::class##class_name = #class_name; \

#endif	//end _COMMDEF_H