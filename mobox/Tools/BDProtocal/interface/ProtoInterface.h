
/*---------------------------------------------------------------------------------------
��Ȩ���� (C), 2013, �й����ӿƼ����ŵ�28�о�������������Ȩ����
δ���й����ӿƼ����ŵ�28�о�������Ȩ��ɲ������Ը��ƣ��ü���ַ���Դ���롣
---------------------------------------------------------------------------------------*/

/*
 �ļ���:	ProtoInterface.h
 ����:		�����豸�����ӿں���
 �޸���ʷ:
	2013.10 ���� ����
 */

#ifndef _PROTO_INTERFACE_HEAD_
#define _PROTO_INTERFACE_HEAD_

#include "nscHead.h"
#include "BD2User.h"

/************************************************************************/
//                                 �궨��                               //
/************************************************************************/
//������롢�궨��(������)
#define PROTO_SUCCESS			(1)
#define PROTO_FAIL				(0)


/************************************************************************/
//                                 ����                                 //
/************************************************************************/
//���͵Ķ��������(typedef,struct,enum��,������)
typedef struct
{
	double X0; //ƽ�Ʋ���
	double Y0;
	double Z0;
	double Ex;  //��ת����
	double Ey;
	double Ez;
	double m;   //��������(��ͬ����ϵʹ�õĵ�λ�᲻һ��)
}HG_TARANS_PARA;

typedef struct tagBLHorXYZ_HG
{
	tagBLHorXYZ_HG()
	{
		BorX = 0.0;
		LorY = 0.0;
		HorZ = 0.0;
	}
	double BorX;    //γ��/X  (��λ����/��)
	double LorY;   //����/Y   (��λ����/��)
	double HorZ;   //�߶�/Z  (��λ����)

}HG_BLHorXYZ;

/* ----------------------------------------------------------------------
���ڼ�¼һ��ԭʼ������������������
pSrcData	�������ݽṹ��ָ�루�˽ṹ�����ڶ�̬���ڲ��ľ�̬������
iMsgType	�������ݽṹ������
iState		��������״̬���ɹ���ʧ�ܵȣ�
----------------------------------------------------------------------*/
typedef struct  tagResolvedData
{
	tagResolvedData()
	{
		pSrcData = NULL;
		iState = 0;
	}
	char*	pSrcData;
	int		iMsgType;
	int		iState;
}RESOLVED_DATA;

typedef struct tagSendData 
{
	tagSendData()
	{
		pSendData = NULL;
		iCmdType = 0;
	}
	char*	pSendData;
	int		iCmdType;
}SEND_DATA;

//�յ����ݵĻص�����
/*----------------------------------------------------------------------
	������:		PROTO_RECV_DATA_CB_FUNC                                                                 
	����:		�������ݵĻص�����                                                                  
	����˵��:                                                                   
		[IN]pSrcData			<char *>		��������ԭʼ���,�ڻص������в�������!	
		[IN]ilength				<int>			��������ԭʼ���ĳ���	
	����ֵ:
		���ɹ��򷵻�PROTO_SUCCESS(0),ʧ�ܷ��غ궨���еĴ������
	��ע:     
		ʹ�����ProtoIFRegCBFunc����˵��
--------------------------------------------------------------------*/
typedef int (*PROTO_RECV_DATA_CB_FUNC)(char* pSrcData, int ilength);


/************************************************************************/
//                               �ӿں���                               //
/************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif

/*----------------------------------------------------------------------
	������:		ProtoIFInit                                                                 
	����:		Э��ģ���ʼ��                                                                  
	����˵��:                                                                   
		[IN]szPort		<char *>		�������ƣ�����"COM1"	                                                                
		[IN]hWnd		<void *>		���Ͳ�������ʱ������֪ͨ��Ϣ�Ĵ��ھ������û�д��ڣ���NULL	                                                                
		[IN]uiUserMsg	<unsigned int>	���Ͳ�������ʱ���򴰿���������Ϣ���룬��û�д��ڣ�������֪ͨ�ź������	  
										��ʹ��vxworksϵͳʱ���˲���Ϊ��������
		[IN]dwBaudRate	<unsigned long>	���ڲ����ʣ�Ĭ��ֵΪ115200	                                                                
   
	����ֵ:
		���ɹ��򷵻�PROTO_SUCCESS(0),ʧ�ܷ��غ궨���еĴ������
	��ע:     
		����д�����ͨ����������Ϣ֪ͨ�յ�����;��û�д�����ͨ�������ź�����֪ͨ�յ�����
		���hWnd,uiUserMsg��Ϊ�գ����Ե���ProtoIFRegCBFuncע��ص�������֪ͨ�յ�����
--------------------------------------------------------------------*/
PROTOINTERFACE_API  extern int  
ProtoIFInit(char *szPort, void *hWnd, unsigned int uiUserMsg, unsigned long dwBaudRate=115200);

/*----------------------------------------------------------------------
	������:		ProtoIFRegCBFunc                                                               
	����:		ע��ص�����                                                                 
	����˵��:                                                                   
		[IN]pDataCallBackFunc		<PROTO_RECV_DATA_CB_FUNC>		�������ݻص�����ָ�룬�������"����"�еĻص���������	                                                                
   
	����ֵ:
		���ɹ��򷵻�PROTO_SUCCESS(0),ʧ�ܷ��غ궨���еĴ������
	��ע:     
		����ProtoIFInit������hWnd,uiUserMsg��Ϊ�ա��ɵ��ú���ע��ص������������յ����ݺ�ģ�����ûص�����֪ͨ�ϲ�
--------------------------------------------------------------------*/
PROTOINTERFACE_API  extern int  __cdecl
ProtoIFRegCBFunc(PROTO_RECV_DATA_CB_FUNC pDataCallBackFunc);

/*----------------------------------------------------------------------
������:		ProtoIFRecvData                                                               
����:		��������                                                               
����˵��:                                                                   
[IN]szDataBuffer		<char *>					�������ݻ�������ַ,���ϲ㸺����������
[IN]iDatalen			<int>						�ϲ����Ľ������ݻ�������С,�������ý������ؽ������ݴ�С
[OUT]pResolveData		<RESOLVED_DATA *>			�����ս�����ɵ�����

����ֵ:
���ɹ��򷵻�PROTO_SUCCESS(0),ʧ�ܷ��غ궨���еĴ������
��ע:     
û��ע��ص�����ʱ����֪ͨ�ϲ��յ����ݺ󣬵��ô˺����������ݽ���
--------------------------------------------------------------------*/
PROTOINTERFACE_API  extern int  __cdecl
ProtoIFRecvData(char *szDataBuffer, int iDatalen, RESOLVED_DATA * pResolveData);

/*----------------------------------------------------------------------
	������:		ProtoIFSendData                                                               
	����:		��������                                                               
	����˵��:    
		[IN]pSendData		<SEND_DATA *>		���õĴ���������

	����ֵ:
		���ɹ��򷵻�PROTO_SUCCESS(0),ʧ�ܷ��غ궨���еĴ������
	��ע:     
		��
--------------------------------------------------------------------*/
PROTOINTERFACE_API  extern int  __cdecl
ProtoIFSendData(SEND_DATA *pSendData, char*& pSendMsg);

PROTOINTERFACE_API  extern int  __cdecl
ProtoIFGetEncodeLen();

/*----------------------------------------------------------------------
	������:		ProtoIFClose                                                               
	����:		�ر�Э��ģ��                                                               
	����˵��:   
		�޲���
		
	����ֵ:
		���ɹ��򷵻�PROTO_SUCCESS(0),ʧ�ܷ��غ궨���еĴ������
	��ע:     
		��
--------------------------------------------------------------------*/
PROTOINTERFACE_API  extern int  __cdecl
ProtoIFClose();

PROTOINTERFACE_API extern int __cdecl 
ProtoEnumPcidevs(int iNo);

PROTOINTERFACE_API extern int __cdecl 
ProtoIFInitPCI(int iPciNo, void *hWnd, unsigned int uiUserMsg);

typedef long APP_ID;
PROTOINTERFACE_API extern long __cdecl Bd1e_UnPack(
				 APP_ID AppId,
				 const char type,
				 unsigned char * pMsgBuff,
				 long MsgBuffLen,
				 unsigned char * pStructBuff,
				 unsigned long * pStructLen);

/*----------------------------------------------------------------------
������:		HG_SetCoordinateTransPara                                                               
����:		��������ת������                                                               
����˵��:   
	[IN]pa		<TEMP_TARANS_PARA>			����ת������

����ֵ:
���ɹ��򷵻�PROTO_SUCCESS(0),ʧ�ܷ��غ궨���еĴ������
��ע:     
��
--------------------------------------------------------------------*/
PROTOINTERFACE_API  extern void __cdecl
HG_SetCoordinateTransPara(HG_TARANS_PARA pa);

/*----------------------------------------------------------------------
������:		HG_GetCoordinateTrans                                                               
����:		����ת��                                                               
����˵��:   
[OUT]pDestCoordinate		<HG_BLHorXYZ*>			ת���������
[IN]SrcCoordinate			<HG_BLHorXYZ>			ת��ǰ������
[IN]DestCoordinateType		<int>				ת�������������
[IN]SrcCoordinateType		<int>				ת��ǰ����������
����ֵ:
��
��ע:     
����������
0��ʾBJ54�����
1��ʾBJ54��ֱ��XYZ
2��ʾBJ54����˹XYH
3��ʾBJ54������XYH
4��ʾWGS84�����
5��ʾWGS84��ֱ��XYZ
6��ʾWGS84����˹XYH
7��ʾWGS84������XYH
8��ʾCGS2000�����
9��ʾCGS2000��ֱ��
10��ʾCGS2000����˹
11��ʾCGS2000������
��   ��CGS2000����WGS84������
--------------------------------------------------------------------*/
PROTOINTERFACE_API  extern void __cdecl
HG_GetCoordinateTrans(HG_BLHorXYZ  *pDestCoordinate, 
						   HG_BLHorXYZ  SrcCoordinate,
						   int DestCoordinateType,
						   int SrcCoordinateType);	//����ת��

#ifdef __cplusplus
} //extern "C"
#endif

#endif // _PROTO_INTERFACE_HEAD_