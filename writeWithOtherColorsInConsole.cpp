#include <iostream>
#include <string>
#include <string.h>
#include <windows.h>

using namespace std;

HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE); // handle output do console

int main()
{
	system("MODE con cols=140 lines=50 ");
	setlocale(LC_ALL, ""); //utf-8
	
	string text = "Olá mundo";
	SetConsoleTextAttribute(console, 0x0C); // define a cor
	cout << text << endl;
	SetConsoleTextAttribute(console, 0xa); // define a cor 
	
	cout << text << endl;
	
	getchar();
	return 0;
}


