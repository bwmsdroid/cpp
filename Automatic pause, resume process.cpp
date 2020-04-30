// UM SIMPLES PROGRAMA PARA PAUSAR AUTOMATICAMENTE TODAS AS THREADS DO PROCESSO EM QUESTÃO
// FICA ESPARANDO ATÉ ACHA-LO
// MADE BY BWMSDROID

#include <iostream> // funcs
#include <windows.h> // win funcs
#include <TlHelp32.h>

using namespace std;

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

bool Suspend_Resume_Thread(DWORD PID, bool suspende) {
	if (PID == 0) {
		return false;
	}
	THREADENTRY32 TH32;
	TH32.dwSize = sizeof(THREADENTRY32);
	HANDLE hSpTh = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0); //lista TDS as threads do windows rodando
	BOOL status = Thread32First(hSpTh, &TH32);
	DWORD cnt_thds = 0;
	while (status) {
		if (TH32.th32OwnerProcessID == PID) {
			{


				if (suspende) {
					HANDLE hThread = OpenThread(THREAD_SUSPEND_RESUME, false, TH32.th32ThreadID);
					if (hThread != NULL) {
						cnt_thds++;
						SuspendThread(hThread);
					}
					CloseHandle(hThread);


				}
				else {
					HANDLE hThread = OpenThread(THREAD_SUSPEND_RESUME, false, TH32.th32ThreadID);
					if (hThread != NULL) {
						cnt_thds++;
						ResumeThread(hThread);
					}
					CloseHandle(hThread);

				}

				}
				
			}
			status = Thread32Next(hSpTh, &TH32);
		}

		CloseHandle(hSpTh);
		Sleep(50);
		wcout << "THREADS MODIFICADAS: ";
		wcout << cnt_thds << " Threads modificadas!" << endl;
		return true;
}


DWORD GetProcessIdByWindowName(char* window_name) {
		HWND windowHandle = FindWindow(NULL, window_name); // pega o identificador da janela
		if (windowHandle == 0) { // ve se achou
			return 0;
		}
		DWORD processID = 0; // cria var pid
		GetWindowThreadProcessId(windowHandle, &processID);
		return processID;
}

void Pausar_processo_inicio_aguardo(char* name, DWORD delay, char result, DWORD PID, char* window_name) {
	cout << "Aguardando a inicializacao do processo" << endl;
	if (strcmp(window_name, "")) { // se tiver nome de janela

		DWORD Explorer_PID = GetProcessIdByName((CHAR*)"explorer.exe");
		
		while (PID == 0 || PID == Explorer_PID || PID == 4) {
			PID = GetProcessIdByWindowName(window_name);
			Sleep(100);
			if (PID == Explorer_PID) {
				cout << "Explorer com o mesmo nome de janela que o selecionado!!! Feche-o e pressione enter!";
				cout << Explorer_PID << endl;
				getchar();
			}
			Sleep(50);
		}
	}
	else if (strcmp(name, "")) { // se tiver nome de processo
		while (!PID) {
			PID = GetProcessIdByName(name);
			Sleep(100);
		}
	}
	if(PID == 0){
		cout << "Algum erro aconteceu" << endl;
		system("pause");
		return;
	}
	wcout << "Encontrado! PID = " << PID << endl;
	if (delay) {
		cout << "Aguardando " << delay << " segundos";
		Sleep(delay * 1000);
	}
	system("cls");
	cout << "Iniciando processo de pausamento!" << endl;

	bool o = result == 'p' ? true : false;

	if (Suspend_Resume_Thread(PID, o)) {
		cout << "Sucesso, todas as threads do processo pausadas!" << endl;
	}
	else {
		cout << "Erro, tente executar como administrador!" << endl;
	}
	
}




int main() {
	SetConsoleTitle("Programa auxiliar");
	cout << "Ola, o que desejas fazer?" << endl << "p = pause; r = resume; e = exit;" << endl;
	char resultado;
	cin >> resultado;
	system("cls");
	
	if (resultado == 'p'|| resultado == 'P') {
		resultado == 'p';
		cout << "Escolheu pausar" << endl;
	}
	else if (resultado == 'r' || resultado == 'R') {
		resultado == 'r';
		cout << "Escolheu resumir" << endl;
	}
	else {
		main();
	}
	

	cout << "Desejas utilizar qual dessas opções para identificar o processo?" << endl << "PID = i (decimal); Window name = w; Process name = p" << endl;
	char resultado2;
	cin >> resultado2;
	DWORD pid = 0;
	char process_name[255] = "";
	char window_name[255] = "";
	if (resultado2 == 'i') {
		cout << "Insert the pid to the operation: ";
		cin >> pid;
	}
	else if (resultado2 == 'w') {
		cout << "Insert the window name to the operation: ";
		cin >> window_name;
	}
	else {
		cout << "Insert the process name to the operation: ";
		cin >> process_name;
	}
	cout << "Insira o delay que voce deseja ter de espera para pausar/resumir assim que o processo for encontrado (segundos)" << endl;
	DWORD delay;
	cin >> delay;
	Pausar_processo_inicio_aguardo(process_name, delay, resultado, pid, window_name);
	system("pause");
	return 0;
}
