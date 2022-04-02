
#ifndef _APDU_DATATYPES_H_
#define _APDU_DATATYPES_H_


	typedef signed char         INT8;
	typedef signed short        INT16;
	typedef signed int          INT32;
	typedef unsigned char       UINT8;
	typedef unsigned short      UINT16;
	typedef unsigned int	    UINT32;

	typedef UINT8				BYTE;
	typedef char				CHAR;
	typedef INT16				SHORT;
	typedef UINT16				USHORT;
	typedef long				LONG;

	typedef UINT32				UINT;
	typedef UINT16				WORD;
	typedef unsigned long		DWORD;
	typedef UINT32				FLAGS;
	typedef CHAR *				LPSTR;
	typedef void *				HANDLE;
	typedef int					BOOL;
	
	
#ifndef TRUE	
	#define TRUE	1
#endif

#ifndef FALSE
	#define FALSE	0
#endif


	/////////////////////////////////////////

	typedef enum 
	{
		devStateNormal = 0,
		devStateNew = 1,
		devStateRemoved = 2,
		devStateAny=3

	}enmDevState;


#endif