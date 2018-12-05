#ifndef _CONNECTION_INTERFACE_H__
#define _CONNECTION_INTERFACE_H__

/****************************************************************************************
 *��Ȩ���� (C), 2016, ������Զ����ǿ������λ���޹�˾����������Ȩ����
 *δ��������Զ����ǿ������λ���޹�˾����Ȩ��ɲ������Ը��ƣ��ü���ַ���Դ���롣
 ****************************************************************************************/

/****************************************************************************************
 *�ļ���:	ProtoInterface.h
 *����:		����2.1/4.0Э��ӿں���
 *�޸���ʷ:
 *2016.1.12 liuxiaoliang ����
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
 *  nPort��Ŀ��˿ں�
 *  nBaudRate,�򿪴��ڵĲ����ʣ�
 *  nDataBits,����λ�ĸ�����
 *  nStopBits,ֹͣλ�ĸ�����
 *  nParity�� У��λ�ķ�ʽ��
 *  ppConnetion���������������������ָ�룻
 *  ����ֵ�������ɹ�������TRUE,���򣺷���FALSE��
 */

PROTOINTERFACE_API  extern BOOL __cdecl open_com_connection(const int nPort,const int nBaudRate,const int nDataBits,const int nStopBits,BOOL bIfParity,const int nParity,IConnection**ppConnetion);


#endif