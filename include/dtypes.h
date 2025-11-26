#ifndef DTYPES_H
#define DTYPES_H

#include <windows.h>
#include <stdint.h>

struct ipAddressMac {
	uint8_t ip[4];
	uint8_t mac[6];
} typedef ipAddressMac;

struct SystemInfoStructure {
	uint8_t osVersion;
	uint8_t architecture;
	char* computerName;
	char* cpuBrand;
	char* userName;
	uint32_t screenWidth;
	uint32_t screenHeight;
	char* systemDirectory;
	char* windowsDirectory;
	char* environmentVariables;
	char* environmentStrings;
	uint8_t processorCount;
	uint64_t totalPhysicalMemory;
	uint32_t oemId;
	uint16_t processorArchitecture;
	uint8_t processorLevel;
	uint32_t processorType;
	uint32_t numberOfProcessors;
	uint32_t pageSize;
	uint64_t activeProcessorMask;
	ipAddressMac networkInfo ;
};

#endif // DTYPES_H
