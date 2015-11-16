/*
 *	If_DBRP.h
 *	Dynamsoft Barcode Reader C++ API header file.
 *
 *	Copyright (C) 2014 Dynamsoft Corporation.
 *	All Rights Reserved.
 */

#ifndef __IF_DBRP_H__
#define __IF_DBRP_H__

#include "CommonConfig.h"
#include "BarcodeFormat.h"
#include "ErrorCode.h"
#include "ExportDefines.h"
#include "BarcodeStructs.h"

class BarcodeReaderInner;

NS_DBR_NATIVE_BEG

/*classes*/
class DBR_API CBarcodeReader
{
private:
	BarcodeReaderInner* m_pBarcodeReader;

public:
	CBarcodeReader();
	~CBarcodeReader();

	int InitLicense(const char* pLicense);

	int DecodeFile(const char* pFileName);

	int DecodeFileRect(const char* pFileName,						   
						   int iRectLeft,
						   int iRectTop,
						   int iRectWidth,
						   int iRectHeight);

#if defined(_WIN32) || defined(_WIN64)
	int DecodeDIB(HANDLE hDIB);

	int DecodeDIBRect(HANDLE hDIB, 
					   int iRectLeft,
					   int iRectTop,
					   int iRectWidth,
					   int iRectHeight);
#endif

	int DecodeBuffer(unsigned char* pDIBBuffer, int iBufferSize);

	int DecodeBufferRect(unsigned char* pDIBBuffer,
						int iBufferSize,
						int iRectLeft,
						int iRectTop,
						int iRectWidth,
						int iRectHeight);

	int SetReaderOptions(const ReaderOptions& option);

	const ReaderOptions& GetReaderOptions() const;
	
	int GetBarcodes(pBarcodeResultArray* ppResults);

	static void FreeBarcodeResults(pBarcodeResultArray *ppResults);
};

NS_DBR_NATIVE_END

extern "C" DBR_API const char* DBR_GetErrorString(int iErrorCode);
extern "C" DBR_API const char* DBR_GetVersion();

#endif