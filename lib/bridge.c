#include <stdio.h>
#include <stdlib.h>
#include "DynamsoftBarcodeReader.h"

TextResult *getTextResultPointer(TextResultArray *resultArray, int offset) {
    return resultArray->results[offset];
}

LocalizationResult *getLocalizationPointer(TextResult *result) {
    return result->localizationResult;
}

const char *getText(TextResult *result) {
    return result->barcodeText;
}

const char *getFormatString(TextResult *result) {
    return result->barcodeFormatString;
}