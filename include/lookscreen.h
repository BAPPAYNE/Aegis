#ifndef LOOK_SCREEN_H
#define LOOK_SCREEN_H

#include <windows.h>
#include <wincodec.h>

// returns a PNG buffer and its size; caller must free() it
BYTE* CaptureScreen(DWORD *outSize);

// optional helpers, if you ever need them directly
HBITMAP CaptureScreenToHBITMAP(int sx, int sy, int sw, int sh);
BYTE* SaveHBITMAPToPNGBuffer(HBITMAP hBitmap, DWORD* outSize);
int Save_Buffer_To_FIle(const BYTE* buffer, DWORD size, const char* filename);

#endif // LOOK_SCREEN_H
