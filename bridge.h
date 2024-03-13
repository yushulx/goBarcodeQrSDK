#include <stdio.h>
#include <stdlib.h>
#include "DynamsoftBarcodeReader.h"

TextResult *getTextResultPointer(TextResultArray *resultArray, int offset);

LocalizationResult *getLocalizationPointer(TextResult *result);

const char *getText(TextResult *result);

const char *getFormatString(TextResult *result);