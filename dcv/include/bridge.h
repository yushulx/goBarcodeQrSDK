#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

// Return codes
#define DBR_OK 0
#define DBR_ERROR -1

    // Structure to hold barcode result
    typedef struct
    {
        char *text;
        char *format;
        int x1, y1, x2, y2, x3, y3, x4, y4;
    } BarcodeResultC;

    // Structure to hold array of results
    typedef struct
    {
        BarcodeResultC *results;
        int count;
    } BarcodeResultArrayC;

    // Core functions
    int DBR_InitLicense(const char *license, char *errorMsg, int errorMsgBufferLen);
    void *DBR_CreateInstance();
    void DBR_DestroyInstance(void *instance);
    const char *DBR_GetVersion();

    // Settings functions
    int DBR_InitRuntimeSettingsWithString(void *instance, const char *content, int conflictMode, char *errorMsg, int errorMsgBufferLen);
    int DBR_InitRuntimeSettingsWithFile(void *instance, const char *fileName, int conflictMode, char *errorMsg, int errorMsgBufferLen);

    // Decoding functions
    int DBR_DecodeFile(void *instance, const char *fileName);
    int DBR_DecodeFileInMemory(void *instance, const unsigned char *buffer, int bufferLen);
    int DBR_GetAllTextResults(void *instance, BarcodeResultArrayC **results);
    void DBR_FreeTextResults(BarcodeResultArrayC **results);

#ifdef __cplusplus
}
#endif
