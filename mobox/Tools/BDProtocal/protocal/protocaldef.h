#ifndef __PROTOCAL_DEF__H_
#define __PROTOCAL_DEF__H_

/***********************************************************
 *厂家定义区域
 ***********************************************************/

//#define __YDHQ_        //远东华强
#define __SHANGYU_   //石家庄尚宇
//#define __BONA_      //江苏博纳雨田

/***********************************************************
 *协议类型定义区域
 ***********************************************************/
#ifdef __YDHQ_
#define CBDProtocal2_1 CRdss2_1SearchExp
#define CBDProtocal4_0 CRdss4_0SearchExp
#endif

#ifdef __SHANGYU_
#define CBDProtocal2_1 CRdss2_1SearchExp_shangyu
#define CBDProtocal4_0 CRdss4_0SearchExp
#endif

#ifdef __BONA_
#define CBDProtocal2_1 CRdss2_1SearchExp
#define CBDProtocal4_0 CRdss4_0SearchExp_BONA
#endif

#endif