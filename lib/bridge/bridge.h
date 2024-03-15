#ifndef C_BRIDGING_H
#define C_BRIDGING_H

#if defined(_WIN32) || defined(_WIN64)
#define C_API __declspec(dllexport)
#else
#define C_API __attribute__((visibility("default")))
#endif

typedef struct
{
    int x1;
    int y1;
    int x2;
    int y2;
    int x3;
    int y3;
    int x4;
    int y4;

} LocalizationResult;

typedef struct
{

    /**Barcode type in BarcodeFormat group 1 as string */
    char *barcodeFormatString;

    /**The barcode text, ends by '\0' */
    char *barcodeText;

    LocalizationResult *localizationResult;

    /**Reserved memory for the struct. The length of this array indicates the size of the memory reserved for this struct. */
    char reserved[44];
} TextResult;

typedef struct
{
    /**The total count of text result */
    int resultsCount;

    /**The text result array */
    TextResult *results;
} TextResultArray;

typedef enum ConflictMode
{
    /**Ignores new settings and inherits the previous settings. */
    CM_IGNORE = 1,

    /**Overwrites the old settings with new settings. */
    CM_OVERWRITE = 2

} ConflictMode;

typedef struct
{
    void *cvr;
    void *result;
} BarcodeReader;

#ifdef __cplusplus
extern "C"
{
#endif
    // Create dbr9.x-like API for dbr10.x
    C_API int DBR_InitLicense(const char *pLicense, char errorMsgBuffer[], const int errorMsgBufferLen);
    C_API int DBR_DecodeFile(void *barcodeReader, const char *pFileName, const char *pTemplateName);
    C_API void *DBR_CreateInstance();
    C_API void DBR_DestroyInstance(void *barcodeReader);
    C_API const char *DBR_GetVersion();
    C_API int DBR_GetAllTextResults(void *barcodeReader, TextResultArray **pResults);
    C_API void DBR_FreeTextResults(TextResultArray **pResults);
    C_API int DBR_InitRuntimeSettingsWithString(void *barcodeReader, const char *content, const ConflictMode conflictMode, char errorMsgBuffer[], const int errorMsgBufferLen);
    C_API int DBR_InitRuntimeSettingsWithFile(void *barcodeReader, const char *pFilePath, const ConflictMode conflictMode, char errorMsgBuffer[], const int errorMsgBufferLen);

    // The interop functions for Go
    C_API TextResult *getTextResultPointer(TextResultArray *resultArray, int offset);
    C_API LocalizationResult *getLocalizationPointer(TextResult *result);
    C_API const char *getText(TextResult *result);
    C_API const char *getFormatString(TextResult *result);
#ifdef __cplusplus
}
#endif

#endif