#include <iostream>
#include <cstdlib>
#include <Windows.h>

using namespace std;

int main() {
	system("chcp 65001"); system("cls");
	constexpr unsigned int lengthFile{ 15 };
	constexpr unsigned int speedPrn{ 5 };
	int flagPrn{};
	int numSegment{};
	int timeSegment{};

	cout << "\tНомер такта\t\tсостояние принтера\t\tномер сегмента" << endl << endl;

	for (unsigned int tact = 0; numSegment <= lengthFile; ++tact) {
		Sleep(40);

		cout << "\t[" << tact << "]\t";		
		switch (flagPrn) {
		case 0: cout << "\t\tожидание";		break;
		case 1: cout << "\t\tвыполнение";	break;
		}
		if (flagPrn) cout << "\t\t\tпечать сегмента  " << numSegment;
		cout << endl;

		if (!flagPrn) {
			++numSegment;
			timeSegment = speedPrn;
			flagPrn		= true;
		}
		else if (timeSegment != 0 && --timeSegment == 0) {
			flagPrn = false;
		}
	}

	cout << "\n\tРабота успешно завершена. Завершение процесса...\n" << endl;

	system("pause");
	return 0;
}