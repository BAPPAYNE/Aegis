#ifndef CLOUDTRANSFER_H
#define CLOUDTRANSFER_H

#include <windows.h>
#include <stdint.h>

// Winsock Management
void InitializeWinsock(void);
void CleanupWinsock(void);

// Cloud Transfer Functions
int SendBufferToCloud(const BYTE* pngBuffer, DWORD bufferSize,
    const char* cloudIP, uint16_t cloudPort);

int SendScreenShotToCloud(const char* cloudIP, uint16_t cloudPort);

// Signal Detection & Configuration
void SetCloudConfig(const char* cloudIP, uint16_t cloudPort);
void DetectCaptureSignals(void);

#endif