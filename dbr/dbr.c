#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include "include/If_DBR.h"

__int64 GetFormat(int iIndex)
{
	__int64 llFormat = 0;

	switch(iIndex)
	{
	case 1:
		llFormat = OneD | QR_CODE | PDF417 |DATAMATRIX;
		break;
	case 2:
		llFormat = OneD;
		break;
	case 3:
		llFormat = QR_CODE;
		break;
	case 4:
		llFormat = CODE_39;
		break;
	case 5:
		llFormat = CODE_128;
		break;
	case 6:
		llFormat = CODE_93;
		break;
	case 7:
		llFormat = CODABAR;
		break;
	case 8:
		llFormat = ITF;
		break;
	case 9:
		llFormat = INDUSTRIAL_25;
		break;
	case 10:
		llFormat = EAN_13;
		break;
	case 11:
		llFormat = EAN_8;
		break;
	case 12:
		llFormat = UPC_A;
		break;
	case 13:
		llFormat = UPC_E;
		break;
	case 14:
		llFormat = PDF417;
		break;
	case 15:
		llFormat = DATAMATRIX;
		break;
	default:
		break;
	}

	return llFormat;
}

const char * GetFormatStr(__int64 format)
{
	if (format == CODE_39)
		return "CODE_39";
	if (format == CODE_128)
		return "CODE_128";
	if (format == CODE_93)
		return "CODE_93";
	if (format == CODABAR)
		return "CODABAR";
	if (format == ITF)
		return "ITF";
	if (format == UPC_A)
		return "UPC_A";
	if (format == UPC_E)
		return "UPC_E";
	if (format == EAN_13)
		return "EAN_13";
	if (format == EAN_8)
		return "EAN_8";
	if (format == INDUSTRIAL_25)
		return "INDUSTRIAL_25";
	if (format == QR_CODE)
		return "QR_CODE";
	if (format == PDF417)
		return "PDF417";
	if (format == DATAMATRIX)
		return "DATAMATRIX";

	return "UNKNOWN";
}

int decode_file(const char *pszImageFile)
{
	if (!pszImageFile)
	{
		return -1;
	}

  // const char *pszImageFile = "f:\\AllSupportedBarcodeTypes.tif";
	__int64 llFormat = (OneD | QR_CODE | PDF417 | DATAMATRIX);
	char pszBuffer[512] = {0};

	int iMaxCount = 0x7FFFFFFF;
	int iIndex = 0;
	ReaderOptions ro = {0};
	pBarcodeResultArray paryResult = NULL;
	int iRet = -1;
	char * pszTemp = NULL;
	char * pszTemp1 = NULL;
	unsigned __int64 ullTimeBegin = 0;
	unsigned __int64 ullTimeEnd = 0;
	size_t iLen;
	FILE* fp = NULL;
	int iExitFlag = 0;

	DBR_InitLicense("38B9B94D8B0E2B41DB1CC80A58946567");

	// Read barcode
	ullTimeBegin = GetTickCount();
	ro.llBarcodeFormat = llFormat;
	ro.iMaxBarcodesNumPerPage = iMaxCount;
	iRet = DBR_DecodeFile(pszImageFile, &ro, &paryResult);
	ullTimeEnd = GetTickCount();

	// Output barcode result
	pszTemp = (char*)malloc(4096);
	if (iRet != DBR_OK)
	{
		sprintf(pszTemp, "Failed to read barcode: %s\r\n", DBR_GetErrorString(iRet));
		printf(pszTemp);
		free(pszTemp);
		return 0;
	}

	if (paryResult->iBarcodeCount == 0)
	{
		sprintf(pszTemp, "No barcode found. Total time spent: %.3f seconds.\r\n", ((float)(ullTimeEnd - ullTimeBegin)/1000));
		printf(pszTemp);
		free(pszTemp);
		DBR_FreeBarcodeResults(&paryResult);
		return 0;
	}

	sprintf(pszTemp, "Total barcode(s) found: %d. Total time spent: %.3f seconds\r\n\r\n", paryResult->iBarcodeCount, ((float)(ullTimeEnd - ullTimeBegin)/1000));
	printf(pszTemp);
	for (iIndex = 0; iIndex < paryResult->iBarcodeCount; iIndex++)
	{
		sprintf(pszTemp, "Barcode %d:\r\n", iIndex + 1);
		printf(pszTemp);
		sprintf(pszTemp, "    Page: %d\r\n", paryResult->ppBarcodes[iIndex]->iPageNum);
		printf(pszTemp);
		sprintf(pszTemp, "    Type: %s\r\n", GetFormatStr(paryResult->ppBarcodes[iIndex]->llFormat));
		printf(pszTemp);
		pszTemp1 = (char*)malloc(paryResult->ppBarcodes[iIndex]->iBarcodeDataLength + 1);
		memset(pszTemp1, 0, paryResult->ppBarcodes[iIndex]->iBarcodeDataLength + 1);
		memcpy(pszTemp1, paryResult->ppBarcodes[iIndex]->pBarcodeData, paryResult->ppBarcodes[iIndex]->iBarcodeDataLength);
		sprintf(pszTemp, "    Value: %s\r\n", pszTemp1);
		printf(pszTemp);
		free(pszTemp1);
		sprintf(pszTemp, "    Region: {Left: %d, Top: %d, Width: %d, Height: %d}\r\n\r\n",
			paryResult->ppBarcodes[iIndex]->iLeft, paryResult->ppBarcodes[iIndex]->iTop,
			paryResult->ppBarcodes[iIndex]->iWidth, paryResult->ppBarcodes[iIndex]->iHeight);
		printf(pszTemp);
	}

	free(pszTemp);
	DBR_FreeBarcodeResults(&paryResult);

	return 0;
}
