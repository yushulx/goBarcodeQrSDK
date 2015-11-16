/*
 *	If_DBR.h
 *	Dynamsoft Barcode Reader C API header file.
 *
 *	Copyright (C) 2014 Dynamsoft Corporation.
 *	All Rights Reserved.
 */

#ifndef __IF_DBR_H__
#define __IF_DBR_H__

#include "BarcodeFormat.h"
#include "ErrorCode.h"
#include "ExportDefines.h"

#include "BarcodeStructs.h"

/* functions*/

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

DBR_API int DBR_InitLicense(const char* pLicense);

DBR_API int DBR_DecodeFile(const char* pFileName, 
				   const pReaderOptions pOption,
				   pBarcodeResultArray *ppResults);

DBR_API int DBR_DecodeFileRect(const char* pFileName,
					   const pReaderOptions pOption,
					   int iRectLeft,
					   int iRectTop,
					   int iRectWidth,
					   int iRectHeight,
					   pBarcodeResultArray *ppResults);

DBR_API int DBR_DecodeBuffer(unsigned char* pDIBBuffer,
						 int iBufferSize,
						 const pReaderOptions pOptions,
						 pBarcodeResultArray *ppResults
						 );

DBR_API int DBR_DecodeBufferRect(unsigned char* pDIBBuffer,
							 int iBufferSize,
							 const pReaderOptions pOptions,
							 int iRectLeft,
							 int iRectTop,
							 int iRectWidth,
							 int iRectHeight,
							 pBarcodeResultArray *ppResults
							 );

DBR_API int DBR_FreeBarcodeResults(pBarcodeResultArray *ppResults);

#if defined(_WIN32) || defined(_WIN64)

DBR_API int DBR_DecodeDIB(HANDLE hDIB,
						  const pReaderOptions pOption,
						  pBarcodeResultArray *ppResults
						  );

DBR_API int DBR_DecodeDIBRect(HANDLE hDIB, 
							 const pReaderOptions pOptions,
							 int iRectLeft,
							 int iRectTop,
							 int iRectWidth,
							 int iRectHeight,
							 pBarcodeResultArray *ppResults
							 );
#endif

DBR_API const char* DBR_GetErrorString(int iErrorCode);
DBR_API const char* DBR_GetVersion();

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif