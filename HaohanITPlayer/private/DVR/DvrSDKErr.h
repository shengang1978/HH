#ifndef _DVR_SDK_ERROR_H
#define _DVR_SDK_ERROR_H

//错误定义

#define HHV_ERROR_CREATESOCKET	-10		//创建socket失败
#define HHV_ERROR_TIMEOUT		-30
#define HHV_ERROR_CONNECT		-40		//建立连接失败
#define HHV_ERROR_SEND			-49		//发送数据失败
#define HHV_ERROR_RECV			-50		//接收数据失败
#define HHV_ERROR_SOCKET_SELECT			-51   //参数错误
#define HHV_ERROR_INVALID_PARA			-60   //参数错误
#define HHV_ERROR_INVALID_IP			-61   //非法IP地址

#define HHV_ERROR_ENTER_CSLOCK			-70	//进入互斥失败
#define HHV_ERROR_NOIDLEPLAYER			-71 //无空闲播放器
#define HHV_ERROR_POORMEMORY			-72 //申请内存失败

#define HHV_ERROR_LOADDLL_SDK				-100				//加载动态库失败
#define HHV_ERROR_LOADDLL_FUNCTION			-101				//加载库函数失败
#define HHV_ERROR_SOCK_TIMEOUT				-102					//设置网络超时时间失败
#define HHV_ERROR_LOGINDVR_CONNECTING		-103          //正在连接DVR
#define HHV_ERROR_USER_OUTLINE				-104                 //用户没有登陆
#define HHV_ERROR_REALSTREM_START			-105				//启动实时流失败  
#define HHV_ERROR_NOSTART_MONITOR			-106             //没有启动监视
#define HHV_ERROR_DVRCHANNUM_ABNORMIT		-107          //通道号大于DVR通道数 
#define HHV_ERROR_REALSTREM_NOSTART		-108            //没有启动实时流  
#define HHV_ERROR_RECORDCHAN_GETSTATE		-109			//获取录像通道状态失败
#define HHV_ERROR_USERID_INVALID			-110				//用户ID失效
#define HHV_ERROR_NoFilePlayBack			-120				//无录像文件

#define HHV_ERROR_NOLOGIN					-130		//没有登录

#define HHV_ERROR_INVALID_USERNAMEORPWD		-140		
#define HHV_ERROR_GROUP_NOTALLOCATE			-141
#define HHV_ERROR_ADMINI_ONLINE				-142
#define HHV_ERROR_DataBase_ERROR			-143  //数据库出错, 无法验证用户
#define HHV_ERROR_USER_NORESON				-144  //未知原因

#define HHV_ERROR_STOP_STATUS				-150
#define HHV_ERROR_LOCALLOGINMAX			-151
#define HHV_ERROR_INVALIDPARA				-152

#define HHV_ERROR_OPENFILE			-160
#define HHV_ERROR_UPDATEBEGIN		-170//开始更新失败
#define HHV_ERROR_READFILE			-171//读文件失败
#define HHV_ERROR_UPDATEDATA		-172
#define HHV_ERROR_UPDATEND			-173
#define HHV_ERROR_REBOOTDVR			-174//重启DVR失败
#define HHV_ERROR_GENEIFRAME		-400	//生成关键帧失败

#endif