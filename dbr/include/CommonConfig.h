#ifndef __COMMON_CONFIG__
#define __COMMON_CONFIG__

#if !defined(_WIN32) && !defined(_WIN64)
typedef long long __int64;
#endif

#ifdef _USE_DBR_NATIVE_NAMESPACE
#define NS_DBR_NATIVE_BEG namespace Dynamsoft{ namespace BarcodeNative {
#define NS_DBR_NATIVE_END }}
#else
#define NS_DBR_NATIVE_BEG 
#define NS_DBR_NATIVE_END
#endif

#endif