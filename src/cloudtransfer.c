#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <stdio.h>
#include <stdint.h>

#pragma comment(lib, "ws2_32.lib")

// --- Send PNG buffer to cloud VM via TCP socket ---
int SendBufferToCloud(const BYTE* pngBuffer, DWORD bufferSize,
    const char* cloudIP, uint16_t cloudPort) {
    WSADATA wsa;
    SOCKET sock;
    struct sockaddr_in server;
    int totalSent = 0;
    int bytesToSend;

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("[ERROR] WSAStartup failed. Error: %d\n", WSAGetLastError());
        return 0;
    }

    // Create socket
    sock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);
    if (sock == INVALID_SOCKET) {
        printf("[ERROR] Socket creation failed. Error: %d\n", WSAGetLastError());
        WSACleanup();
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
        WSACleanup();
        return 0;
    }

    printf("[INFO] Connected to cloud VM.\n");

    // Send PNG size first (4 bytes, network byte order)
    DWORD networkSize = htonl(bufferSize);
    if (send(sock, (const char*)&networkSize, sizeof(networkSize), 0) == SOCKET_ERROR) {
        printf("[ERROR] Failed to send image size. Error: %d\n", WSAGetLastError());
        closesocket(sock);
        WSACleanup();
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
            WSACleanup();
            return 0;
        }

        totalSent += sent;
        printf("[INFO] Sent %d/%lu bytes...\n", totalSent, (unsigned long)bufferSize);
    }

    printf("[INFO] Screenshot uploaded successfully.\n");

    closesocket(sock);
    WSACleanup();
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