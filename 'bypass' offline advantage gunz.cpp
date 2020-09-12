
#include <iostream> // funcs
#include <windows.h> // win funcs
#include <TlHelp32.h>
using namespace std;

HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE); // console color

typedef LONG    NTSTATUS;

typedef NTSTATUS(WINAPI* pNtQIT)(HANDLE, LONG, PVOID, ULONG, PULONG);

#define STATUS_SUCCESS    ((NTSTATUS)0x00000000L)

#define ThreadQuerySetWin32StartAddress 9


DWORD WINAPI GetThreadStartAddress(HANDLE hThread)

{

	NTSTATUS ntStatus;

	HANDLE hDupHandle;

	DWORD dwStartAddress;



	pNtQIT NtQueryInformationThread = (pNtQIT)GetProcAddress(GetModuleHandle("ntdll.dll"), "NtQueryInformationThread");

	if (NtQueryInformationThread == NULL) return 0;



	HANDLE hCurrentProcess = GetCurrentProcess();

	if (!DuplicateHandle(hCurrentProcess, hThread, hCurrentProcess, &hDupHandle, THREAD_QUERY_INFORMATION, FALSE, 0)) {

		SetLastError(ERROR_ACCESS_DENIED);

		return 0;

	}

	ntStatus = NtQueryInformationThread(hDupHandle, ThreadQuerySetWin32StartAddress, &dwStartAddress, sizeof(DWORD), NULL);

	CloseHandle(hDupHandle);



	if (ntStatus != STATUS_SUCCESS) return 0;

	return dwStartAddress;

}




DWORD GetProcessIdByName(char* name)
{
	PROCESSENTRY32 PE32;
	PE32.dwSize = sizeof(PROCESSENTRY32);
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); // dá um print nos processos
	BOOL status = Process32First(hSnap, &PE32); // BLZ, INICIOU
	while (status) {
		if (!strcmp(PE32.szExeFile, name)) {
			CloseHandle(hSnap);
			return PE32.th32ProcessID;
		}
		status = Process32Next(hSnap, &PE32);
	}
	CloseHandle(hSnap);
	return 0;
}




DWORD GetProcessIdByWindowName(TCHAR* window_name) {
	HWND windowHandle = FindWindow(NULL, window_name); // pega o identificador da janela
	if (windowHandle == 0) { // ve se achou
		return 0;
	}
	DWORD processID = 0; // cria var pid
	GetWindowThreadProcessId(windowHandle, &processID);
	return processID;
}

DWORD getModuleAddressByName(DWORD PID, char* module_name) { // Retorna o ModuleAddress do primeiro modulo que encontramos com este nome
	SetLastError(0);
	std::cout << "Module name: " << module_name << endl;
	MODULEENTRY32 me32;
	me32.dwSize = sizeof(MODULEENTRY32);
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, PID);
	if (snapshot == INVALID_HANDLE_VALUE) {
		cout << "CreateToolhelp32Snapshot module handle error! = " << GetLastError() << endl;
		system("pause");
		exit(0);
	}
	bool isNotEnd = true;
	Module32First(snapshot, &me32);
	while (isNotEnd) {
		std::cout << "Module addr: " << me32.szModule << endl;
		if (!strcmp(me32.szModule, module_name)) {
			CloseHandle(snapshot);
			return (DWORD)me32.modBaseAddr;
		}
		isNotEnd = Module32Next(snapshot, &me32);
	}
	CloseHandle(snapshot);
	std::cout << "Não achou Err = " << GetLastError() << endl;
	return 0;
}

DWORD OperarThread(DWORD PID = 0, bool suspender = false) { // retorna o numero de thread modificadas
	if (PID == 0) {
		return false;
	}
	DWORD moduleAddress = 0x400000;//getModuleAddressByName(PID, (char*)"UGGunz.exe");
	//DWORD moduleAddress = getModuleAddressByName(PID, (char*)"theduel.exe");
	THREADENTRY32 TH32;
	TH32.dwSize = sizeof(THREADENTRY32);
	HANDLE hSpTh = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	if (hSpTh == INVALID_HANDLE_VALUE) {
		cout << "CreateToolhelp32Snapshot handle error!" << endl;
		system("pause");
	}
	BOOL status = Thread32First(hSpTh, &TH32);
	DWORD cnt_thds = 0;
	HANDLE Current_Handle_Thread;
	int num_threads_operalizadas = 0;
	while (status) {
		if (TH32.th32OwnerProcessID == PID) { // filtra pelo PID
			{
				// open thread
				Current_Handle_Thread = OpenThread(THREAD_ALL_ACCESS, false, TH32.th32ThreadID);
				if (Current_Handle_Thread == INVALID_HANDLE_VALUE) {
					cout << "Current_Handle_Thread handle error!" << endl;
					system("pause");
				}
				// filter by StartAddress
				//if(GetThreadStartAddress(Current_Handle_Thread) == (moduleAddress + 0x3d9940)){
				if (GetThreadStartAddress(Current_Handle_Thread) == (moduleAddress + 0x1e67f0)) {
					std::cout << "TID = " <<  TH32.th32ThreadID << endl;
					num_threads_operalizadas++;
					if(suspender){SuspendThread(Current_Handle_Thread);}
					else { ResumeThread(Current_Handle_Thread); }
				}
				CloseHandle(Current_Handle_Thread); // keep it clean
			}

		}
		status = Thread32Next(hSpTh, &TH32);
	}
	CloseHandle(hSpTh);
	cout << num_threads_operalizadas << endl;
	return num_threads_operalizadas;
}

int main() {
	setlocale(LC_ALL, "Portuguese");
	system("MODE con cols=80 lines=20 ");
	SetConsoleTextAttribute(console, 0x0a);
	std::cout << "Iniciando o Hack!" << endl;
	//DWORD PID = GetProcessIdByName((char*)"theduel.exe");
	DWORD PID = GetProcessIdByName((char*)"UGGunz.exe");
	if (PID == 0) {
		SetConsoleTextAttribute(console, 0x0C);
		std::cout << "Jogo não está aberto!" << endl;
		system("pause");
		return 0;
	}
	std::cout << "PID = " << PID << endl;

	HANDLE op = OpenProcess(PROCESS_ALL_ACCESS, false, PID);
	if (op == INVALID_HANDLE_VALUE) {
		SetConsoleTextAttribute(console, 0x0C);
		cout << "Erro ao abrir o processo, tem alguma proteção?" << endl;
		system("pause");
		exit(0);
	}


	bool toggle = true;
	DWORD result_operated_threads = 0;
	
	std::cout << "Preparado para funcionar! F2 ATIVA/DESATIVA | F12 FECHA" << endl;
	while (!(GetKeyState(VK_F12) & 0x8000)) {
		Sleep(100); // evitar a fadiga kkk
		if (GetKeyState(VK_F2) & 0x8000) { //
			
			Sleep(100);// evitar q aperte 2 vezes
			//
			result_operated_threads = OperarThread(PID, toggle);
			if (result_operated_threads == 0) { // deu erro, verifica PID
				//PID = GetProcessIdByName((char*)"theduel.exe");
				Beep(80, 1000);
				PID = GetProcessIdByName((char*)"UGGunz.exe");
				if (!PID) { SetConsoleTextAttribute(console, 0x0C); std::cout << "Erro, saiste?" << endl; }
			}
			else if (result_operated_threads == (1 || 2)) {
				SetConsoleTextAttribute(console, 0x0C);
				Beep(80, 1000);
				std::cout << "Não precisa ativar no lobby/login" << endl;
			}
			else {
				system("cls");
				std::cout << "Bypass está ";
				if (toggle) {
					Beep(500, 100);
					SetConsoleTextAttribute(console, 0x0a);
					std::cout << "Ativado!";
				}
				else {
					Beep(5000, 100);
					SetConsoleTextAttribute(console, 0x0C);
					std::cout << "Desativado!";
				}
			}


			toggle = !toggle;
			Sleep(100);// evitar q aperte 2 vezes
		}
	}
	SetConsoleTextAttribute(console, 0x0C);
	std::cout << "Saindo!" << endl;
	// listar todas as threads
	return 0;
}