#ifndef __BARCODE_FORMAT__
#define __BARCODE_FORMAT__

#include "CommonConfig.h"

const __int64 OneD = 0x3FFL;

const __int64 CODE_39 = 0x1L;
const __int64 CODE_128 = 0x2L;
const __int64 CODE_93 = 0x4L;
const __int64 CODABAR = 0x8L;
const __int64 ITF = 0x10L;
const __int64 EAN_13 = 0x20L;
const __int64 EAN_8 = 0x40L;
const __int64 UPC_A = 0x80L;
const __int64 UPC_E = 0x100L;
const __int64 INDUSTRIAL_25 = 0x200L;

const __int64 PDF417 =  0x2000000L;
const __int64 DATAMATRIX = 0x8000000L;
const __int64 QR_CODE = 0x4000000L;

#endif