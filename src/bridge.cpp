#include "../dcv/include/bridge.h"
#include "DynamsoftCaptureVisionRouter.h"
#include "DynamsoftLicense.h"
#include "DynamsoftBarcodeReader.h"
#include "DynamsoftUtility.h"
#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <fstream>
#include <cstdio>

using namespace dynamsoft::license;
using namespace dynamsoft::cvr;
using namespace dynamsoft::dbr;
using namespace dynamsoft::basic_structures;

// Structure to hold router instance and results
struct BarcodeReaderInstance
{
    CCaptureVisionRouter *router;
    CCapturedResultArray *lastResultArray; // For multi-page results
    std::vector<BarcodeResultC> results;
    std::vector<std::string> textStrings;
    std::vector<std::string> formatStrings;

    BarcodeReaderInstance() : router(nullptr), lastResultArray(nullptr) {}
};

extern "C"
{
    int DBR_InitLicense(const char *license, char *errorMsg, int errorMsgSize)
    {
        if (!license)
            return -1;

        int ret = CLicenseManager::InitLicense(license, errorMsg, errorMsgSize);
        return ret;
    }

    void *DBR_CreateInstance()
    {
        try
        {
            BarcodeReaderInstance *instance = new BarcodeReaderInstance();
            instance->router = new CCaptureVisionRouter();
            return instance;
        }
        catch (...)
        {
            return nullptr;
        }
    }

    void DBR_DestroyInstance(void *instance)
    {
        if (instance)
        {
            BarcodeReaderInstance *inst = static_cast<BarcodeReaderInstance *>(instance);
            if (inst->lastResultArray)
            {
                inst->lastResultArray->Release();
            }
            if (inst->router)
            {
                delete inst->router;
            }
            delete inst;
        }
    }

    const char *DBR_GetVersion()
    {
        return CBarcodeReaderModule::GetVersion();
    }

    int DBR_InitRuntimeSettingsWithString(void *instance, const char *content, int conflictMode, char *errorMsg, int errorMsgSize)
    {
        if (!instance || !content)
            return -1;

        BarcodeReaderInstance *inst = static_cast<BarcodeReaderInstance *>(instance);
        if (!inst->router)
            return -1;

        int ret = inst->router->InitSettings(content, errorMsg, errorMsgSize);
        return ret;
    }

    int DBR_InitRuntimeSettingsWithFile(void *instance, const char *fileName, int conflictMode, char *errorMsg, int errorMsgSize)
    {
        if (!instance || !fileName)
            return -1;

        BarcodeReaderInstance *inst = static_cast<BarcodeReaderInstance *>(instance);
        if (!inst->router)
            return -1;

        int ret = inst->router->InitSettingsFromFile(fileName, errorMsg, errorMsgSize);
        return ret;
    }

    int DBR_DecodeFile(void *instance, const char *fileName)
    {
        if (!instance || !fileName)
            return -1;

        BarcodeReaderInstance *inst = static_cast<BarcodeReaderInstance *>(instance);
        if (!inst->router)
            return -1;

        // Release previous result
        if (inst->lastResultArray)
        {
            inst->lastResultArray->Release();
            inst->lastResultArray = nullptr;
        }

        try
        {
            CCapturedResultArray *captureResultArray = inst->router->CaptureMultiPages(fileName, CPresetTemplate::PT_READ_BARCODES);
            if (!captureResultArray)
                return -1;

            // Store the result array for later processing
            inst->lastResultArray = captureResultArray;
            inst->lastResultArray->Retain(); // Keep reference

            return 0;
        }
        catch (...)
        {
            return -1;
        }
    }

    int DBR_DecodeFileInMemory(void *instance, const unsigned char *buffer, int bufferLen)
    {
        if (!instance || !buffer || bufferLen <= 0)
            return -1;

        BarcodeReaderInstance *inst = static_cast<BarcodeReaderInstance *>(instance);
        if (!inst->router)
            return -1;

        // Release previous results
        if (inst->lastResultArray)
        {
            inst->lastResultArray->Release();
            inst->lastResultArray = nullptr;
        }

        try
        {
            CCapturedResultArray *captureResultArray = inst->router->CaptureMultiPages(buffer, bufferLen, CPresetTemplate::PT_READ_BARCODES);

            if (!captureResultArray)
                return -1;

            // Store the result array for later processing
            inst->lastResultArray = captureResultArray;
            inst->lastResultArray->Retain(); // Keep reference

            return 0;
        }
        catch (...)
        {
            return -1;
        }
    }

    int DBR_GetAllTextResults(void *instance, BarcodeResultArrayC **results)
    {
        if (!instance || !results)
            return -1;

        BarcodeReaderInstance *inst = static_cast<BarcodeReaderInstance *>(instance);

        // Clear previous results
        inst->results.clear();
        inst->textStrings.clear();
        inst->formatStrings.clear();

        try
        {
            int totalBarcodes = 0;

            // Handle multi-page results (from CaptureMultiPages)
            if (inst->lastResultArray)
            {
                int pageCount = inst->lastResultArray->GetResultsCount();

                for (int pageIndex = 0; pageIndex < pageCount; pageIndex++)
                {
                    CCapturedResult *pageResult = (CCapturedResult *)inst->lastResultArray->GetResult(pageIndex);
                    if (!pageResult)
                        continue;

                    // Get page ID from image tag, fallback to page index
                    int pageId = pageIndex;
                    if (pageResult->GetOriginalImageTag())
                    {
                        int sdkPageId = pageResult->GetOriginalImageTag()->GetImageId();
                        if (sdkPageId >= 0)
                        {
                            pageId = sdkPageId;
                        }
                    }

                    if (pageResult->GetErrorCode() != EC_OK)
                    {
                        std::cout << "Error on page " << pageId << ": " << pageResult->GetErrorCode()
                                  << ", " << pageResult->GetErrorString() << std::endl;
                        continue;
                    }

                    CDecodedBarcodesResult *barcodeResult = pageResult->GetDecodedBarcodesResult();
                    if (barcodeResult && barcodeResult->GetItemsCount() > 0)
                    {
                        int barcodeCount = barcodeResult->GetItemsCount();

                        for (int j = 0; j < barcodeCount; j++)
                        {
                            const CBarcodeResultItem *item = barcodeResult->GetItem(j);
                            if (item)
                            {
                                BarcodeResultC result;

                                // Store strings in vectors to ensure they persist
                                inst->textStrings.push_back(std::string(item->GetText()));
                                inst->formatStrings.push_back(std::string(item->GetFormatString()));

                                result.text = const_cast<char *>(inst->textStrings.back().c_str());
                                result.format = const_cast<char *>(inst->formatStrings.back().c_str());

                                // Get location points
                                CPoint *points = item->GetLocation().points;
                                result.x1 = points[0][0];
                                result.y1 = points[0][1];
                                result.x2 = points[1][0];
                                result.y2 = points[1][1];
                                result.x3 = points[2][0];
                                result.y3 = points[2][1];
                                result.x4 = points[3][0];
                                result.y4 = points[3][1];

                                // Set page ID
                                result.pageId = pageId;

                                inst->results.push_back(result);
                                totalBarcodes++;
                            }
                        }
                    }
                }
            }

            if (totalBarcodes == 0)
            {
                *results = nullptr;
                return 0;
            }

            // Create result array with dynamic allocation
            BarcodeResultArrayC *resultArray = new BarcodeResultArrayC();
            resultArray->results = inst->results.data();
            resultArray->count = inst->results.size();

            *results = resultArray;
            return 0;
        }
        catch (...)
        {
            *results = nullptr;
            return -1;
        }
    }

    void DBR_FreeTextResults(BarcodeResultArrayC **results)
    {
        if (results && *results)
        {
            delete *results;
            *results = nullptr;
        }
    }
} // extern "C"
