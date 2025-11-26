#include <windows.h>
#include <wincodec.h>
#include <stdio.h>

#pragma comment(lib, "windowscodecs.lib")



// capture screen to HBITMAP
HBITMAP CaptureScreenToHBITMAP(int sx, int sy, int sw, int sh) {
    HDC hdcScreen = GetDC(NULL);
    if (!hdcScreen) return NULL;

    HDC hdcMem = CreateCompatibleDC(hdcScreen);
    if (!hdcMem) {
        ReleaseDC(NULL, hdcScreen);
        return NULL;
    }

    HBITMAP hBitmap = CreateCompatibleBitmap(hdcScreen, sw, sh);
    if (!hBitmap) {
        DeleteDC(hdcMem);
        ReleaseDC(NULL, hdcScreen);
        return NULL;
    }

    HGDIOBJ old = SelectObject(hdcMem, hBitmap);
    BitBlt(hdcMem, 0, 0, sw, sh, hdcScreen, sx, sy, SRCCOPY);
    SelectObject(hdcMem, old);

    DeleteDC(hdcMem);
    ReleaseDC(NULL, hdcScreen);

    return hBitmap;
}

// save HBITMAP to PNG in memory (returns buffer)
BYTE* SaveHBITMAPToPNGBuffer(HBITMAP hBitmap, DWORD* outSize) {
    HRESULT hr;
    IWICImagingFactory *pFactory = NULL;
    IWICBitmapEncoder *pEncoder = NULL;
    IWICBitmapFrameEncode *pFrame = NULL;
    IWICStream *pStream = NULL;
    IWICBitmap *pWICBitmap = NULL;
    IPropertyBag2 *pProps = NULL;
    HGLOBAL hGlobal = NULL;
    IStream *pMemStream = NULL;
    BYTE* buffer = NULL;
    *outSize = 0;

    CoInitialize(NULL);

    // create WIC factory
    hr = CoCreateInstance(&CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER,
                          &IID_IWICImagingFactory, (LPVOID*)&pFactory);
    if (FAILED(hr)) goto cleanup;

    // convert from HBITMAP to WIC bitmap
    hr = pFactory->lpVtbl->CreateBitmapFromHBITMAP(pFactory, hBitmap, NULL,
                                                   WICBitmapUsePremultipliedAlpha, &pWICBitmap);
    if (FAILED(hr)) goto cleanup;

    // create a memory-backed stream
    hGlobal = GlobalAlloc(GMEM_MOVEABLE, 0);
    if (!hGlobal) goto cleanup;
    CreateStreamOnHGlobal(hGlobal, TRUE, &pMemStream);

    // create encoder
    hr = pFactory->lpVtbl->CreateEncoder(pFactory, &GUID_ContainerFormatPng, NULL, &pEncoder);
    if (FAILED(hr)) goto cleanup;

    hr = pEncoder->lpVtbl->Initialize(pEncoder, pMemStream, WICBitmapEncoderNoCache);
    if (FAILED(hr)) goto cleanup;

    // create a frame
    hr = pEncoder->lpVtbl->CreateNewFrame(pEncoder, &pFrame, &pProps);
    if (FAILED(hr)) goto cleanup;

    hr = pFrame->lpVtbl->Initialize(pFrame, pProps);
    if (FAILED(hr)) goto cleanup;

    hr = pFrame->lpVtbl->WriteSource(pFrame, (IWICBitmapSource*)pWICBitmap, NULL);
    if (FAILED(hr)) goto cleanup;

    pFrame->lpVtbl->Commit(pFrame);
    pEncoder->lpVtbl->Commit(pEncoder);

    // get stream buffer
    STATSTG stats;
    LARGE_INTEGER zero = {0};
    pMemStream->lpVtbl->Seek(pMemStream, zero, STREAM_SEEK_SET, NULL);
    pMemStream->lpVtbl->Stat(pMemStream, &stats, STATFLAG_NONAME);

    *outSize = (DWORD)stats.cbSize.QuadPart;
    buffer = (BYTE*)malloc(*outSize);
    if (buffer) {
        ULONG readBytes = 0;
        pMemStream->lpVtbl->Read(pMemStream, buffer, *outSize, &readBytes);
    }

cleanup:
    if (pProps) pProps->lpVtbl->Release(pProps);
    if (pFrame) pFrame->lpVtbl->Release(pFrame);
    if (pEncoder) pEncoder->lpVtbl->Release(pEncoder);
    if (pStream) pStream->lpVtbl->Release(pStream);
    if (pWICBitmap) pWICBitmap->lpVtbl->Release(pWICBitmap);
    if (pFactory) pFactory->lpVtbl->Release(pFactory);
    if (pMemStream) pMemStream->lpVtbl->Release(pMemStream);

    CoUninitialize();
    return buffer;
}


BYTE* CaptureScreen(DWORD *outSize) {
    int width = GetSystemMetrics(SM_CXSCREEN);
    int height = GetSystemMetrics(SM_CYSCREEN);

    HBITMAP hBmp = CaptureScreenToHBITMAP(0, 0, width, height);
    if (!hBmp) return NULL;

    BYTE* pngBuffer = SaveHBITMAPToPNGBuffer(hBmp, outSize);
    DeleteObject(hBmp);
    return pngBuffer;
}


int Save_Buffer_To_FIle(const BYTE *buffer, DWORD size, const char * filename) {
    FILE *imageSaveData = fopen(filename, "wb") ;
    if (!imageSaveData) {
        fprintf(stderr, "Failed to save image") ;
        return 0;
    }
    
    fwrite (buffer, 1, size, imageSaveData) ;
    fclose (imageSaveData) ;

    return TRUE ;
}