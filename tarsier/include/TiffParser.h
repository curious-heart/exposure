#ifndef TIFFPARSER_H
#define TIFFPARSER_H

#include "include/IRayImage.h"
typedef int * (*CreateImageMng)();
typedef void (*ReleaseImageMng)(int * pMng);
typedef void (*TiffClose)(int * pMng);
typedef int (*TiffGetImageCount)(int * pMng);
typedef bool (*TiffOpen)(int * pMng, const char * path, int flag);
typedef void (*TiffReadImage)(int * pMng, IRayImage * pImg, int index);
typedef void (*TiffWriteImage)(int * pMng, IRayImage * pImg);

#endif // TIFFPARSER_H
