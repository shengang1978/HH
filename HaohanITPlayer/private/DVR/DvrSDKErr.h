#ifndef _DVR_SDK_ERROR_H
#define _DVR_SDK_ERROR_H

//������

#define HHV_ERROR_CREATESOCKET	-10		//����socketʧ��
#define HHV_ERROR_TIMEOUT		-30
#define HHV_ERROR_CONNECT		-40		//��������ʧ��
#define HHV_ERROR_SEND			-49		//��������ʧ��
#define HHV_ERROR_RECV			-50		//��������ʧ��
#define HHV_ERROR_SOCKET_SELECT			-51   //��������
#define HHV_ERROR_INVALID_PARA			-60   //��������
#define HHV_ERROR_INVALID_IP			-61   //�Ƿ�IP��ַ

#define HHV_ERROR_ENTER_CSLOCK			-70	//���뻥��ʧ��
#define HHV_ERROR_NOIDLEPLAYER			-71 //�޿��в�����
#define HHV_ERROR_POORMEMORY			-72 //�����ڴ�ʧ��

#define HHV_ERROR_LOADDLL_SDK				-100				//���ض�̬��ʧ��
#define HHV_ERROR_LOADDLL_FUNCTION			-101				//���ؿ⺯��ʧ��
#define HHV_ERROR_SOCK_TIMEOUT				-102					//�������糬ʱʱ��ʧ��
#define HHV_ERROR_LOGINDVR_CONNECTING		-103          //��������DVR
#define HHV_ERROR_USER_OUTLINE				-104                 //�û�û�е�½
#define HHV_ERROR_REALSTREM_START			-105				//����ʵʱ��ʧ��  
#define HHV_ERROR_NOSTART_MONITOR			-106             //û����������
#define HHV_ERROR_DVRCHANNUM_ABNORMIT		-107          //ͨ���Ŵ���DVRͨ���� 
#define HHV_ERROR_REALSTREM_NOSTART		-108            //û������ʵʱ��  
#define HHV_ERROR_RECORDCHAN_GETSTATE		-109			//��ȡ¼��ͨ��״̬ʧ��
#define HHV_ERROR_USERID_INVALID			-110				//�û�IDʧЧ
#define HHV_ERROR_NoFilePlayBack			-120				//��¼���ļ�

#define HHV_ERROR_NOLOGIN					-130		//û�е�¼

#define HHV_ERROR_INVALID_USERNAMEORPWD		-140		
#define HHV_ERROR_GROUP_NOTALLOCATE			-141
#define HHV_ERROR_ADMINI_ONLINE				-142
#define HHV_ERROR_DataBase_ERROR			-143  //���ݿ����, �޷���֤�û�
#define HHV_ERROR_USER_NORESON				-144  //δ֪ԭ��

#define HHV_ERROR_STOP_STATUS				-150
#define HHV_ERROR_LOCALLOGINMAX			-151
#define HHV_ERROR_INVALIDPARA				-152

#define HHV_ERROR_OPENFILE			-160
#define HHV_ERROR_UPDATEBEGIN		-170//��ʼ����ʧ��
#define HHV_ERROR_READFILE			-171//���ļ�ʧ��
#define HHV_ERROR_UPDATEDATA		-172
#define HHV_ERROR_UPDATEND			-173
#define HHV_ERROR_REBOOTDVR			-174//����DVRʧ��
#define HHV_ERROR_GENEIFRAME		-400	//���ɹؼ�֡ʧ��

#endif