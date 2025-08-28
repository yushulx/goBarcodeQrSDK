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
    CDecodedBarcodesResult *lastResult;
    std::vector<BarcodeResultC> results;
    std::vector<std::string> textStrings;
    std::vector<std::string> formatStrings;

    BarcodeReaderInstance() : router(nullptr), lastResult(nullptr) {}
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
            if (inst->lastResult)
            {
                inst->lastResult->Release();
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
        if (inst->lastResult)
        {
            inst->lastResult->Release();
            inst->lastResult = nullptr;
        }

        try
        {
            CCapturedResult *result = inst->router->Capture(fileName, CPresetTemplate::PT_READ_BARCODES);
            if (!result)
                return -1;

            if (result->GetErrorCode() != EC_OK)
            {
                int errorCode = result->GetErrorCode();
                result->Release();
                return errorCode;
            }

            inst->lastResult = result->GetDecodedBarcodesResult();
            if (inst->lastResult)
            {
                inst->lastResult->Retain(); // Keep reference
            }

            result->Release();
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

        // Release previous result
        if (inst->lastResult)
        {
            inst->lastResult->Release();
            inst->lastResult = nullptr;
        }

        try
        {
            CCapturedResult *result = inst->router->Capture(buffer, bufferLen, CPresetTemplate::PT_READ_BARCODES);

            if (!result)
                return -1;

            if (result->GetErrorCode() != EC_OK)
            {
                int errorCode = result->GetErrorCode();
                result->Release();
                return errorCode;
            }

            inst->lastResult = result->GetDecodedBarcodesResult();
            if (inst->lastResult)
            {
                inst->lastResult->Retain(); // Keep reference
            }

            result->Release();
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
        if (!inst->lastResult)
        {
            *results = nullptr;
            return 0;
        }

        try
        {
            int count = inst->lastResult->GetItemsCount();
            if (count == 0)
            {
                *results = nullptr;
                return 0;
            }

            // Clear previous results
            inst->results.clear();
            inst->textStrings.clear();
            inst->formatStrings.clear();

            // Reserve space
            inst->results.reserve(count);
            inst->textStrings.reserve(count);
            inst->formatStrings.reserve(count);

            // Convert results
            for (int i = 0; i < count; i++)
            {
                const CBarcodeResultItem *item = inst->lastResult->GetItem(i);
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

                    inst->results.push_back(result);
                }
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
