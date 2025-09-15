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

    int DBR_DecodeFile(void *instance, const char *fileName, BarcodeResultArrayC **results, char *errorMsg, int errorMsgSize)
    {
        if (!instance || !fileName || !results)
        {
            if (errorMsg && errorMsgSize > 0)
            {
                strncpy(errorMsg, "Invalid instance, filename, or results pointer", errorMsgSize - 1);
                errorMsg[errorMsgSize - 1] = '\0';
            }
            return -1;
        }

        *results = nullptr;
        BarcodeReaderInstance *inst = static_cast<BarcodeReaderInstance *>(instance);
        if (!inst->router)
        {
            if (errorMsg && errorMsgSize > 0)
            {
                strncpy(errorMsg, "Router not initialized", errorMsgSize - 1);
                errorMsg[errorMsgSize - 1] = '\0';
            }
            return -1;
        }

        // Release previous result
        if (inst->lastResultArray)
        {
            inst->lastResultArray->Release();
            inst->lastResultArray = nullptr;
        }

        // Clear previous results
        inst->results.clear();
        inst->textStrings.clear();
        inst->formatStrings.clear();

        try
        {
            CCapturedResultArray *captureResultArray = inst->router->CaptureMultiPages(fileName, CPresetTemplate::PT_READ_BARCODES);
            if (!captureResultArray)
            {
                if (errorMsg && errorMsgSize > 0)
                {
                    strncpy(errorMsg, "Failed to capture from file", errorMsgSize - 1);
                    errorMsg[errorMsgSize - 1] = '\0';
                }
                return -1;
            }

            std::string warningMessage = "";
            int totalBarcodes = 0;
            int pageCount = captureResultArray->GetResultsCount();

            // First pass: count total barcodes to reserve vector capacity
            int estimatedBarcodeCount = 0;
            for (int pageIndex = 0; pageIndex < pageCount; pageIndex++)
            {
                const CCapturedResult *pageResult = captureResultArray->GetResult(pageIndex);
                if (!pageResult)
                    continue;

                CDecodedBarcodesResult *barcodeResult = pageResult->GetDecodedBarcodesResult();
                if (barcodeResult && barcodeResult->GetItemsCount() > 0)
                {
                    estimatedBarcodeCount += barcodeResult->GetItemsCount();
                }
            }

            // Reserve capacity to prevent reallocation
            inst->textStrings.reserve(estimatedBarcodeCount);
            inst->formatStrings.reserve(estimatedBarcodeCount);
            inst->results.reserve(estimatedBarcodeCount);

            // Second pass: process barcodes
            for (int pageIndex = 0; pageIndex < pageCount; pageIndex++)
            {
                const CCapturedResult *pageResult = captureResultArray->GetResult(pageIndex);
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

                // Collect warning messages but don't stop processing
                if (pageResult->GetErrorCode() != EC_OK)
                {
                    if (!warningMessage.empty())
                        warningMessage += "; ";
                    warningMessage += "Page " + std::to_string(pageId) + ": " + pageResult->GetErrorString();
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
                            const char *textPtr = item->GetText();
                            const char *formatPtr = item->GetFormatString();

                            // Store both strings first to avoid vector reallocation issues
                            std::string textStr = textPtr ? std::string(textPtr) : "";
                            std::string formatStr = formatPtr ? std::string(formatPtr) : "";

                            // Reserve space to avoid reallocation during this loop
                            size_t currentTextIndex = inst->textStrings.size();
                            size_t currentFormatIndex = inst->formatStrings.size();

                            inst->textStrings.push_back(textStr);
                            inst->formatStrings.push_back(formatStr);

                            // Use indexed access to avoid invalidation
                            result.text = const_cast<char *>(inst->textStrings[currentTextIndex].c_str());
                            result.format = const_cast<char *>(inst->formatStrings[currentFormatIndex].c_str());

                            // Get location points
                            CQuadrilateral location = item->GetLocation();
                            result.x1 = location.points[0][0];
                            result.y1 = location.points[0][1];
                            result.x2 = location.points[1][0];
                            result.y2 = location.points[1][1];
                            result.x3 = location.points[2][0];
                            result.y3 = location.points[2][1];
                            result.x4 = location.points[3][0];
                            result.y4 = location.points[3][1];

                            // Set page ID
                            result.pageId = pageId;

                            inst->results.push_back(result);
                            totalBarcodes++;
                        }
                    }
                }
            }

            captureResultArray->Release();

            // Set warning message if any, even if we found barcodes
            if (errorMsg && errorMsgSize > 0)
            {
                if (!warningMessage.empty())
                {
                    strncpy(errorMsg, warningMessage.c_str(), errorMsgSize - 1);
                    errorMsg[errorMsgSize - 1] = '\0';
                }
                else
                {
                    errorMsg[0] = '\0'; // Clear error message on success
                }
            }

            if (totalBarcodes > 0)
            {
                // Create result array with dynamic allocation
                BarcodeResultArrayC *resultArray = new BarcodeResultArrayC();
                resultArray->results = inst->results.data();
                resultArray->count = inst->results.size();
                *results = resultArray;
            }

            return totalBarcodes > 0 ? 0 : (warningMessage.empty() ? 0 : -1);
        }
        catch (const std::exception &e)
        {
            if (errorMsg && errorMsgSize > 0)
            {
                strncpy(errorMsg, e.what(), errorMsgSize - 1);
                errorMsg[errorMsgSize - 1] = '\0';
            }
            return -1;
        }
        catch (...)
        {
            if (errorMsg && errorMsgSize > 0)
            {
                strncpy(errorMsg, "Unknown error occurred", errorMsgSize - 1);
                errorMsg[errorMsgSize - 1] = '\0';
            }
            return -1;
        }
    }

    int DBR_DecodeFileInMemory(void *instance, const unsigned char *buffer, int bufferLen, BarcodeResultArrayC **results, char *errorMsg, int errorMsgSize)
    {
        if (!instance || !buffer || bufferLen <= 0 || !results)
        {
            if (errorMsg && errorMsgSize > 0)
            {
                strncpy(errorMsg, "Invalid instance, buffer, buffer length, or results pointer", errorMsgSize - 1);
                errorMsg[errorMsgSize - 1] = '\0';
            }
            return -1;
        }

        *results = nullptr;
        BarcodeReaderInstance *inst = static_cast<BarcodeReaderInstance *>(instance);
        if (!inst->router)
        {
            if (errorMsg && errorMsgSize > 0)
            {
                strncpy(errorMsg, "Router not initialized", errorMsgSize - 1);
                errorMsg[errorMsgSize - 1] = '\0';
            }
            return -1;
        }

        // Release previous results
        if (inst->lastResultArray)
        {
            inst->lastResultArray->Release();
            inst->lastResultArray = nullptr;
        }

        // Clear previous results
        inst->results.clear();
        inst->textStrings.clear();
        inst->formatStrings.clear();

        try
        {
            CCapturedResultArray *captureResultArray = inst->router->CaptureMultiPages(buffer, bufferLen, CPresetTemplate::PT_READ_BARCODES);
            if (!captureResultArray)
            {
                if (errorMsg && errorMsgSize > 0)
                {
                    strncpy(errorMsg, "Failed to capture from memory", errorMsgSize - 1);
                    errorMsg[errorMsgSize - 1] = '\0';
                }
                return -1;
            }

            std::string warningMessage = "";
            int totalBarcodes = 0;
            int pageCount = captureResultArray->GetResultsCount();

            // First pass: count total barcodes to reserve vector capacity
            int estimatedBarcodeCount = 0;
            for (int pageIndex = 0; pageIndex < pageCount; pageIndex++)
            {
                const CCapturedResult *pageResult = captureResultArray->GetResult(pageIndex);
                if (!pageResult)
                    continue;

                CDecodedBarcodesResult *barcodeResult = pageResult->GetDecodedBarcodesResult();
                if (barcodeResult && barcodeResult->GetItemsCount() > 0)
                {
                    estimatedBarcodeCount += barcodeResult->GetItemsCount();
                }
            }

            // Reserve capacity to prevent reallocation
            inst->textStrings.reserve(estimatedBarcodeCount);
            inst->formatStrings.reserve(estimatedBarcodeCount);
            inst->results.reserve(estimatedBarcodeCount);

            // Second pass: process barcodes
            for (int pageIndex = 0; pageIndex < pageCount; pageIndex++)
            {
                const CCapturedResult *pageResult = captureResultArray->GetResult(pageIndex);
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

                // Collect warning messages but don't stop processing
                if (pageResult->GetErrorCode() != EC_OK)
                {
                    if (!warningMessage.empty())
                        warningMessage += "; ";
                    warningMessage += "Page " + std::to_string(pageId) + ": " + pageResult->GetErrorString();
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
                            const char *textPtr = item->GetText();
                            const char *formatPtr = item->GetFormatString();

                            // Store both strings first to avoid vector reallocation issues
                            std::string textStr = textPtr ? std::string(textPtr) : "";
                            std::string formatStr = formatPtr ? std::string(formatPtr) : "";

                            // Reserve space to avoid reallocation during this loop
                            size_t currentTextIndex = inst->textStrings.size();
                            size_t currentFormatIndex = inst->formatStrings.size();

                            inst->textStrings.push_back(textStr);
                            inst->formatStrings.push_back(formatStr);

                            // Use indexed access to avoid invalidation
                            result.text = const_cast<char *>(inst->textStrings[currentTextIndex].c_str());
                            result.format = const_cast<char *>(inst->formatStrings[currentFormatIndex].c_str());

                            // Get location points
                            CQuadrilateral location = item->GetLocation();
                            result.x1 = location.points[0][0];
                            result.y1 = location.points[0][1];
                            result.x2 = location.points[1][0];
                            result.y2 = location.points[1][1];
                            result.x3 = location.points[2][0];
                            result.y3 = location.points[2][1];
                            result.x4 = location.points[3][0];
                            result.y4 = location.points[3][1];

                            // Set page ID
                            result.pageId = pageId;

                            inst->results.push_back(result);
                            totalBarcodes++;
                        }
                    }
                }
            }

            captureResultArray->Release();

            // Set warning message if any, even if we found barcodes
            if (errorMsg && errorMsgSize > 0)
            {
                if (!warningMessage.empty())
                {
                    strncpy(errorMsg, warningMessage.c_str(), errorMsgSize - 1);
                    errorMsg[errorMsgSize - 1] = '\0';
                }
                else
                {
                    errorMsg[0] = '\0'; // Clear error message on success
                }
            }

            if (totalBarcodes > 0)
            {
                // Create result array with dynamic allocation
                BarcodeResultArrayC *resultArray = new BarcodeResultArrayC();
                resultArray->results = inst->results.data();
                resultArray->count = inst->results.size();
                *results = resultArray;
            }

            return totalBarcodes > 0 ? 0 : (warningMessage.empty() ? 0 : -1);
        }
        catch (const std::exception &e)
        {
            if (errorMsg && errorMsgSize > 0)
            {
                strncpy(errorMsg, e.what(), errorMsgSize - 1);
                errorMsg[errorMsgSize - 1] = '\0';
            }
            return -1;
        }
        catch (...)
        {
            if (errorMsg && errorMsgSize > 0)
            {
                strncpy(errorMsg, "Unknown error occurred", errorMsgSize - 1);
                errorMsg[errorMsgSize - 1] = '\0';
            }
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
