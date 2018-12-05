#ifndef __BTYPES_H__
#define __BTYPES_H__

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
typedef void*          HANDLE;
#define TRUE     1
#define FALSE    0
#endif

typedef __int64        int64_t;
typedef unsigned __int64 uint64_t;

#define NULL   0

#endif