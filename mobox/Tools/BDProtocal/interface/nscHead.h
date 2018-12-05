
#ifndef NSC_HeadH__
#define NSC_HeadH__

//-----------------------------------------------------------------------------
// 定义平台类型
//-----------------------------------------------------------------------------

#define OS_WINDOWS	1
#define OS_LINUX	2
#define OS_VXWORKS	3
#define OS_WINCE	4

#define CPU_IA32 0
#define CPU_ARM  1
#define CPU_PPC  2
#define PROTOINTERFACE_EXPORTS

//-----------------------------------------------------------------------------
// 平台
//-----------------------------------------------------------------------------
#if defined(_WIN32_WCE)  || (_WIN32_WCE>211) 

	#define OS_TYPE  OS_WINCE
	#define CPU_TYPE CPU_IA32

	#ifndef NSC_API
		#ifdef NSC_API_EXPORTS
			#define NSC_API  __declspec(dllexport)
		#else
			#define NSC_API  __declspec(dllimport)
		#endif
	#endif

	#define NSC_STDCALL  __stdcall

#elif defined(_WIN32) || defined(WIN32) || defined(_WINDOWS) || defined(WINDOWS)      // CB5/MVC编译器自定义的宏。
	
	#define OS_TYPE  OS_WINDOWS
	#define CPU_TYPE CPU_IA32

	#ifndef NSC_API
		#ifdef NSC_API_EXPORTS
			#define NSC_API  __declspec(dllexport)
		#else
			#define NSC_API  __declspec(dllimport)
		#endif
	#endif

	#define NSC_STDCALL  __stdcall
	
	#ifdef PROTOINTERFACE_EXPORTS
	#define PROTOINTERFACE_API __declspec(dllexport)
	#else
	#define PROTOINTERFACE_API __declspec(dllimport)
	#endif

#elif defined(_LINUX)

	#define OS_TYPE  OS_LINUX
	#define CPU_TYPE CPU_IA32

	#ifndef NSC_API
		#define NSC_API
	#endif
	
	#define NSC_STDCALL

#else
	#define OS_TYPE  OS_VXWORKS
	#define CPU_TYPE CPU_PPC

	#ifndef NSC_API
		#define NSC_API
	#endif
	
	#define NSC_STDCALL

	#define __cdecl 
	
	#define PROTOINTERFACE_API
	#define ERROR_SUCCESS                    0L
	

#endif


//-----------------------------------------------------------------------------
// 类型定义
//-----------------------------------------------------------------------------
#if OS_TYPE == OS_WINDOWS  || OS_TYPE == OS_WINCE 

	/*#ifndef INT16
		typedef short INT16;
	#endif
	
	#ifndef UINT8
		typedef unsigned char UINT8;
	#endif
	
	#ifndef INT8
		typedef char INT8;
	#endif	

	#ifndef UINT16
		typedef unsigned short UINT16;
	#endif*/

#elif OS_TYPE == OS_LINUX

	#ifndef BOOL
		typedef char BOOL;
	#endif
	
	#ifndef CHAR
		typedef char CHAR;
	#endif
	
	#ifndef INT16
		typedef short INT16;
	#endif
	
	#ifndef INT32
		typedef long INT32;
	#endif
	
	#ifndef INT
		typedef int INT;
	#endif	
	
	#ifndef UINT8
		typedef unsigned char UINT8;
	#endif
	
	#ifndef UINT16
		typedef unsigned short UINT16;
	#endif
	
	#ifndef UINT32
		typedef unsigned long UINT32;
	#endif
	
	#ifndef UINT
		typedef unsigned int UINT;
	#endif

	#ifndef BYTE
		typedef unsigned char BYTE;
	#endif

	#ifndef WORD
		typedef unsigned short WORD;	
	#endif

	#ifndef DWORD
		typedef   unsigned int DWORD;
	#endif


	#ifndef NULL
	#ifdef  __cplusplus
		#define NULL    0
	#else
		#define NULL    ((void *)0)
	#endif
	#endif
#else
	#ifndef BYTE
		typedef char BYTE;
	#endif

	#ifndef WORD
			typedef unsigned short WORD;	
	#endif

	#ifndef DWORD
		typedef unsigned long DWORD;
	#endif

	#ifndef WPARAM
		typedef unsigned int WAPRAM;		
	#endif

	#ifndef LPARAM
		typedef unsigned int LPARAM;		
	#endif

	#ifndef LONG
		typedef long LONG;
	#endif

	#define MAX_PATH          260

	#ifndef NULL
	#ifdef  __cplusplus
		#define NULL    0
	#else
		#define NULL    ((void *)0)
	#endif
	#endif	
#endif


//-----------------------------------------------------------------------------
// SWAP--高低字节转换
//-----------------------------------------------------------------------------

#define SWAP4(x)	((( (x) & 0xff) << 24) | (( (x) & 0xff00) << 8) | (( (x) & 0xff0000) >> 8) | (( (x) & 0xff000000) >> 24))
#define SWAP2(x)	((( (x) & 0xff) << 8) | (( (x) & 0xff00) >> 8)) 

#if CPU_TYPE==CPU_IA32
	#define SWAP2_IA32(a)		(a)
	#define SWAP4_IA32(a)		(a)
#else
	#define SWAP2_IA32(a)		SWAP2(a)
	#define SWAP4_IA32(a)		SWAP4(a)
#endif

//-----------------------------------------------------------------------------
// 头文件
//-----------------------------------------------------------------------------

#if (OS_TYPE == OS_VXWORKS)

	#include <vxWorks.h>
	#include <stdio.h>
	#include <tasklib.h>
	#include <sys/stat.h>
	#include <sys/types.h>
	#include <stdlib.h>
	//#include <string>
	//#include <list>
	#include <stdarg.h>
	#include <types.h>
	#include <dirent.h>
	#include <stat.h>
	#include <unistd.h>
	//#include <fstream>
	#include <errno.h>
	#include <unistd.h>
	#include <ioLib.h>
	#include <iosLib.h>
	#include <resolvLib.h>
	#include <hostLib.h>
	#include <tickLib.h>
	#include <sysLib.h>
	#include <inetLib.h>
	#include <math.h >
	//#include <semaphore.h>
	//#include <pthread.h>

	//#include <string>
	#include <vector>
	//#include <deque>
	//#include <map>
	//#include <set>
	//#include <algorithm>
	//using namespace std;
	#define INFINITE            0xFFFFFFFF  // Infinite timeout
	#define OutputDebugString(a)
	#define _T(a)


#elif (OS_TYPE == OS_LINUX)

	#include <stdio.h>
	#include <errno.h>
	#include <sys/stat.h>
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <netdb.h>
	#include <unistd.h>
	#include <fcntl.h>
	#include <sys/io.h>
	#include <pthread.h>
	#include <semaphore.h>

	#include <string>
	#include <vector>
	#include <deque>
	#include <map>
	#include <set>
	#include <algorithm>
	using namespace std;

#elif (OS_TYPE == OS_WINDOWS)

	#include <windows.h>
	#include <io.h>
	#include <fcntl.h>
	#include <time.h>
	#include <sys/types.h>
	#include <sys/stat.h>
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <errno.h>
	#include <assert.h>

	#include <string>
	#include <vector>
	#include <deque>
	#include <map>
	#include <set>
	#include <algorithm>
	#include <tchar.h>
	#include <vector>
	#include "math.h"
	using namespace std;

#elif (OS_TYPE == OS_WINCE)

	#include <windows.h>
	#include <assert.h>
	#include <stdio.h>
	#include <string.h>
	#include <Winsock2.h>

	#include <string>
	#include <vector>
	#include <deque>
	#include <map>
	#include <set>
	#include <algorithm>
	using namespace std;

#endif

//-----------------------------------------------------------------------------
// 时间函数/宏
//-----------------------------------------------------------------------------

#include <time.h>

#define nscDiffTick(b,a)	((a)<(b))?((b)-(a)):(0xffffffff-(a)+(b))    // 两ticks时间之差

#if OS_TYPE == OS_VXWORKS

	#define nscGetTicks()	  	(tickGet()*1000/sysClkRateGet())	// 取当前ticks,单位ms

	inline void nscSleep(int msec)									// 延时ms
	{
		int tcks = (sysClkRateGet() * msec) / 1000;
		if (tcks < 1)	tcks = 1;
		taskDelay(tcks);
	}

#elif OS_TYPE == OS_WINDOWS  || OS_TYPE == OS_WINCE 

	#define nscGetTicks()	  	GetTickCount()
	#define nscSleep(a)			Sleep(a)

#else

	#define nscGetTicks()	  	GetTickCount()
	#define nscSleep(a)			sleep(a)

#endif

//-------------------------------------------------------------------------------------------------
// 调试/断言
//-------------------------------------------------------------------------------------------------

#if (OS_TYPE == OS_WINDOWS || OS_TYPE == OS_WINCE) 
	NSC_API void NSC_STDCALL _NSC_DEBUG_(char *msg,...);
#endif

#ifdef _DEBUG

	#if (OS_TYPE == OS_VXWORKS || OS_TYPE == OS_LINUX)
	
		#define NSC_DEBUG(fmt, arg...)	printf(fmt, ##arg)

		#define NSC_ASSERT(f)													\
			{																	\
				if (f)															\
					NULL;														\
				else															\
				{																\
					printf("assert error: %s, line %d\n", __FILE__, __LINE__);	\
					fflush(stdout); 											\
					return;														\
				}
			}

		#define NSC_ASSERTV(f,res)												\
			{																	\
				if (f)															\
					NULL;														\
				else															\
				{																\
					printf("assert error: %s, line %d\n", __FILE__, __LINE__);	\
					fflush(stdout);												\
					return res;													\
				}
			}
	
	#else	// windows
	
		#define NSC_DEBUG _NSC_DEBUG_
		
		#define NSC_ASSERT(f)													\
			{																	\
				if (f)															\
					NULL;														\
				else															\
				{																\
					NSC_DEBUG("assert error: %s, line %d\n",__FILE__, __LINE__);	\
					return;														\
				}																\
			}

		#define NSC_ASSERTV(f,res)												\
			{																	\
				if (f)															\
					NULL;														\
				else															\
				{																\
					NSC_DEBUG("assert error: %s, line %d\n",__FILE__, __LINE__);	\
					return res;													\
				}																\
			}
	#endif
			
#else	// 没有定义_DEBUG时

	__inline void NSC_DEBUG(char *fmt, ...) {}
	#define NSC_ASSERT(f)
	#define NSC_ASSERTV(f,res)

#endif


//-------------------------------------------------------------------------------------------------
// 文件打开属性
//-------------------------------------------------------------------------------------------------

#if (OS_TYPE == OS_VXWORKS || OS_TYPE == OS_LINUX)

	#define FILE_MODE	(S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
	#define DIR_MODE	(FILEMODE | S_IXUSR | S_IXGRP | S_IXOTH)

#elif (OS_TYPE == OS_WINDOWS || OS_TYPE == OS_WINCE)

	#define FILE_MODE    (S_IREAD|S_IWRITE)

#endif

//-------------------------------------------------------------------------------------------------
// 安全释放
//-------------------------------------------------------------------------------------------------

#define SAFE_FREE(m)			if (m != NULL)		\
								{					\
									free(m);		\
									m = NULL;		\
								}

#define SAFE_DELETE(s)			if (s != NULL)		\
								{					\
									delete s;		\
									s = NULL;		\
								}

#define SAFE_DELETE_ARRAY(s)	if (s != NULL)		\
								{					\
									delete [] s;	\
                                    s = NULL;       \
								}

#define SAFE_CLOSE(s)			if (s != -1)		\
								{					\
									close(s);		\
									s = -1;			\
								}

//------------------------------------------------------------------------------------------------
// 成功/失败
//-------------------------------------------------------------------------------------------------

#define NSC_OK	  0	    	// 成功
#define NSC_ERROR -1		// 失败

#ifndef	TRUE
	#define TRUE	1
#endif

#ifndef	FALSE
	#define FALSE	0
#endif

//-------------------------------------------------------------------------------------------------
// 基本宏
//-------------------------------------------------------------------------------------------------

#ifndef MIN
	#define MIN(x, y)	((x) < (y) ? (x) : (y))
#endif

#ifndef	MAX
	#define MAX(x, y)	((x) > (y) ? (x) : (y))
#endif

#ifndef	MAKEWORD
	#define MAKEWORD(a, b)	((UINT16)(((UINT8)((UINT32)(a) & 0xff)) | ((UINT16)((UINT8)((UINT32)(b) & 0xff))) << 8))
#endif

#ifndef	MAKELONG
	#define MAKELONG(a, b)	((INT32)(((UINT16)((UINT32)(a) & 0xffff)) | ((UINT32)((UINT16)((UINT32)(b) & 0xffff))) << 16))
#endif

#ifndef	LOWORD
	#define LOWORD(l)		((UINT16)((UINT32)(l) & 0xffff))
#endif

#ifndef	HIWORD
	#define HIWORD(l)		((UINT16)((UINT32)(l) >> 16))
#endif

#ifndef	LOBYTE
	#define LOBYTE(w)		((UINT8)((UINT32)(w) & 0xff))
#endif

#ifndef	HIBYTE
	#define HIBYTE(w)		((UINT8)((UINT32)(w) >> 8))
#endif

#endif   // nscHeadH_
