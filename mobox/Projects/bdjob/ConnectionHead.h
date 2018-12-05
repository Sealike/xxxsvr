#ifndef __HEAD_H__
#define __HEAD_H__

#define _DLL_COMPILE__
#ifdef _DLL_COMPILE__
#define  PROTOINTERFACE_EXPORTS
#endif

typedef  char*  outbuffer_t;
typedef  char*  inbuffer_t;
typedef  void*  outvoid_t;
typedef  char*  protocalname_t;

#ifndef  _WINDEF_
	typedef unsigned int   UINT;
	typedef unsigned long  DWORD;
	typedef unsigned char  BYTE;
	typedef int            BOOL;
	typedef unsigned short WORD;
	#define TRUE     1
	#define FALSE    0
#endif
#endif
