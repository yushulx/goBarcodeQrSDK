#ifndef __EXPORT_DEFINES__
#define __EXPORT_DEFINES__

#if defined(_WIN32) || defined(_WIN64)
	#ifdef DBR_EXPORTS
		#define DBR_API __declspec(dllexport)
	#else
		#define DBR_API 
	#endif

#include <windows.h>

#else
	#define DBR_API
#endif

#endif