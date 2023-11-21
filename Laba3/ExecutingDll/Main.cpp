#include <stdio.h>
#include <iostream>
#include <conio.h>
#include <windows.h>
#include <tlhelp32.h>
#include <Header.h>

using namespace std;

typedef int Func(int, int);

const int number = 4;

DWORD GetProcessIdByProcessName(string processName)
{
	DWORD processId = 0;
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 processEntry;

	ZeroMemory(&processEntry, sizeof(processEntry));
	processEntry.dwSize = sizeof(processEntry);

	bool isFound = false;
	while (Process32Next(hSnapshot, &processEntry) && !isFound)
	{
		if (!processName.compare(processEntry.szExeFile))
		{
			processId = processEntry.th32ProcessID;
			isFound = true;
		}
	}

	return processId;
}

void main()
{
	string strings[number] = {
	"One two three",
	"Lab 3 lab 4 ",
	"Security",
	};


	//memory check
	for (int i = 0; i < number; ++i)
		cout << strings[i].c_str() << endl;

	cout << "Replace in progress\n" << endl;

	Replace(strings[2].c_str(), "HACKED!");

	for (int i = 0; i < number; ++i)
		cout << strings[i].c_str() << endl;


	//dll test's
	cout << "Static Include: " << endl << "3 * 2 = " << Sum(3, 2) << endl << "4  % 3 = " << Sub(4, 3) << endl;

	HMODULE dll = 0;
	if ((dll = LoadLibrary("SomeDll"))) {
		Func* _Sum, * _Sub;
		_Sum = (Func*)GetProcAddress(dll, MAKEINTRESOURCE(4));
		_Sub = (Func*)GetProcAddress(dll, MAKEINTRESOURCE(3));
		cout << "Dynamic Include: " << endl << "3 * 2 = " << _Sum(3, 2) << endl << "4 % 3 = " << _Sub(4, 3) << endl;
		FreeLibrary(dll);
	}
	else
		printf("Cant load Dll");


	//dll injection
	cout << "Enter NAME of the process without .exe: " << endl;
	std::string AppName;
	std::cin >> AppName;
	DWORD pid = GetProcessIdByProcessName(AppName + ".exe");
	cout << "PID: ";
	cout << pid << endl;

	HANDLE hRemoteProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);

	if (hRemoteProcess != NULL)
	{
		cout << "\nPID finded" << endl;

		LPVOID threadFunction = (LPVOID)GetProcAddress(GetModuleHandle("kernel32.dll"), "LoadLibraryA");

		string argument("InjectionDll.dll");

		LPVOID argumentAddress = VirtualAllocEx(hRemoteProcess, NULL, argument.length() + 1, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

		WriteProcessMemory(hRemoteProcess, (LPVOID)argumentAddress, argument.c_str(), argument.length() + 1, NULL);

		if (CreateRemoteThread(hRemoteProcess, NULL, 0, (LPTHREAD_START_ROUTINE)threadFunction, (LPVOID)argumentAddress, 0, NULL))
		{
			Sleep(1000);
			cout << "Creating thread" << endl;
			CloseHandle(hRemoteProcess);
		}
		else
			cout << "Cant create thread" << endl;


	}
	else
		cout << "Cant find PID" << endl;

	cout << "\nEnter key for exit" << endl;
	_getch();
}