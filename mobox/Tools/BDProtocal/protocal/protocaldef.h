#ifndef __PROTOCAL_DEF__H_
#define __PROTOCAL_DEF__H_

/***********************************************************
 *���Ҷ�������
 ***********************************************************/

//#define __YDHQ_        //Զ����ǿ
#define __SHANGYU_   //ʯ��ׯ����
//#define __BONA_      //���ղ�������

/***********************************************************
 *Э�����Ͷ�������
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