
/*---------------------------------------------------------------------------------------
版权所有 (C), 2013, 中国电子科技集团第28研究所。保留所有权利。
未经中国电子科技集团第28研究所的授权许可不得擅自复制，裁剪或分发该源代码。
---------------------------------------------------------------------------------------*/

/*
 文件名:	ProtoInterface.h
 功能:		定义设备驱动接口函数
 修改历史:
	2013.10 汪磊 创建
 */

#ifndef _PROTO_INTERFACE_HEAD_
#define _PROTO_INTERFACE_HEAD_

#include "nscHead.h"
#include "BD2User.h"

/************************************************************************/
//                                 宏定义                               //
/************************************************************************/
//错误代码、宏定义(待补充)
#define PROTO_SUCCESS			(1)
#define PROTO_FAIL				(0)


/************************************************************************/
//                                 类型                                 //
/************************************************************************/
//类型的定义和声明(typedef,struct,enum等,待补充)
typedef struct
{
	double X0; //平移参数
	double Y0;
	double Z0;
	double Ex;  //旋转参数
	double Ey;
	double Ez;
	double m;   //比例缩放(不同坐标系使用的单位会不一样)
}HG_TARANS_PARA;

typedef struct tagBLHorXYZ_HG
{
	tagBLHorXYZ_HG()
	{
		BorX = 0.0;
		LorY = 0.0;
		HorZ = 0.0;
	}
	double BorX;    //纬度/X  (单位：度/米)
	double LorY;   //经度/Y   (单位：度/米)
	double HorZ;   //高度/Z  (单位：米)

}HG_BLHorXYZ;

/* ----------------------------------------------------------------------
用于记录一条原始数据所解析出的内容
pSrcData	解析数据结构体指针（此结构体是在动态库内部的静态变量）
iMsgType	解析数据结构体类型
iState		解析数据状态（成功、失败等）
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

//收到数据的回调函数
/*----------------------------------------------------------------------
	类型名:		PROTO_RECV_DATA_CB_FUNC                                                                 
	功能:		接收数据的回调函数                                                                  
	参数说明:                                                                   
		[IN]pSrcData			<char *>		接收数据原始语句,在回调函数中不能销毁!	
		[IN]ilength				<int>			接收数据原始语句的长度	
	返回值:
		若成功则返回PROTO_SUCCESS(0),失败返回宏定义中的错误代码
	备注:     
		使用请见ProtoIFRegCBFunc函数说明
--------------------------------------------------------------------*/
typedef int (*PROTO_RECV_DATA_CB_FUNC)(char* pSrcData, int ilength);


/************************************************************************/
//                               接口函数                               //
/************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif

/*----------------------------------------------------------------------
	函数名:		ProtoIFInit                                                                 
	功能:		协议模块初始化                                                                  
	参数说明:                                                                   
		[IN]szPort		<char *>		串口名称，例如"COM1"	                                                                
		[IN]hWnd		<void *>		当低层有数据时，接收通知消息的窗口句柄，若没有窗口，填NULL	                                                                
		[IN]uiUserMsg	<unsigned int>	当低层有数据时，向窗口所发的消息代码，若没有窗口，可以填通知信号量句柄	  
										当使用vxworks系统时，此参数为串口类型
		[IN]dwBaudRate	<unsigned long>	串口波特率，默认值为115200	                                                                
   
	返回值:
		若成功则返回PROTO_SUCCESS(0),失败返回宏定义中的错误代码
	备注:     
		如果有窗口则通过发窗口消息通知收到数据;若没有窗口则通过设置信号量来通知收到数据
		如果hWnd,uiUserMsg均为空，可以调用ProtoIFRegCBFunc注册回调函数来通知收到数据
--------------------------------------------------------------------*/
PROTOINTERFACE_API  extern int  
ProtoIFInit(char *szPort, void *hWnd, unsigned int uiUserMsg, unsigned long dwBaudRate=115200);

/*----------------------------------------------------------------------
	函数名:		ProtoIFRegCBFunc                                                               
	功能:		注册回调函数                                                                 
	参数说明:                                                                   
		[IN]pDataCallBackFunc		<PROTO_RECV_DATA_CB_FUNC>		接收数据回调函数指针，定义请见"类型"中的回调函数定义	                                                                
   
	返回值:
		若成功则返回PROTO_SUCCESS(0),失败返回宏定义中的错误代码
	备注:     
		若在ProtoIFInit函数中hWnd,uiUserMsg均为空。可调用函数注册回调函数，当接收到数据后，模块会调用回调函数通知上层
--------------------------------------------------------------------*/
PROTOINTERFACE_API  extern int  __cdecl
ProtoIFRegCBFunc(PROTO_RECV_DATA_CB_FUNC pDataCallBackFunc);

/*----------------------------------------------------------------------
函数名:		ProtoIFRecvData                                                               
功能:		接收数据                                                               
参数说明:                                                                   
[IN]szDataBuffer		<char *>					接收数据缓冲区地址,由上层负责分配和销毁
[IN]iDatalen			<int>						上层分配的接收数据缓冲区大小,函数调用结束返回接收数据大小
[OUT]pResolveData		<RESOLVED_DATA *>			所接收解析完成的数据

返回值:
若成功则返回PROTO_SUCCESS(0),失败返回宏定义中的错误代码
备注:     
没有注册回调函数时，当通知上层收到数据后，调用此函数进行数据接收
--------------------------------------------------------------------*/
PROTOINTERFACE_API  extern int  __cdecl
ProtoIFRecvData(char *szDataBuffer, int iDatalen, RESOLVED_DATA * pResolveData);

/*----------------------------------------------------------------------
	函数名:		ProtoIFSendData                                                               
	功能:		发送数据                                                               
	参数说明:    
		[IN]pSendData		<SEND_DATA *>		填充好的待发送数据

	返回值:
		若成功则返回PROTO_SUCCESS(0),失败返回宏定义中的错误代码
	备注:     
		无
--------------------------------------------------------------------*/
PROTOINTERFACE_API  extern int  __cdecl
ProtoIFSendData(SEND_DATA *pSendData, char*& pSendMsg);

PROTOINTERFACE_API  extern int  __cdecl
ProtoIFGetEncodeLen();

/*----------------------------------------------------------------------
	函数名:		ProtoIFClose                                                               
	功能:		关闭协议模块                                                               
	参数说明:   
		无参数
		
	返回值:
		若成功则返回PROTO_SUCCESS(0),失败返回宏定义中的错误代码
	备注:     
		无
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
函数名:		HG_SetCoordinateTransPara                                                               
功能:		设置坐标转换参数                                                               
参数说明:   
	[IN]pa		<TEMP_TARANS_PARA>			坐标转换参数

返回值:
若成功则返回PROTO_SUCCESS(0),失败返回宏定义中的错误代码
备注:     
无
--------------------------------------------------------------------*/
PROTOINTERFACE_API  extern void __cdecl
HG_SetCoordinateTransPara(HG_TARANS_PARA pa);

/*----------------------------------------------------------------------
函数名:		HG_GetCoordinateTrans                                                               
功能:		坐标转换                                                               
参数说明:   
[OUT]pDestCoordinate		<HG_BLHorXYZ*>			转换后的坐标
[IN]SrcCoordinate			<HG_BLHorXYZ>			转换前的坐标
[IN]DestCoordinateType		<int>				转换后的坐标类型
[IN]SrcCoordinateType		<int>				转换前的坐标类型
返回值:
无
备注:     
坐标类型有
0表示BJ54—大地
1表示BJ54—直角XYZ
2表示BJ54—高斯XYH
3表示BJ54—麦卡托XYH
4表示WGS84—大地
5表示WGS84—直角XYZ
6表示WGS84—高斯XYH
7表示WGS84—麦卡托XYH
8表示CGS2000—大地
9表示CGS2000—直角
10表示CGS2000—高斯
11表示CGS2000—麦卡托
※   将CGS2000按照WGS84来换算
--------------------------------------------------------------------*/
PROTOINTERFACE_API  extern void __cdecl
HG_GetCoordinateTrans(HG_BLHorXYZ  *pDestCoordinate, 
						   HG_BLHorXYZ  SrcCoordinate,
						   int DestCoordinateType,
						   int SrcCoordinateType);	//坐标转换

#ifdef __cplusplus
} //extern "C"
#endif

#endif // _PROTO_INTERFACE_HEAD_