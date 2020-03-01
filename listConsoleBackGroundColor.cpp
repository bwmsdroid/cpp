#include <iostream>
#include <string>
#include <windows.h>

using namespace std;

HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE); // para uso do SetConsoleTextAttribute()

template <typename I> std::string n2hexstr(I w, size_t hex_len = sizeof(I) << 1) {
	static const char* digits = "0123456789ABCDEF";
	std::string rc(hex_len, '0');
	for (size_t i = 0, j = (hex_len - 1) * 4; i < hex_len; ++i, j -= 4)
		rc[i] = digits[(w >> j) & 0x0f];
	return rc;
}

void list_background(int time = 3000) {
	int x = 10;
	string texto_color = "color ";

	for (x = 0; x <= 255; x += 16) {
		texto_color = "color " + n2hexstr(x, 2);
		system(texto_color.c_str());
		cout << "cor: " << texto_color << endl;
		Sleep(3000);
	}



}

int main()
{
	list_background();
	getchar();
	return 0;
}
