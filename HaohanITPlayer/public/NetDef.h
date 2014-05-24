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

#define NAME_LENGTH_NET		32 //网络传输使用的文件名长度

//传输方式
#define TCP_TYPE 0
#define UDP_TYPE 1
#define MCAST_TYPE 2

#define DVRNameLen 80

#define _DVRIPLen  48

#define _DVRSerialNumLen 64


//用户类型
#define  VIDEO_USER_TYPE	1
//视频用户_需要当前时刻之前的固定长度的视频数据
#define  VIDEO_USER_NeedData_BeforeCurrentTime_TYPE		2 

//命令类型接口定义
#define SYSTEM_COMMAND_BASE                       0x1000

//登陆请求
#define SYSTEM_LOGIN_REQUEST		              ( SYSTEM_COMMAND_BASE + 1  )
#define SYSTEM_LOGOUT_REQUEST		              ( SYSTEM_COMMAND_BASE + 6  )
#define SYSTEM_RELOGIN_REQUEST					  ( SYSTEM_COMMAND_BASE + 26  )

//请求视频监视
#define SYSTEM_MONITOR_START_DIRECT              ( SYSTEM_COMMAND_BASE + 44  )
#define SYSTEM_MONITOR_STOP						 ( SYSTEM_COMMAND_BASE + 46  )

// 监视, 并且需要当前时刻之前的固定长度的视频数据
#define SYSTEM_MONITOR_START_NeedData_BeforeCurrentTime	 ( SYSTEM_COMMAND_BASE + 51 )

//文件名回放
#define SYSTEM_VIDEO_OPEN_REQUEST		         ( SYSTEM_COMMAND_BASE + 73  )
#define SYSTEM_VIDEO_CLOSE_REQUEST	   	         ( SYSTEM_COMMAND_BASE + 77  )
#define SYSTEM_VIDEO_READ_REQUEST	             ( SYSTEM_COMMAND_BASE + 78  )
#define SYSTEM_VIDEO_SEEK_REQUEST                ( SYSTEM_COMMAND_BASE + 81  )

#define SYSTEM_VIDEOCHANNEL_WORKSATUTUS_REQUEST			( SYSTEM_COMMAND_BASE + 88 )
#define SYSTEM_DVR_VIDEOCHANNEL_WORKSATUTUS_REQUEST		( SYSTEM_COMMAND_BASE + 89 )
#define SYSTEM_ALL_VIDEOCHANNEL_WORKSATUTUS_REQUEST		( SYSTEM_COMMAND_BASE + 90 )

// 按时间回放录像
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





//通用回应
#define SYSTEM_REQUEST_ACCEPT                ( SYSTEM_COMMAND_BASE + 333  )
#define SYSTEM_REQUEST_REJECT                ( SYSTEM_COMMAND_BASE + 334  )



#define NET_DVR_PICCFG1			1		//图象参数(和海康的DVR的SDK NET_DVR_PICCFG重合了所以后面加了个1)
#define NET_DVR_COMPRESSCFG		2		//压缩参数
#define NET_DVR_RECORDCFG		3		//录像时间参数


//用户登陆回应错误原因定义
#define USER_INVALID_USERNAME		-1		
#define USER_INVALID_PASSWORD		-2
#define USER_GROUP_NOTALLOCATE		-3
#define USER_ADMINI_ONLINE          -4
#define DataBase_ERROR				-5  //  数据库出错, 无法验证用户

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

//功能: 点
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

//功能: 一条线段
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

//登陆结构
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
//登陆回应
struct NET_LOGIN_RESPONSE
{
    COMMAND_HEADER header;
    UINT reason;
    UINT UserType;//用户类型，指示是否是超级用户 0 为超级用户
    UINT CSVersion;//DVR版本信息
    USHORT csSameNameID;//这个是临时编号，目的是为了区分用同一个用户名登录的用户
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

//CS返回的录像文件记录
struct SYSTEM_VIDEO_FILE
{
    SYSTEM_VIDEO_FILE()
    { memset( this, 0x00, sizeof(SYSTEM_VIDEO_FILE) ); }
    int           video;  //文件标签
	SGUID		  dvrCode;
	int           channel;
    char	      DevName[NAME_LENGTH_NET];//设备名字
    U8            has_audio  ;
    U16           video_type ;    //0:手动；1：告警；2：定时
    U16           video_quality   ;
	
    DWORD         filelen_low;//文件长度
    DWORD         filelen_hi; //文件长度
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

	//	 需要下载或回放的文件的名字
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
 *	按时间回放请求
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

// 按时间回放的回应信息
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
 *	功能: 开始监视, 依据虚拟视频通道编号
 */
struct SStart_Monitor_ByVirtualVChnIndex
{
	COMMAND_HEADER	head;
	int channel;
	int childstream;
    int transType;	//流传输协议
	
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
	int	udp_port;//用户的UDP接收端口
	int dvrtype;
    int dvrsubtype;
	////DVR的MFR类型
	unsigned int u32DVRMFRType;	
    int transType;	//流传输协议
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
    int             streamheadsize;//流头大小
	
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

//功能: VS的信息, 其是VS信息链的组成单元
struct SVSChainUnit
{
	char VSIP[16];
	int  nVSTCPPort;
	int nMaxNumOfConnect; //最大连接限制 
	
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

//图像参数
struct NET_HHVDVR_PICCFG
{
	DWORD dwSize;
	BYTE sChanName[NAME_LENGTH_CHANNEL];
	DWORD dwVideoFormat;	// 只读：视频制式 1-NTSC 2-PAL
	BYTE byBrightness;  	//亮度,0-255
	BYTE byContrast;    	//对比度,0-255	
	BYTE bySaturation;  	//饱和度,0-255 
	BYTE byHue;    			//色调,0-255	
};

//压缩参数
struct NET_HHVDVR_COMPRESSION_INFO
{
	BYTE byStreamType;		//码流类型0-视频流,1-复合流
	BYTE byResolution;  	//分辨率0-QCIF 1-CIF, 2-2CIF, 3-4CIF
	BYTE byBitrateType;		//码率类型0:变码率，1:定码率
	BYTE  byPicQuality;		//图象质量 0-最好 1-次好 2-较好 3-一般 4-较差 5-差	
	DWORD dwVideoFrameRate;	//帧率
	DWORD dwImageFrameRate;	//原始图像采集帧率
};

//录像时段
struct NET_HHVDVR_RECORDSCHED
{
	//开始时间
    BYTE byStartHour;
	BYTE byStartMin;
	BYTE byStartSec;

	//结束时间
	BYTE byStopHour;
	BYTE byStopMin;
	BYTE byStopSec;

	BYTE byRecordType;	//录像类型 0:定时录像
	CHAR reservedData[5];
};

struct NET_HHVDVR_RECORD
{
	DWORD dwSize;
	DWORD dwRecord;  //是否录像 0-否 1-是
	NET_HHVDVR_RECORDSCHED struRecordSched[MAX_DAYS][MAX_TIMESEGMENT];
	DWORD dwRecordTime;	// 录象时间长度 
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
                                                                                
//功能: 直接停止录像的udp消息
struct SRecordStop_Msg_Direct
{                 
    COMMAND_HEADER  header;

	SGUID DVRID; 
	char DVRIP[_DVRIPLen];				
	unsigned short	 DVRPort; // DVR的端口

	unsigned int Channel;
	unsigned char	type; //录像类型
	
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

//功能: 直接开始录像的udp消息
struct SRecordStart_Msg_Direct
{                 
    COMMAND_HEADER  header;

	SGUID	 DVRID;				//DVR的编码，用于创建唯一存储目录
	char DVRIP[_DVRIPLen];				
	unsigned short	 DVRPort;	//	DVR的端口
	CHAR Password[32];			//	密码		
	char UserName[32];			//	登陆DVR的用户名
	unsigned int	 DVRType;		//	DVR的类型
	unsigned int	 DVRSubType;	//	DVR的子类型
	unsigned int	 u32DVRMFRType;	
	unsigned int  streamType;		//  监视流类型

	unsigned int Channel;
	unsigned char	type;			//录像类型
	unsigned int	elapse;			//录像时长

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
   


//SERVER接收的配置信息：
struct HHVSERVER_INFO
{
	unsigned int		VideoServerID;
	char    VideoServerName[32];//服务器名称
	char    VideoServerIP[32];	//服务器IP地址
	unsigned short		ServerPort;		//提供服务端口
	unsigned short		IntraPort;		//内部通讯端口
	unsigned int		MaxUser;			
	unsigned int		MaxChannel;
	char	McastIP[32];
	char	Recorderpath[MAX_PATH];	
	unsigned int		Intertime;			//单位录像文件时间长度
	unsigned char		OverLay;			//是否自动覆盖

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

//STORESERVER登陆返回
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
	unsigned char		bAdd; //0:删除DVR, 1: 添加DVR 
	SGUID	DVRID;
	
	unsigned int		AddrType;//支持根据ID自动解析IP 
	
	char	DVRIP[_DVRIPLen];				
	unsigned short		DVRPort;		//DVR的端口
	unsigned char		ChannelNum;		//通道数量

	//连接主机的管理密码
	//将长度由10增至32
	CHAR REQ_Manager_Password[32];	
	
	//浏览密码
	//将长度由10增至32
	CHAR REQ_Video_Password[32];			
	
	//登陆DVR的用户名, 当前用于DLH264的DVR
	char UserName_LoginDVR[32];	
	
	unsigned int DVRType;
	
	unsigned int DVRSubType;

	unsigned int u32DVRMFRType;	

	//DVR的最大连接限制
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
 *	功能: 定时录像记录
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


#define UPDATE_TYPE_VPK	1	//智能分析配置
//文件上传请求
struct REMOTE_UPDATE_BEGIN_REQUEST
{
    COMMAND_HEADER  header ;
    U16				loginID; 
	U16				uploadType;//配置文件类型
	U16				channel;
	U32				Length;	//升级文件长度
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

//实际数据升级
struct REMOTE_UPDATE_IMAGEDATA_REQUEST
{
    COMMAND_HEADER  header ;
    U16				loginID; 
	U16				length;
	U8				data[1024];	//数据
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


//数据下载结束
struct REMOTE_UPDATE_END_REQUEST
{
    COMMAND_HEADER  header ;
    U16				loginID; 
	U32				CheckSum;	//数据校验和
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

//Dvr重新启动
#define RESTART_TYPE_DVR	0
#define RESTART_TYPE_VPK	1
#define RESTART_TYPE_PC		2
struct REMOTE_REBOOT_DVR_REQUEST
{
    COMMAND_HEADER  header ;
    U16				loginID;
	U16				restartType;	//0:重启DVR 1:重启VPK 2://重启机器
	U16				channel;		//VPK通道号
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


//生成关键帧
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
