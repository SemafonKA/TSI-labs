//#define TEST
//#define CHART_TEST

#include <iostream>
#include <string>
#include <random>
#include <array>
#include <vector>
#include <windows.h>
#include "./Dlist/Dlist.h"

using namespace std;

#ifdef TEST
const     bool   g_isDetailedOut = true;
constexpr time_t g_totalTime = 15;
#else
const     bool   g_isDetailedOut = false;
constexpr time_t g_totalTime = 86400;
#endif
constexpr size_t g_numIterations = 6;
constexpr size_t g_maxFileSize = 10;
constexpr time_t g_maxInterval = 10;

int64_t randR(int64_t _from, int64_t _to) {
	static random_device rd;
	if (_from > _to) std::swap(_from, _to);
	return (rd() % (_to - _from + 1) + _from);
}

/* задержка перед отправкой нового файла */
class Delay {
private:
	const time_t m_maxInterval;

	time_t m_delayTime{};

public:
	Delay(time_t _maxInterval = 10) : m_maxInterval(_maxInterval) {}

	time_t getMaxInterval() const { return m_maxInterval; }
	time_t getInterval() const { return m_delayTime; }

	int tick() {
		if (m_delayTime > 0) {
			--m_delayTime;
			return 1;
		}
		else return 0;
	}
	friend time_t getNewDelay(Delay& _delay);
};
time_t getNewDelay(Delay& _delay) {
	_delay.m_delayTime = randR(1, _delay.m_maxInterval);
	return _delay.m_delayTime;
}

class Computer;
/* сам файл */
class Files {
friend class Computer;
friend class Memory;

private:
	size_t m_size{};

public:
	Files(size_t _fileSize) : m_size(_fileSize) {}

	size_t getSize() const { return m_size; }
};

/* Память, которая хранит в себе файлы посегментно */
class Memory {
friend class Computer;

private:
	size_t m_segmentsCount{};
	size_t m_maxSegmentsCount{};
	size_t m_maxFilesCount{};

	Dlist<Files*> m_file_list{};

	int file_decrease(size_t _index) {
		if (_index < 0 || _index >= m_file_list.size())
			return -1;
		Files* file = m_file_list.at(_index);
		file->m_size--;
		if (file->m_size == 0) m_file_list.pop(_index);
		m_segmentsCount--;
		return 0;
	}

public:
	size_t getSegmentsCount()     const { return m_segmentsCount; }
	size_t getMaxSegmentsCount()  const { return m_maxSegmentsCount; }
	size_t getFilesCount()        const { return m_file_list.size(); }
	size_t getMaxFilesCount()     const { return m_maxFilesCount; }

	bool isEmpty() const { return m_segmentsCount == 0; }

	size_t addFile(Files* _file) {
		m_segmentsCount += _file->getSize();
		m_file_list.push_back(_file);
		m_maxSegmentsCount = max(m_maxSegmentsCount, m_segmentsCount);
		m_maxFilesCount = max(m_maxFilesCount, m_file_list.size());
		return m_segmentsCount;
	}
};

/* Программа, которая обрабатывает файлы в памяти посегментно */
class Computer {
public:
	void tick(Memory& _memory) {
		if (_memory.isEmpty()) return;
		_memory.file_decrease(0);
	}

	void tick_min(Memory& _memory) {
		if (_memory.isEmpty()) return;
		int minSize{}, indMinSize{};
		minSize = _memory.m_file_list.front()->m_size;
		for (int i = 1; i < _memory.m_file_list.size(); ++i) {
			if (_memory.m_file_list.at(i)->m_size < minSize) {
				minSize = _memory.m_file_list.at(i)->m_size;
				indMinSize = i;
			}
		}
		_memory.file_decrease(indMinSize);
	}
};

/* структура для коллекционирования данных в итоговую таблицу */
struct InfoNode {
	uint32_t maxLength{};
	uint32_t maxFilesCount{};
	double   mean{};
	double   meanFiles{};

	InfoNode(double _mean = 0, uint32_t _maxLength = 0, double _meanFiles = 0, uint32_t _maxFilesCount = 0) {
		mean = _mean;
		maxLength = _maxLength;
		maxFilesCount = _maxFilesCount;
		meanFiles = _meanFiles;
	}
};

int main() {
	system("chcp 65001"); system("mode con cols=180 lines=3000");  system("cls");

	cout << "Формат вывода: Данные для линейного выполнения / Данные для выполнения по минимуму" << endl << endl;
	array<InfoNode, g_numIterations> collectorLinear, collectorMinimal;
	for (int iterator{ 0 }; iterator < g_numIterations; ++iterator) {
		cout << "[Проход номер " << iterator + 1 << "]" << endl << endl;
		Memory RAMlinear, RAMminimal;
		Delay delay(g_maxInterval);
		Files* newFileLinear{ nullptr }, * newFileMinimal{ nullptr };
		Computer systemLinear, systemMinimal;

		if (g_isDetailedOut)
			cout << "Период\t\t" << "Количество сегментов\t\t\t\t" << "Максимальное\t" << "Среднее\t\t\t" << "Число файлов в памяти" << endl;

		uint64_t totalLengthLinear{}, totalLengthMinimal{};
		uint64_t totalFilesInRAMLinear{}, totalFilesInRAMMinimal{};
		string outStr;

		for (time_t time = 0; time < g_totalTime; ++time) {
			if (g_isDetailedOut) {
				outStr.clear();
				outStr += to_string(time + 1) += "\t\t";
			}

			systemLinear.tick(RAMlinear);
			systemMinimal.tick_min(RAMminimal);
			if (delay.getInterval() == 0) {
				newFileLinear = new Files(randR(1, g_maxFileSize));
				newFileMinimal = new Files(newFileLinear->getSize());

				getNewDelay(delay);
				RAMlinear.addFile(newFileLinear);
				RAMminimal.addFile(newFileMinimal);
				if (g_isDetailedOut) {
					outStr += to_string(RAMlinear.getSegmentsCount()) += " / ";
					outStr += to_string(RAMminimal.getSegmentsCount()) += "\t(добавлен новый файл размером ";
					outStr += to_string(newFileLinear->getSize()) += " )\t";
				}
			}
			else {
				delay.tick();
				if (g_isDetailedOut) {
					outStr += to_string(RAMlinear.getSegmentsCount()) += " / ";
					outStr += to_string(RAMminimal.getSegmentsCount()) += "\t\t\t\t\t\t";
				}
			}

			totalFilesInRAMLinear  += RAMlinear.getFilesCount();
			totalFilesInRAMMinimal += RAMminimal.getFilesCount();
			totalLengthLinear  += RAMlinear.getSegmentsCount();
			totalLengthMinimal += RAMminimal.getSegmentsCount();

			if (g_isDetailedOut) {
				outStr += to_string(RAMlinear.getMaxSegmentsCount()) += " / ";
				outStr += to_string(RAMminimal.getMaxSegmentsCount()) += "\t\t";
				outStr += to_string(static_cast<double>(totalLengthLinear) / (time + 1)) += " / ";
				outStr += to_string(static_cast<double>(totalLengthMinimal) / (time + 1)) += "\t";
				outStr += to_string(RAMlinear.getFilesCount()) += " / ";
				outStr += to_string(RAMminimal.getFilesCount());
				cout << outStr << endl;
				Sleep(20);
			}
		}
		collectorLinear.at(iterator) = InfoNode(static_cast<double>(totalLengthLinear) / g_totalTime, RAMlinear.getMaxSegmentsCount(), static_cast<double>(totalFilesInRAMLinear) / g_totalTime, RAMlinear.getMaxFilesCount());
		collectorMinimal.at(iterator) = InfoNode(static_cast<double>(totalLengthMinimal) / g_totalTime, RAMminimal.getMaxSegmentsCount(), static_cast<double>(totalFilesInRAMMinimal) / g_totalTime, RAMminimal.getMaxFilesCount());

		cout << "[TIME OUT]" << endl;
		cout << "Максимальный объём занимаемой памяти: " << RAMlinear.getMaxSegmentsCount() << " / " << RAMminimal.getMaxSegmentsCount() << endl;
		cout << "Максимальное число файлов в памяти: " << RAMlinear.getMaxFilesCount() << " / " << RAMminimal.getMaxFilesCount() << endl << endl;

		cout << endl << endl;
		Sleep(500);
	}

	cout.precision(4);
	cout << "[Итоги после " << g_numIterations << " Итераций]" << endl;
	cout << "Номер\t\t\t\t\t";
	for (int i = 1; i <= g_numIterations; ++i) {
		cout << i << "\t\t\t";
	}
	cout << endl << "Среднее число сегментов в памяти: \t";
	for (int i = 0; i < g_numIterations; ++i) {
		cout << collectorLinear.at(i).mean << " / " << collectorMinimal.at(i).mean << "\t\t";
	}
	cout << endl << "Максимальное число сегментов: \t\t";
	for (int i = 0; i < g_numIterations; ++i) {
		cout << collectorLinear.at(i).maxLength << " / " << collectorMinimal.at(i).maxLength << "\t\t\t";
	}
	cout << endl << "Среднее число файлов в памяти: \t\t";
	for (int i = 0; i < g_numIterations; ++i) {
		cout << collectorLinear.at(i).meanFiles << " / " << collectorMinimal.at(i).meanFiles << "\t\t";
	}
	cout << endl << "Максимальное число файлов: \t\t";
	for (int i = 0; i < g_numIterations; ++i) {
		cout << collectorLinear.at(i).maxFilesCount << " / " << collectorMinimal.at(i).maxFilesCount << "\t\t\t";
	}
	cout << endl << endl << endl << endl << endl;

	std::cout << "\nДля продолжения нажмите ввод";
	cin.ignore(cin.rdbuf()->in_avail());
	cin.get();
	return 0;
}