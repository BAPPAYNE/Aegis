#ifndef SYSINFO_H
#define SYSINFO_H

#include <windows.h>
#include <stdint.h>


int GetSystemInfoDetails(struct SystemInfoStructure* sysInfo);
char* get_OSVersion();
char* get_Architecture();
char* get_ComputerName();
char* get_CPUBrand();
char* get_UserName();
uint32_t get_ScreenWidth();
uint32_t get_ScreenHeight();
char* get_SystemDirectory();
char* get_WindowsDirectory();
char* get_EnvironmentVariables();
char* get_EnvironmentStrings();
uint8_t get_ProcessorCount();
uint64_t get_TotalPhysicalMemory();
SystemInfoStructure::ipAddressMac get_NetworkInfo();




#endif // SYSINFO_H