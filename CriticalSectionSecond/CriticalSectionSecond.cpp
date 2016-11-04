#include "stdafx.h"
#include <windows.h>
#include <wchar.h>
#include <string>
#include <iostream>
#include <stdio.h>
#include <conio.h>
#include <tchar.h>

void logError(LPCTSTR);
void logInfo(LPCTSTR, LPCTSTR);
void doDirtyWork(LPCTSTR, HANDLE);

int main()
{
	const LPCWSTR SHARED_MEMORY_NAME = TEXT("SharedMemory");
	const LPCWSTR MUTEX_NAME = TEXT("SyncMutex");
	const int SHARED_MEMORY_BUFFER_SIZE = 256;

	HANDLE sharedMemory;
	HANDLE mutex;
	LPCTSTR messageBuffer;

	sharedMemory = CreateFileMapping(
		INVALID_HANDLE_VALUE,
		NULL,
		PAGE_READWRITE,
		0,
		SHARED_MEMORY_BUFFER_SIZE,
		SHARED_MEMORY_NAME
	);

	if (sharedMemory == NULL) {
		logError(TEXT("Cannot get or create shared memory"));

		std::cout << GetLastError();

		getchar();
		return 1;
	}

	mutex = CreateMutex(
		NULL,
		FALSE,
		MUTEX_NAME
	);

	if (mutex == NULL) {
		logError(TEXT("Cannot create mutex"));

		return 1;
	}

	messageBuffer = (LPCTSTR)MapViewOfFile(
		sharedMemory,
		FILE_MAP_ALL_ACCESS,
		0,
		0,
		SHARED_MEMORY_BUFFER_SIZE
	);

	if (messageBuffer == NULL) {
		logError(TEXT("Cannot map view of file"));

		CloseHandle(sharedMemory);
		CloseHandle(mutex);

		return 1;
	}

	doDirtyWork(messageBuffer, mutex);

	UnmapViewOfFile(messageBuffer);
	CloseHandle(sharedMemory);
	CloseHandle(mutex);

	getchar();

	return 0;
}

void doDirtyWork(LPCTSTR messageBuffer, HANDLE mutex) {
	const LPCTSTR PROCESS_MESSAGE = TEXT("SECOND PROCESS MESSAGE");

	for (int i = 0; i < 3; i++) {
		WaitForSingleObject(mutex, INFINITE);

		logInfo(TEXT("Read shared memory message"), messageBuffer);

		logInfo(TEXT("Write shared memory message"), PROCESS_MESSAGE);

		CopyMemory(
			(PVOID)messageBuffer,
			PROCESS_MESSAGE,
			(_tcslen(PROCESS_MESSAGE) * sizeof(LPCTSTR))
		);

		logInfo(TEXT("Read shared memory message"), messageBuffer);

		Sleep(100);

		ReleaseMutex(mutex);
	}
}

void logError(LPCTSTR message) {
	std::wcout << "[ERROR]: SecondProgram: " << message << std::endl;
	std::flush(std::cout);
}

void logInfo(LPCTSTR messagePrefix, LPCTSTR message) {
	std::wcout << "[INFO]: SecondProgram: " << messagePrefix
		<< ": " << message << std::endl;
	std::flush(std::cout);
}

