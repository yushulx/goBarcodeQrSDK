#ifndef __BARCODE_STRUCTS__
#define __BARCODE_STRUCTS__

#include "CommonConfig.h"

NS_DBR_NATIVE_BEG

#pragma pack(push)
#pragma pack(1)
/*structs*/
typedef struct tagBarcodeResult
{
	__int64 llFormat;
	char* pBarcodeData;
	int iBarcodeDataLength;
	int iLeft;
	int iTop;
	int iWidth;
	int iHeight;
	int iX1;
	int iY1;
	int iX2;
	int iY2;
	int iX3;
	int iY3;
	int iX4;
	int iY4;
	int iPageNum;
} BarcodeResult, *pBarcodeResult;

typedef struct tagBarcodeResultArray
{
	int iBarcodeCount;
	pBarcodeResult *ppBarcodes;
} BarcodeResultArray, *pBarcodeResultArray;

typedef struct tagReaderOptions
{
	int iMaxBarcodesNumPerPage;
	__int64 llBarcodeFormat;
} ReaderOptions, *pReaderOptions;

#pragma pack(pop)

NS_DBR_NATIVE_END

#endif