#include <string.h>
#include <stdio.h>

#include "sysinfo.h"
#include "dtypes.h"

int GetSystemInfoDetails(struct SystemInfoStructure* sysInfo) {

	SYSTEM_INFO siSysInfo;
	GetSystemInfo(&siSysInfo);
	if (!sysInfo) {
		return 0; // Null pointer check
	}
	sysInfo->osVersion = get_OSVersion()[0];
	sysInfo->architecture = get_Architecture()[0];
	sysInfo->computerName = get_ComputerName();
	sysInfo->cpuBrand = get_CPUBrand();
	sysInfo->userName = get_UserName();
	sysInfo->systemDirectory = get_SystemDirectory();
	sysInfo->windowsDirectory = get_WindowsDirectory();
	sysInfo->environmentVariables = get_EnvironmentVariables();
	sysInfo->environmentStrings = get_EnvironmentStrings();
	sysInfo->processorCount = get_ProcessorCount();
	sysInfo->totalPhysicalMemory = get_TotalPhysicalMemory();
	sysInfo->oemId = siSysInfo.dwOemId;
	sysInfo->processorArchitecture = siSysInfo.wProcessorArchitecture;
	sysInfo->processorLevel = siSysInfo.wProcessorLevel;
	sysInfo->processorType = siSysInfo.dwProcessorType;
	sysInfo->numberOfProcessors = siSysInfo.dwNumberOfProcessors;
	sysInfo->pageSize = siSysInfo.dwPageSize;
	sysInfo->activeProcessorMask = (uint64_t)(uintptr_t)siSysInfo.dwActiveProcessorMask;
	sysInfo->screenWidth = get_ScreenWidth();
	sysInfo->screenHeight = get_ScreenHeight();

	
	return 1; // Success
}
char* get_OSVersion() {
	OSVERSIONINFOEX osvi;
	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	if (GetVersionEx((OSVERSIONINFO*)&osvi)) {
		static char version[128];
		sprintf(version, "%lu.%lu (Build %lu)", osvi.dwMajorVersion, osvi.dwMinorVersion, osvi.dwBuildNumber);
		return version;
	}
	return "Unknown";
}

char* get_CPUBrand() {
	static char brand[0x40];
	int cpuInfo[4] = { -1 };
	__cpuid(cpuInfo, 0x80000000);
	unsigned int nExIds = cpuInfo[0];
	memset(brand, 0, sizeof(brand));
	for (unsigned int i = 0x80000000; i <= nExIds; ++i) {
		__cpuid(cpuInfo, i);
		if (i == 0x80000002)
			memcpy(brand, cpuInfo, sizeof(cpuInfo));
		else if (i == 0x80000003)
			memcpy(brand + 16, cpuInfo, sizeof(cpuInfo));
		else if (i == 0x80000004)
			memcpy(brand + 32, cpuInfo, sizeof(cpuInfo));
	}
	return brand;
}

char* get_Architecture() {
	SYSTEM_INFO siSysInfo;
	GetSystemInfo(&siSysInfo);
	switch (siSysInfo.wProcessorArchitecture) {
	case PROCESSOR_ARCHITECTURE_AMD64:
		return "x64 (AMD or Intel)";
	case PROCESSOR_ARCHITECTURE_ARM:
		return "ARM";
	case PROCESSOR_ARCHITECTURE_IA64:
		return "Intel Itanium-based";
	case PROCESSOR_ARCHITECTURE_INTEL:
		return "x86";
	default:
		return "Unknown architecture";
	}
}

char* get_ComputerName() {
	static char computerName[MAX_COMPUTERNAME_LENGTH + 1];
	DWORD size = sizeof(computerName);
	if (GetComputerNameA(computerName, &size)) {
		return computerName;
	}
	return "Unknown";
}

char* get_UserName() {
	static char userName[256];
	DWORD size = sizeof(userName);
	if (GetUserNameA(userName, &size)) {
		return userName;
	}
	return "Unknown";
}

uint32_t get_ScreenWidth() {
	return (uint32_t)GetSystemMetrics(SM_CXSCREEN);
}

uint32_t get_ScreenHeight() {
	return (uint32_t)GetSystemMetrics(SM_CYSCREEN);
}

char* get_SystemDirectory() {
	static char systemDir[MAX_PATH];
	if (GetSystemDirectoryA(systemDir, MAX_PATH)) {
		return systemDir;
	}
	return "Unknown";
}

char* get_WindowsDirectory() {
	static char windowsDir[MAX_PATH];
	if (GetWindowsDirectoryA(windowsDir, MAX_PATH)) {
		return windowsDir;
	}
	return "Unknown";
}

char* get_EnvironmentVariables() {
	static char envVars[8192];
	DWORD size = GetEnvironmentVariableA(NULL, envVars, sizeof(envVars));
	if (size > 0 && size < sizeof(envVars)) {
		GetEnvironmentVariableA(NULL, envVars, sizeof(envVars));
		return envVars;
	}
	return "Unknown";
}

char* get_EnvironmentStrings() {
	LPCH envStrings = GetEnvironmentStringsA();
	if (envStrings) {
		static char buffer[8192];
		size_t offset = 0;
		LPCH current = envStrings;
		while (*current) {
			size_t len = strlen(current);
			if (offset + len + 1 < sizeof(buffer)) {
				strcpy_s(buffer + offset, sizeof(buffer) - offset, current);
				offset += len + 1;
			}
			current += len + 1;
		}
		buffer[offset] = '\0';
		FreeEnvironmentStringsA(envStrings);
		return buffer;
	}
	return "Unknown";
}

uint8_t get_ProcessorCount() {
	SYSTEM_INFO siSysInfo;
	GetSystemInfo(&siSysInfo);
	return (uint8_t)siSysInfo.dwNumberOfProcessors;
}

uint64_t get_TotalPhysicalMemory() {
	MEMORYSTATUSEX statex;
	statex.dwLength = sizeof(statex);
	if (GlobalMemoryStatusEx(&statex)) {
		return statex.ullTotalPhys;
	}
	return 0;
}

ipAddressMac get_NetworkInfo() {
	ipAddressMac netInfo;
	memset(&netInfo, 0, sizeof(netInfo));
	
	return netInfo;
}