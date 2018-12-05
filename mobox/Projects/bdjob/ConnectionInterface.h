#ifndef _CONNECTION_INTERFACE_H__
#define _CONNECTION_INTERFACE_H__

/****************************************************************************************
 *版权所有 (C), 2016, 深圳市远东华强导航定位有限公司。保留所有权利。
 *未经深圳市远东华强导航定位有限公司的授权许可不得擅自复制，裁剪或分发该源代码。
 ****************************************************************************************/

/****************************************************************************************
 *文件名:	ProtoInterface.h
 *功能:		北斗2.1/4.0协议接口函数
 *修改历史:
 *2016.1.12 liuxiaoliang 创建
 ****************************************************************************************/

#include "ConnectionHead.h"

//#include "../BDProtocal/protocal/ChnavtectGlobal.h"
#include "ChnavtectGlobal.h"
#ifdef PROTOINTERFACE_EXPORTS
#define PROTOINTERFACE_API __declspec(dllexport)
#else
#define PROTOINTERFACE_API __declspec(dllimport)
#endif


/*******************paritytype********************
 *	EVENPARITY
 *	MARKPARITY
 *	NOPARITY
 *	ODDPARITY
 *	SPACEPARITY
 *************************************************/

/* ********************stopbits********************
 *	ONESTOPBIT   1   stop bit 
 *	ONE5STOPBITS 1.5 stop bits 
 *	TWOSTOPBITS  2   stop bits 
 *************************************************/

/*
 *  nPort，目标端口号
 *  nBaudRate,打开串口的波特率；
 *  nDataBits,数据位的个数；
 *  nStopBits,停止位的个数；
 *  nParity， 校验位的方式；
 *  ppConnetion：输出参数，创建的链接指针；
 *  返回值：创建成功，返回TRUE,否则：返回FALSE；
 */

PROTOINTERFACE_API  extern BOOL __cdecl open_com_connection(const int nPort,const int nBaudRate,const int nDataBits,const int nStopBits,BOOL bIfParity,const int nParity,IConnection**ppConnetion);


#endif