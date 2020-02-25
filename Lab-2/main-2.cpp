#include <iostream>
#include <cstdlib>
#include <Windows.h>

using namespace std;

int main() {
	system("chcp 65001"); system("cls");

	constexpr unsigned int lengthFile1{ 15 };
	constexpr unsigned int speedPrn{ 5 };
	int flagPrn{};
	int numSegmentPrn{};
	int timeSegmentPrn{};

	constexpr unsigned int lengthFile2{ 30 };
	constexpr unsigned int speedWrt{ 4 };
	int flagWrt{};
	int numSegmentWrt{};
	int timeSegmentWrt{};

	cout << "\tДлина файла принтера = [" << lengthFile1 << "]\t" << "Скорость печати = [" << 
		speedPrn << "]" << endl;
	cout << "\tДлина файла записи = [" << lengthFile2 << "]\t" << "Скорость записи = [" << 
		speedWrt << "]" << endl << endl;
	cout << "\t[Номер такта]\t\t[состояние принтера]\t\t[состояние диска]" << endl << endl;
	
	bool isEnded{};
	bool ifNewSended{ false };
	for (unsigned int tact = 0; !isEnded; ++tact) {
		Sleep(40);
		isEnded		= true;
		ifNewSended = false;

		cout << "\t[" << tact << "]\t\t\t";
		switch (flagPrn) {
		case 0: cout << "*ожидание*\t\t\t";									break;
		case 1: cout << "печать сегмента " << numSegmentPrn << "\t\t";		break;
		}

		switch (flagWrt) {
		case 0: cout << "*ожидание*";										break;
		case 1: cout << "запись сегмента " << numSegmentWrt;				break;
		}
		cout << endl;

		// Пока (печатаемый сегмент не последний) или (последний и печатается)
		if ((numSegmentPrn < lengthFile1) || (numSegmentPrn == lengthFile1 && flagPrn)) {
			isEnded = false;

			if (!flagPrn && !ifNewSended) {
				++numSegmentPrn;
				timeSegmentPrn = speedPrn;
				ifNewSended = true;
				flagPrn = true;
			}
			else if (timeSegmentPrn > 0 && --timeSegmentPrn == 0) {
				flagPrn = false;
			}
		}

		if ((numSegmentWrt < lengthFile2) || (numSegmentWrt == lengthFile2 && flagWrt)) {
			isEnded = false;

			if (!flagWrt && !ifNewSended) {
				++numSegmentWrt;
				timeSegmentWrt = speedWrt;
				ifNewSended = true;
				flagWrt = true;
			}
			else if (timeSegmentWrt > 0 && --timeSegmentWrt == 0) {
				flagWrt = false;
			}
		}
	}

	cout << "\n\tРабота успешно завершена. Завершение процесса...\n" << endl;
	
	system("pause");
	return 0;
}