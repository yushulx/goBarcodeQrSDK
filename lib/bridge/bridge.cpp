#include <iostream>
#include <string>
#include <cstring>
#include "bridge.h"
#include "DynamsoftCaptureVisionRouter.h"

using namespace std;
using namespace dynamsoft::license;
using namespace dynamsoft::cvr;
using namespace dynamsoft::dbr;

C_API int DBR_InitLicense(const char *pLicense, char errorMsgBuffer[], const int errorMsgBufferLen)
{
    int ret = CLicenseManager::InitLicense(pLicense, errorMsgBuffer, 512);
    if (ret != 0)
    {
        cout << "Error: " << errorMsgBuffer << endl;
    }
    return ret;
}

C_API void *DBR_CreateInstance()
{
    BarcodeReader *barcodeReader = new BarcodeReader;
    CCaptureVisionRouter *cvr = new CCaptureVisionRouter;
    barcodeReader->cvr = cvr;
    barcodeReader->result = NULL;
    return (void *)barcodeReader;
}

C_API void DBR_DestroyInstance(void *barcodeReader)
{
    if (barcodeReader != NULL)
    {
        BarcodeReader *reader = (BarcodeReader *)barcodeReader;
        if (reader->cvr != NULL)
        {
            delete (CCaptureVisionRouter *)reader->cvr;
            reader->cvr = NULL;
        }

        if (reader->result != NULL)
        {
            ((CCapturedResult *)reader->result)->Release();
            reader->result = NULL;
        }

        delete reader;
        barcodeReader = NULL;
    }
}

C_API int DBR_DecodeFile(void *barcodeReader, const char *pFileName, const char *pTemplateName)
{
    BarcodeReader *reader = (BarcodeReader *)barcodeReader;
    if (!reader || !reader->cvr)
        return -1;

    CCapturedResult *result = ((CCaptureVisionRouter *)reader->cvr)->Capture(pFileName);
    int errorCode = result->GetErrorCode();
    if (result->GetErrorCode() != 0)
    {
        cout << "Error: " << result->GetErrorCode() << "," << result->GetErrorString() << endl;
    }

    reader->result = result;

    return errorCode;
}

C_API const char *DBR_GetVersion()
{
    // return "1.0.2";
    return CBarcodeReaderModule::GetVersion();
}

C_API int DBR_GetAllTextResults(void *barcodeReader, TextResultArray **pResults)
{
    BarcodeReader *reader = (BarcodeReader *)barcodeReader;
    if (!reader || !reader->cvr || !reader->result)
        return -1;

    CCapturedResult *result = (CCapturedResult *)reader->result;

    int capturedResultItemCount = result->GetItemsCount();
    if (capturedResultItemCount == 0)
        return -1;

    TextResultArray *textResults = (TextResultArray *)calloc(1, sizeof(TextResultArray));
    textResults->resultsCount = capturedResultItemCount;
    textResults->results = (TextResult *)calloc(capturedResultItemCount, sizeof(TextResult));
    *pResults = textResults;

    for (int j = 0; j < capturedResultItemCount; j++)
    {
        const CCapturedResultItem *capturedResultItem = result->GetItem(j);
        CapturedResultItemType type = capturedResultItem->GetType();
        if (type == CapturedResultItemType::CRIT_BARCODE)
        {
            const CBarcodeResultItem *barcodeResultItem = dynamic_cast<const CBarcodeResultItem *>(capturedResultItem);
            // cout << "Result " << j + 1 << endl;
            // cout << "Barcode Format: " << barcodeResultItem->GetFormatString() << endl;
            // cout << "Barcode Text: " << barcodeResultItem->GetText() << endl;
            char *barcodeFormatString = (char *)barcodeResultItem->GetFormatString();
            char *barcodeText = (char *)barcodeResultItem->GetText();
            textResults->results[j].barcodeFormatString = (char *)malloc(strlen(barcodeFormatString) + 1);
            strcpy(textResults->results[j].barcodeFormatString, barcodeFormatString);
            textResults->results[j].barcodeText = (char *)malloc(strlen(barcodeText) + 1);
            strcpy(textResults->results[j].barcodeText, barcodeText);

            CPoint *points = barcodeResultItem->GetLocation().points;
            textResults->results[j].localizationResult = (LocalizationResult *)malloc(sizeof(LocalizationResult));
            textResults->results[j].localizationResult->x1 = points[0][0];
            textResults->results[j].localizationResult->y1 = points[0][1];
            textResults->results[j].localizationResult->x2 = points[1][0];
            textResults->results[j].localizationResult->y2 = points[1][1];
            textResults->results[j].localizationResult->x3 = points[2][0];
            textResults->results[j].localizationResult->y3 = points[2][1];
            textResults->results[j].localizationResult->x4 = points[3][0];
            textResults->results[j].localizationResult->y4 = points[3][1];
        }
    }

    result->Release();
    reader->result = NULL;

    return 0;
}

C_API void DBR_FreeTextResults(TextResultArray **pResults)
{
    if (pResults)
    {
        if (*pResults)
        {
            if ((*pResults)->results)
            {
                for (int i = 0; i < (*pResults)->resultsCount; i++)
                {
                    if ((*pResults)->results[i].barcodeFormatString)
                    {
                        free((*pResults)->results[i].barcodeFormatString);
                    }
                    if ((*pResults)->results[i].barcodeText)
                    {
                        free((*pResults)->results[i].barcodeText);
                    }

                    if ((*pResults)->results[i].localizationResult)
                    {
                        free((*pResults)->results[i].localizationResult);
                    }
                }
                free((*pResults)->results);
            }
        }
    }
}

C_API TextResult *getTextResultPointer(TextResultArray *resultArray, int offset)
{
    if (resultArray == NULL || offset < 0 || offset >= resultArray->resultsCount)
        return NULL;

    return &resultArray->results[offset];
}

C_API LocalizationResult *getLocalizationPointer(TextResult *result)
{
    return result->localizationResult;
}

C_API const char *getText(TextResult *result)
{
    return result->barcodeText;
}

C_API const char *getFormatString(TextResult *result)
{
    return result->barcodeFormatString;
}

C_API int DBR_InitRuntimeSettingsWithString(void *barcodeReader, const char *content, const ConflictMode conflictMode, char errorMsgBuffer[], const int errorMsgBufferLen)
{
    BarcodeReader *reader = (BarcodeReader *)barcodeReader;
    if (!reader || !reader->cvr)
        return -1;

    int ret = ((CCaptureVisionRouter *)reader->cvr)->InitSettings(content, errorMsgBuffer, errorMsgBufferLen);

    if (ret != 0)
    {
        cout << "Error: " << errorMsgBuffer << endl;
    }

    return ret;
}

C_API int DBR_InitRuntimeSettingsWithFile(void *barcodeReader, const char *pFilePath, const ConflictMode conflictMode, char errorMsgBuffer[], const int errorMsgBufferLen)
{
    BarcodeReader *reader = (BarcodeReader *)barcodeReader;
    if (!reader || !reader->cvr)
        return -1;

    int ret = ((CCaptureVisionRouter *)reader->cvr)->InitSettingsFromFile(pFilePath, errorMsgBuffer, errorMsgBufferLen);

    if (ret != 0)
    {
        cout << "Error: " << errorMsgBuffer << endl;
    }

    return ret;
}