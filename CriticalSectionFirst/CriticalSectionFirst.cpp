#include "stdafx.h"
#include "windows.h"
#include "wchar.h"
#include "string"
#include "iostream"

void logError(LPCTSTR);
void logInfo(LPCTSTR);
void logInfo(LPCTSTR, LPCTSTR);
void doDirtyWork(LPCTSTR);

int main()
{
	const LPCTSTR SHARED_MEMORY_FILENAME = TEXT("SharedMemoryFile.txt");
	const int SHARED_MEMORY_BUFFER_SIZE = 256;
	
	HANDLE sharedMemory;
	LPCTSTR messageBuffer;

	HANDLE sharedMemory = CreateFileMapping(
		INVALID_HANDLE_VALUE,
		NULL,
		PAGE_READWRITE,
		0,
		SHARED_MEMORY_BUFFER_SIZE,
		SHARED_MEMORY_FILENAME
	);
	
	if (sharedMemory == NULL) {
		logError(TEXT("Cannot get or create shared memory"));

		return 1;
	}

	messageBuffer = (LPCTSTR) MapViewOfFile(
		sharedMemory,
		FILE_MAP_ALL_ACCESS,
		0,
		0,
		SHARED_MEMORY_BUFFER_SIZE
	);

	if (messageBuffer == NULL) {
		logError(TEXT("Cannot map view of file"));

		CloseHandle(sharedMemory);

		return 1;
	}

	doDirtyWork(messageBuffer);

	UnmapViewOfFile(messageBuffer);
	CloseHandle(sharedMemory);

	return 0;
}

void doDirtyWork(LPCTSTR messageBuffer) {
	const LPCTSTR PROCESS_MESSAGE = TEXT("first process...");

	// TODO LOCK

	logInfo(TEXT("Read shared memory message"), messageBuffer);

	logInfo(TEXT("Write shared memory message"), PROCESS_MESSAGE);

	CopyMemory(
		(PVOID)messageBuffer,
		PROCESS_MESSAGE,
		(_tcslen(PROCESS_MESSAGE) * sizeof(LPCTSTR))
	);

	logInfo(TEXT("Read shared memory message"), messageBuffer);

	// TODO UNLOCK
}

void logError(LPCTSTR message) {
	std::cout << "[ERROR]: FirstProgram: " << message << std::endl;
	std::flush(std::cout);
}

void logInfo(LPCTSTR message) {
	std::cout << "[INFO]: FirstProgram:" << message << std::endl;
	std::flush(std::cout);
}

void logInfo(LPCTSTR messagePrefix, LPCTSTR message) {
	std::cout << "[INFO]: FirstProgram: " << messagePrefix << ": " << message;
	std::flush(std::cout);
}

