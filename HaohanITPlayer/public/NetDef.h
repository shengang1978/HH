#ifndef _REMOTE_NET_DEF_H
#define _REMOTE_NET_DEF_H

#include "winsock2.h"
#include "GUID.h"

typedef  unsigned char			U8  ;
typedef  unsigned short			U16 ;
typedef  unsigned int			U32 ;
typedef  unsigned __int64		U64 ;

#define NAME_LENGTH_DVR		80
#define NAME_LENGTH_CHANNEL 32

#define _DVRIPLen 48
#define _DVRSerialNumLen	64

#define MAX_TIMESEGMENT		4
#define MAX_DAYS 			7

#define NAME_LENGTH_NET		32 //���紫��ʹ�õ��ļ�������

//���䷽ʽ
#define TCP_TYPE 0
#define UDP_TYPE 1
#define MCAST_TYPE 2

#define DVRNameLen 80

#define _DVRIPLen  48

#define _DVRSerialNumLen 64


//�û�����
#define  VIDEO_USER_TYPE	1
//��Ƶ�û�_��Ҫ��ǰʱ��֮ǰ�Ĺ̶����ȵ���Ƶ����
#define  VIDEO_USER_NeedData_BeforeCurrentTime_TYPE		2 

//�������ͽӿڶ���
#define SYSTEM_COMMAND_BASE                       0x1000

//��½����
#define SYSTEM_LOGIN_REQUEST		              ( SYSTEM_COMMAND_BASE + 1  )
#define SYSTEM_LOGOUT_REQUEST		              ( SYSTEM_COMMAND_BASE + 6  )
#define SYSTEM_RELOGIN_REQUEST					  ( SYSTEM_COMMAND_BASE + 26  )

//������Ƶ����
#define SYSTEM_MONITOR_START_DIRECT              ( SYSTEM_COMMAND_BASE + 44  )
#define SYSTEM_MONITOR_STOP						 ( SYSTEM_COMMAND_BASE + 46  )

// ����, ������Ҫ��ǰʱ��֮ǰ�Ĺ̶����ȵ���Ƶ����
#define SYSTEM_MONITOR_START_NeedData_BeforeCurrentTime	 ( SYSTEM_COMMAND_BASE + 51 )

//�ļ����ط�
#define SYSTEM_VIDEO_OPEN_REQUEST		         ( SYSTEM_COMMAND_BASE + 73  )
#define SYSTEM_VIDEO_CLOSE_REQUEST	   	         ( SYSTEM_COMMAND_BASE + 77  )
#define SYSTEM_VIDEO_READ_REQUEST	             ( SYSTEM_COMMAND_BASE + 78  )
#define SYSTEM_VIDEO_SEEK_REQUEST                ( SYSTEM_COMMAND_BASE + 81  )

#define SYSTEM_VIDEOCHANNEL_WORKSATUTUS_REQUEST			( SYSTEM_COMMAND_BASE + 88 )
#define SYSTEM_DVR_VIDEOCHANNEL_WORKSATUTUS_REQUEST		( SYSTEM_COMMAND_BASE + 89 )
#define SYSTEM_ALL_VIDEOCHANNEL_WORKSATUTUS_REQUEST		( SYSTEM_COMMAND_BASE + 90 )

// ��ʱ��ط�¼��
#define SYSTEM_VIDEO_Playback_ByTime_REQUEST			( SYSTEM_COMMAND_BASE + 91 )

#define SYSTEM_TIMESYNCH_SET			(SYSTEM_COMMAND_BASE + 9)
#define	SYSTEM_DVR_INFO					(SYSTEM_COMMAND_BASE + 101)
#define	SYSTEM_START_RECORD_Direct		(SYSTEM_COMMAND_BASE + 108)	
#define	SYSTEM_STOP_RECORD_Direct		(SYSTEM_COMMAND_BASE + 109)	
#define SYSTEM_TIMEREC_INFO				(SYSTEM_COMMAND_BASE + 110)	


#define		REMOTE_SYSTEM_UPDATE_REQUEST			( SYSTEM_COMMAND_BASE + 46 )
#define		REMOTE_SYSTEM_UPDATE_DATA				( SYSTEM_COMMAND_BASE + 47 )
#define		REMOTE_SYSTEM_UPDATE_OVER				( SYSTEM_COMMAND_BASE + 48 )
#define		REMOTE_SYSTEM_REBOOT_DVR				( SYSTEM_COMMAND_BASE + 44 )
#define		REMOTE_SYSTEM_MAKE_IFRAME				( SYSTEM_COMMAND_BASE + 8 )





//ͨ�û�Ӧ
#define SYSTEM_REQUEST_ACCEPT                ( SYSTEM_COMMAND_BASE + 333  )
#define SYSTEM_REQUEST_REJECT                ( SYSTEM_COMMAND_BASE + 334  )



#define NET_DVR_PICCFG1			1		//ͼ�����(�ͺ�����DVR��SDK NET_DVR_PICCFG�غ������Ժ�����˸�1)
#define NET_DVR_COMPRESSCFG		2		//ѹ������
#define NET_DVR_RECORDCFG		3		//¼��ʱ�����


//�û���½��Ӧ����ԭ����
#define USER_INVALID_USERNAME		-1		
#define USER_INVALID_PASSWORD		-2
#define USER_GROUP_NOTALLOCATE		-3
#define USER_ADMINI_ONLINE          -4
#define DataBase_ERROR				-5  //  ���ݿ����, �޷���֤�û�

#pragma pack(1)

struct COMMAND_HEADER
{
	WORD CmdType	;
	WORD CmdPara	;
	UINT command  ;
	UINT length	;
	VOID	h2n() { CmdType = htonl(CmdType), command = htonl(command); length = htonl(length); CmdPara = htons(CmdPara); }
	VOID	n2h() { h2n(); }
} ; 

struct DATE_INFO
{
	WORD year  ;
	BYTE month ;
	BYTE date  ;
	
	void h2n() { year = htons(year); }
	void n2h() { h2n(); }
};

struct TIME_INFO
{
	BYTE hour   ;
	BYTE minute ;
	BYTE second ;
	BYTE reserved ;
};

struct TIME_PHASEEX
{
    TIME_INFO 	start ;
    TIME_INFO 	end ;
    BYTE        	active ;
} ;

//����: ��
struct SPoint 
{
	UINT u32_X;
	UINT u32_Y;
	
	void  h2n() {
		u32_X = htonl( u32_X );
		u32_Y = htonl( u32_Y );
	}
	void  n2h() { h2n(); }
};

//����: һ���߶�
struct SLine
{
	SPoint oSPoint1;
	SPoint oSPoint2;
	
	void  h2n() {
		oSPoint1.h2n();
		oSPoint2.h2n();
	}
	void  n2h() { h2n(); }
};

//��½�ṹ
struct NET_LOGIN_MESSAGE
{
    COMMAND_HEADER header;
    char UserName[NAME_LENGTH_NET];
    char PassWord[NAME_LENGTH_NET];
    void h2n()
    {
        header.h2n();
    }
    void n2h()
    {
        h2n();
    }
};
//��½��Ӧ
struct NET_LOGIN_RESPONSE
{
    COMMAND_HEADER header;
    UINT reason;
    UINT UserType;//�û����ͣ�ָʾ�Ƿ��ǳ����û� 0 Ϊ�����û�
    UINT CSVersion;//DVR�汾��Ϣ
    USHORT csSameNameID;//�������ʱ��ţ�Ŀ����Ϊ��������ͬһ���û�����¼���û�
    void h2n()
    {
        header.h2n();
        reason         = htonl( reason );
        CSVersion      = htonl(CSVersion);
        UserType       = htonl(UserType);
        csSameNameID   = htons(csSameNameID);
    }
    void n2h()
    {
        h2n();
    }
};

//CS���ص�¼���ļ���¼
struct SYSTEM_VIDEO_FILE
{
    SYSTEM_VIDEO_FILE()
    { memset( this, 0x00, sizeof(SYSTEM_VIDEO_FILE) ); }
    int           video;  //�ļ���ǩ
	SGUID		  dvrCode;
	int           channel;
    char	      DevName[NAME_LENGTH_NET];//�豸����
    U8            has_audio  ;
    U16           video_type ;    //0:�ֶ���1���澯��2����ʱ
    U16           video_quality   ;
	
    DWORD         filelen_low;//�ļ�����
    DWORD         filelen_hi; //�ļ�����
    U8            frame_rate;
    U8            video_format;
    U8            resolution;
    char          IP[32] ;
    short         Port;
	
    DATE_INFO     start_date ;
    TIME_INFO     start_time ;
    DATE_INFO     end_date   ;
    TIME_INFO     end_time   ;
	
    void h2n()
    {
		video = htonl( video );
		channel = htonl( channel );
		video_type = htons( video_type );
		video_quality = htons( video_quality );
		start_date.year = htons( start_date.year );
		end_date.year = htons( end_date.year );
		filelen_low = htonl( filelen_low );
		filelen_hi = htonl( filelen_hi );
		Port = htons( Port );
    }
} ;

struct FILE_OPEN_REQUEST_MSG
{
    COMMAND_HEADER header;
	SGUID		dvrID;
	UINT		channel;
    DWORD		video   ; 	
	DATE_INFO   StartDate;
	TIME_INFO   StartTime;
	DATE_INFO   EndDate;
	TIME_INFO   EndTime;

	//	 ��Ҫ���ػ�طŵ��ļ�������
	char		FileName[100];	
};       

struct FILE_OPEN_RESPONSE_MSG
{
    COMMAND_HEADER header;
    DWORD   reason  ;    
} ;      

struct FILE_SEEK_REQUEST_MSG
{
   COMMAND_HEADER header;
    DWORD   hpos    ;    
    DWORD   lpos    ;
} ;

struct FILE_SEEK_RESPONSE_MSG
{
   COMMAND_HEADER header;
} ;       

struct FILE_READ_REQUEST_MSG
{
	COMMAND_HEADER header;
	DWORD	length;	
};  

struct  FILE_READ_RESPONSE_MSG
{
	COMMAND_HEADER header;
	DWORD	Data;	
};  
 
/*	
 *	��ʱ��ط�����
 */
struct Playback_ByTime_REQUEST_MSG
{
    COMMAND_HEADER header;
	SGUID		dvrID;
	UINT		channel;
	DATE_INFO   StartDate;
	TIME_INFO   StartTime;
	DATE_INFO   EndDate;
	TIME_INFO   EndTime;

	VOID h2n() 
	{
		header.h2n();
		channel = htonl(channel);
		StartDate.h2n();
		EndDate.h2n();
	}

	VOID n2h() 
	{
		h2n();
	}
};  

// ��ʱ��طŵĻ�Ӧ��Ϣ
struct SPlayBackByTimeResponseInfo
{
	int nPlayBackLength;
	int nAudio;
	int nVideotype;
	int nQuality;
	int nFrameRate;
	int nVideoFormat;
	int nResolution;
	
	DATE_INFO   StartDate;
	TIME_INFO   StartTime;
	DATE_INFO   EndDate;
	TIME_INFO   EndTime;

	int nFileHeadSize;
	void h2n()
	{
		nPlayBackLength = htonl(nPlayBackLength);
		nFileHeadSize = htonl(nFileHeadSize);
		nAudio = htonl(nAudio);
		nVideotype = htonl(nVideotype);
		nQuality = htonl(nQuality);
		nFrameRate = htonl(nFrameRate);
		nVideoFormat = htonl(nVideoFormat);
		nResolution = htonl(nResolution);
		StartDate.h2n();
		EndDate.h2n();
	}
	void n2h()
	{
		h2n();
	}
};

/*	
 *	����: ��ʼ����, ����������Ƶͨ�����
 */
struct SStart_Monitor_ByVirtualVChnIndex
{
	COMMAND_HEADER	head;
	int channel;
	int childstream;
    int transType;	//������Э��
	
	SStart_Monitor_ByVirtualVChnIndex()
	{
		memset( this, 0x00, sizeof(SStart_Monitor_ByVirtualVChnIndex) );
	}
	
	void h2n() 
	{ 
		head.h2n(); 
		channel = htonl(channel); 
		childstream = htonl(childstream);
		transType = htonl(transType);
	}
	
	void n2h() 
	{ 
		h2n();
	}
};

typedef struct Client_Start_Monitor_Direct
{
	COMMAND_HEADER	header;
	char dvrIP[_DVRIPLen];
	int	 dvrPort;
	char userName[32];
	char pwd[32];
	int channel;
	int childstream;
	int	udp_port;//�û���UDP���ն˿�
	int dvrtype;
    int dvrsubtype;
	////DVR��MFR����
	unsigned int u32DVRMFRType;	
    int transType;	//������Э��
	unsigned int u32DVRMaxLinkNum; 
	unsigned int u32VSMaxLinkNum; 
	char sSerialNum[_DVRSerialNumLen]; 
	char sProxyIP[32];
	int nProxyPort; 
	
	Client_Start_Monitor_Direct() { memset( this, 0x00, sizeof(Client_Start_Monitor_Direct) ); }
	
	void h2n() 
	{ 
		header.h2n(); 

		dvrPort = htonl(dvrPort); 
		channel = htonl(channel); 
		childstream = htonl(childstream);
		udp_port = htonl(udp_port);
		dvrtype = htonl(dvrtype); 
		dvrsubtype = htonl(dvrsubtype);
		transType = htonl(transType);
		u32DVRMFRType = htonl(u32DVRMFRType); 
		u32DVRMaxLinkNum = htonl(u32DVRMaxLinkNum); 
		u32VSMaxLinkNum = htonl(u32VSMaxLinkNum); 
		nProxyPort = htonl(nProxyPort); 
	}
	
	void n2h() 
	{ 
		h2n();
	}
}CLIENT_START_MONITOR_DIRECT;

struct CLIENT_STOP_MONITOR
{
	COMMAND_HEADER	header;
	int				dvrID;
	int				channel;
	void h2n() 
	{ 
		header.h2n();
		dvrID = htonl(dvrID);
		channel = htonl(channel);
	}
};

struct CLIENT_START_MONITOR_RESPONSE
{
	COMMAND_HEADER	header;
	INT				reason;
	char            mcast_IP[32];
    unsigned short  mcast_port;
    int             streamheadsize;//��ͷ��С
	
	void h2n()
    {
		header.h2n();
		reason = htonl( reason );
		mcast_port = htons( mcast_port );
		streamheadsize = htonl(streamheadsize);
    }
	
    void n2h()
    {
        h2n();
    }

};

//����: VS����Ϣ, ����VS��Ϣ������ɵ�Ԫ
struct SVSChainUnit
{
	char VSIP[16];
	int  nVSTCPPort;
	int nMaxNumOfConnect; //����������� 
	
	void h2n()
    {
		nVSTCPPort = htonl( nVSTCPPort );
		nMaxNumOfConnect = htonl( nMaxNumOfConnect ); 
    }
	
    void n2h()
    {
        nVSTCPPort = ntohl( nVSTCPPort );
		nMaxNumOfConnect = ntohl( nMaxNumOfConnect ); 
    }
};

//ͼ�����
struct NET_HHVDVR_PICCFG
{
	DWORD dwSize;
	BYTE sChanName[NAME_LENGTH_CHANNEL];
	DWORD dwVideoFormat;	// ֻ������Ƶ��ʽ 1-NTSC 2-PAL
	BYTE byBrightness;  	//����,0-255
	BYTE byContrast;    	//�Աȶ�,0-255	
	BYTE bySaturation;  	//���Ͷ�,0-255 
	BYTE byHue;    			//ɫ��,0-255	
};

//ѹ������
struct NET_HHVDVR_COMPRESSION_INFO
{
	BYTE byStreamType;		//��������0-��Ƶ��,1-������
	BYTE byResolution;  	//�ֱ���0-QCIF 1-CIF, 2-2CIF, 3-4CIF
	BYTE byBitrateType;		//��������0:�����ʣ�1:������
	BYTE  byPicQuality;		//ͼ������ 0-��� 1-�κ� 2-�Ϻ� 3-һ�� 4-�ϲ� 5-��	
	DWORD dwVideoFrameRate;	//֡��
	DWORD dwImageFrameRate;	//ԭʼͼ��ɼ�֡��
};

//¼��ʱ��
struct NET_HHVDVR_RECORDSCHED
{
	//��ʼʱ��
    BYTE byStartHour;
	BYTE byStartMin;
	BYTE byStartSec;

	//����ʱ��
	BYTE byStopHour;
	BYTE byStopMin;
	BYTE byStopSec;

	BYTE byRecordType;	//¼������ 0:��ʱ¼��
	CHAR reservedData[5];
};

struct NET_HHVDVR_RECORD
{
	DWORD dwSize;
	DWORD dwRecord;  //�Ƿ�¼�� 0-�� 1-��
	NET_HHVDVR_RECORDSCHED struRecordSched[MAX_DAYS][MAX_TIMESEGMENT];
	DWORD dwRecordTime;	// ¼��ʱ�䳤�� 
};


struct DB_INFOEx
{
	char DBIP[32];
	char UserName[20];
	char Password[20];
	char sDBDriverVersion[32];
	int u32Port;
	int DBType;
		
	void n2h() {
		u32Port = htonl( u32Port );
		DBType = htonl( DBType );
	};
		
	void h2n() { n2h(); }
};
                
struct DB_INFO
{
	char	DBIP[32];
	char	UserName[20];
	char	Password[20];
};

//struct defined for base packet
//struct COMMAND_HEADER
//{     
//    unsigned short sender  ;
//	unsigned short command ;
//	void	n2h()
//	{
//		sender = ntohs(sender);
//		command = ntohs(command);
//	};
//	void	h2n()
//	{
//		sender = htons(sender);
//		command = htons(command);
//	};
//} ;


struct TIME_SYNCH_MSG
{
	COMMAND_HEADER  header;
	DATE_INFO	di;
	TIME_INFO	ti;
	void h2n()  { header.h2n(); di.year = htons(di.year);}
	void n2h()  { header.n2h(); di.year = ntohs(di.year);}
}; 

struct HEART_BEAT_MSG
{
	COMMAND_HEADER  header;
	unsigned int type;
	char sIP[32]; 
	void h2n()
	{
		header.h2n();
		type = htonl( type );
	}
	void n2h()
	{
		header.n2h();
		type = ntohl( type );
	}
};

struct LOGIN_REQUEST_MSG
{ 
    COMMAND_HEADER  header;
    unsigned int		type;	
	unsigned short		tcpport;
	char	localname[64];
	char sIP[32]; 

	LOGIN_REQUEST_MSG() {memset(this, 0x00, sizeof(LOGIN_REQUEST_MSG));}
	void n2h()
	{
		header.n2h();
		type	= ntohl(type);		
		tcpport	= ntohs(tcpport);
	};
	void h2n()
	{
		n2h();
	};
} ;
                                                                                
//����: ֱ��ֹͣ¼���udp��Ϣ
struct SRecordStop_Msg_Direct
{                 
    COMMAND_HEADER  header;

	SGUID DVRID; 
	char DVRIP[_DVRIPLen];				
	unsigned short	 DVRPort; // DVR�Ķ˿�

	unsigned int Channel;
	unsigned char	type; //¼������
	
	void n2h()
	{
		header.n2h();
		
		DVRPort = ntohs(DVRPort);

		Channel = ntohl( Channel );
	};
	void h2n()
	{
		n2h();
	};
};

//����: ֱ�ӿ�ʼ¼���udp��Ϣ
struct SRecordStart_Msg_Direct
{                 
    COMMAND_HEADER  header;

	SGUID	 DVRID;				//DVR�ı��룬���ڴ���Ψһ�洢Ŀ¼
	char DVRIP[_DVRIPLen];				
	unsigned short	 DVRPort;	//	DVR�Ķ˿�
	CHAR Password[32];			//	����		
	char UserName[32];			//	��½DVR���û���
	unsigned int	 DVRType;		//	DVR������
	unsigned int	 DVRSubType;	//	DVR��������
	unsigned int	 u32DVRMFRType;	
	unsigned int  streamType;		//  ����������

	unsigned int Channel;
	unsigned char	type;			//¼������
	unsigned int	elapse;			//¼��ʱ��

	char sProxyIP[32]; 
	int nProxyPort; 
	
	void n2h()
	{
		header.n2h();
		
		DVRPort = ntohs(DVRPort);
		DVRType = ntohl( DVRType );
		DVRSubType = ntohl(DVRSubType);
		u32DVRMFRType = ntohl(u32DVRMFRType); 
		streamType = ntohl(streamType);

		Channel = ntohl( Channel );
		elapse = ntohl(elapse);
		nProxyPort = ntohl(nProxyPort);  
	};
	void h2n()
	{
		n2h();
	};
}; 
   


//SERVER���յ�������Ϣ��
struct HHVSERVER_INFO
{
	unsigned int		VideoServerID;
	char    VideoServerName[32];//����������
	char    VideoServerIP[32];	//������IP��ַ
	unsigned short		ServerPort;		//�ṩ����˿�
	unsigned short		IntraPort;		//�ڲ�ͨѶ�˿�
	unsigned int		MaxUser;			
	unsigned int		MaxChannel;
	char	McastIP[32];
	char	Recorderpath[MAX_PATH];	
	unsigned int		Intertime;			//��λ¼���ļ�ʱ�䳤��
	unsigned char		OverLay;			//�Ƿ��Զ�����

	void n2h()
	{
		VideoServerID = ntohl(VideoServerID);
		ServerPort = ntohs(ServerPort);
		IntraPort = ntohs(IntraPort);
		MaxUser = ntohl(MaxUser);
		MaxChannel = ntohl(MaxChannel);
		Intertime = ntohl(Intertime);
	};
	void h2n()
	{
		n2h();
	};
}; 

//STORESERVER��½����
struct SS_LOGIN_RESPONSE
{
	COMMAND_HEADER			head;
	HHVSERVER_INFO		server_info;
	void n2h()
	{
		head.n2h();
		server_info.n2h();
	};
	void h2n()
	{
		n2h();
	};
};


struct DVR_INFO_MSG
{
	COMMAND_HEADER  header;
	unsigned char		bAdd; //0:ɾ��DVR, 1: ���DVR 
	SGUID	DVRID;
	
	unsigned int		AddrType;//֧�ָ���ID�Զ�����IP 
	
	char	DVRIP[_DVRIPLen];				
	unsigned short		DVRPort;		//DVR�Ķ˿�
	unsigned char		ChannelNum;		//ͨ������

	//���������Ĺ�������
	//��������10����32
	CHAR REQ_Manager_Password[32];	
	
	//�������
	//��������10����32
	CHAR REQ_Video_Password[32];			
	
	//��½DVR���û���, ��ǰ����DLH264��DVR
	char UserName_LoginDVR[32];	
	
	unsigned int DVRType;
	
	unsigned int DVRSubType;

	unsigned int u32DVRMFRType;	

	//DVR�������������
	unsigned int u32MaxNumOfConnect;

	char sProxyIP[32]; 
	int nProxyPort; 

	void n2h()
	{
		header.h2n();
		AddrType = ntohl(AddrType);
		DVRPort = ntohs(DVRPort);
		DVRType = ntohl(DVRType);
		DVRSubType = ntohl(DVRSubType);
		u32DVRMFRType = ntohl(u32DVRMFRType); 
		u32MaxNumOfConnect = ntohl(u32MaxNumOfConnect); 
		nProxyPort = ntohl(nProxyPort);
	};
	void h2n()
	{
		n2h();
	};

};

/*	
 *	����: ��ʱ¼���¼
 */
struct STIMEREC_INFO_MSG
{
	COMMAND_HEADER  header;
	unsigned char bAdd;
	SGUID TimerId;
	//SGUID DVRID;
	unsigned int ChnIndex;
    unsigned int StartTime;       
    unsigned int EndTime;       
    unsigned char DayOfWeek;               
    unsigned char Active;

	void h2n()
	{
		header.h2n();
		ChnIndex = htonl(ChnIndex);
		StartTime = htonl(StartTime); 
		EndTime = htonl(EndTime);
	};

	void n2h()
	{
		h2n();
	};
	
};


#define UPDATE_TYPE_VPK	1	//���ܷ�������
//�ļ��ϴ�����
struct REMOTE_UPDATE_BEGIN_REQUEST
{
    COMMAND_HEADER  header ;
    U16				loginID; 
	U16				uploadType;//�����ļ�����
	U16				channel;
	U32				Length;	//�����ļ�����
	REMOTE_UPDATE_BEGIN_REQUEST(){
		memset( this, 0x00, sizeof(REMOTE_UPDATE_BEGIN_REQUEST) );
	}
	void h2n(){
		header.h2n();
		loginID	= htons( loginID );
		uploadType = htons(uploadType);
		channel = htons( channel );
		Length	= htonl( Length );
	}
	void n2h(){
		h2n();
	}
};

struct REMOTE_UPDATE_BEGIN_RESPONSE
{
    COMMAND_HEADER	header ;
    U32             Reason ;
	REMOTE_UPDATE_BEGIN_RESPONSE(){
		memset( this, 0x00, sizeof(REMOTE_UPDATE_BEGIN_RESPONSE) );
	}
	void h2n(){
		header.h2n();
		Reason	= htonl( Reason );
	}
	void n2h(){
		h2n();
	}
} ;

//ʵ����������
struct REMOTE_UPDATE_IMAGEDATA_REQUEST
{
    COMMAND_HEADER  header ;
    U16				loginID; 
	U16				length;
	U8				data[1024];	//����
	REMOTE_UPDATE_IMAGEDATA_REQUEST(){
		memset( this, 0x00, sizeof(REMOTE_UPDATE_IMAGEDATA_REQUEST) );
	}
	void h2n(){
		header.h2n();
		loginID	= htons( loginID );
		length = htons(length);
	}
	void n2h(){
		h2n();
	}
} ;
typedef REMOTE_UPDATE_BEGIN_RESPONSE REMOTE_UPDATE_IMAGEDATA_RESPONSE;


//�������ؽ���
struct REMOTE_UPDATE_END_REQUEST
{
    COMMAND_HEADER  header ;
    U16				loginID; 
	U32				CheckSum;	//����У���
	REMOTE_UPDATE_END_REQUEST(){
		memset( this, 0x00, sizeof(REMOTE_UPDATE_END_REQUEST) );
	}
	void h2n(){
		header.h2n();
		loginID		= htons( loginID );
		CheckSum	= htonl( CheckSum );
	}
	void n2h(){
		h2n();
	}
} ;

struct REMOTE_UPDATE_END_RESPONSE
{
    COMMAND_HEADER  header ;
	U16				loginID;
	U32				Reason;		//
	REMOTE_UPDATE_END_RESPONSE(){
		memset( this, 0x00, sizeof(REMOTE_UPDATE_END_RESPONSE) );
	}
	void h2n(){
		header.h2n();
		loginID = htons(loginID);
		Reason	= htonl( Reason );
	}
	void n2h(){
		h2n();
	}
};

struct REMOTE_SYSTEM_NORMAL_REQUEST
{
    COMMAND_HEADER  header ;
    U16				loginID;
	REMOTE_SYSTEM_NORMAL_REQUEST(){
		memset( this, 0x00, sizeof(REMOTE_SYSTEM_NORMAL_REQUEST) );
	}
	void h2n(){
		header.h2n();
		loginID	= htons( loginID );
	}
	void n2h(){
		h2n();
	}
} ;

struct REMOTE_SYSTEM_NORMAL_RESPONSE
{
    COMMAND_HEADER	header ;
    U32             Reason ;
	REMOTE_SYSTEM_NORMAL_RESPONSE(){
		memset( this, 0x00, sizeof(REMOTE_SYSTEM_NORMAL_RESPONSE) );
	}
	void h2n(){
		header.h2n();
		Reason	= htonl( Reason );
	}
	void n2h(){
		h2n();
	}
} ;

//Dvr��������
#define RESTART_TYPE_DVR	0
#define RESTART_TYPE_VPK	1
#define RESTART_TYPE_PC		2
struct REMOTE_REBOOT_DVR_REQUEST
{
    COMMAND_HEADER  header ;
    U16				loginID;
	U16				restartType;	//0:����DVR 1:����VPK 2://��������
	U16				channel;		//VPKͨ����
	REMOTE_REBOOT_DVR_REQUEST(){
		memset( this, 0x00, sizeof(REMOTE_REBOOT_DVR_REQUEST) );
	}
	void h2n(){
		header.h2n();
		loginID	= htons( loginID );
		restartType = htons( restartType );
		channel = htons( channel );
	}
	void n2h(){
		h2n();
	}
} ;

typedef REMOTE_SYSTEM_NORMAL_RESPONSE REMOTE_REBOOT_DVR_RESPONSE;


//���ɹؼ�֡
struct REMOTE_MAKE_IFRAME_REQUEST
{
    COMMAND_HEADER  header ;
    U16				loginID; 
    U16				Channel;//Reserve
	REMOTE_MAKE_IFRAME_REQUEST(){
		memset( this, 0x00, sizeof(REMOTE_MAKE_IFRAME_REQUEST) );
	}
	void h2n(){
		header.h2n();
		loginID	= htons( loginID );
		Channel = htons( Channel );
	}
	void n2h(){
		h2n();
	}
} ;
typedef REMOTE_SYSTEM_NORMAL_RESPONSE REMOTE_MAKE_IFRAME_RESPONSE;


#pragma pack()

#endif
