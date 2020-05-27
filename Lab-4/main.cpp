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
constexpr size_t g_numIterations = 5;
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
	system("chcp 65001"); /*system("mode con cols=180 lines=3000"); */ system("cls");
	{
		cout << "[Тест для системы, в которой файлы берутся последовательно]" << endl;
		array<InfoNode, g_numIterations> collector;
		for (int iterator{ 0 }; iterator < g_numIterations; ++iterator) {
			cout << "[Проход номер " << iterator + 1 << "]" << endl << endl;
			Memory RAM;
			Delay delay(g_maxInterval);
			Files* newFile{ nullptr };
			Computer system;

			if (g_isDetailedOut)
				cout << "Период\t\t" << "Количество сегментов\t\t\t\t" << "Максимальное\t" << "Среднее\t\t" << "Число файлов в памяти" << endl;

			uint64_t totalLength{};
			uint64_t totalFilesInRAM{};
			string outStr;

			for (time_t time = 0; time < g_totalTime; ++time) {
				if (g_isDetailedOut) {
					outStr.clear();
					outStr += to_string(time + 1) += "\t\t";
				}

				system.tick(RAM);
				if (delay.getInterval() == 0) {
					newFile = new Files(randR(1, g_maxFileSize));
					getNewDelay(delay);
					RAM.addFile(newFile);
					if (g_isDetailedOut) {
						outStr += to_string(RAM.getSegmentsCount()) += "\t(добавлен новый файл размером ";
						outStr += to_string(newFile->getSize()) += " )\t";
					}
				}
				else {
					delay.tick();
					if (g_isDetailedOut)
						outStr += to_string(RAM.getSegmentsCount()) += "\t\t\t\t\t\t";
				}

				totalFilesInRAM += RAM.getFilesCount();
				totalLength += RAM.getSegmentsCount();

				if (g_isDetailedOut) {
					outStr += to_string(RAM.getMaxSegmentsCount()) += "\t\t";
					outStr += to_string(static_cast<double>(totalLength) / (time + 1)) += "\t";
					outStr += to_string(RAM.getFilesCount());
					cout << outStr << endl;
					Sleep(20);
				}
			}
			collector.at(iterator) = InfoNode(static_cast<double>(totalLength) / g_totalTime, RAM.getMaxSegmentsCount(), static_cast<double>(totalFilesInRAM) / g_totalTime, RAM.getMaxFilesCount());

			cout << "[TIME OUT]" << endl;
			cout << "Максимальный объём занимаемой памяти: " << RAM.getMaxSegmentsCount() << endl;
			cout << "Максимальное число файлов в памяти: " << RAM.getMaxFilesCount() << endl << endl;

			cout << endl << endl;
			Sleep(500);
		}

		cout.precision(4);
		cout << "[Итоги после " << g_numIterations << " Итераций для системы, в которой файлы берутся последовательно]" << endl;
		cout << "Номер\t\t\t\t\t";
		for (int i = 1; i <= g_numIterations; ++i) {
			cout << i << "\t\t";
		}
		cout << endl << "Среднее число сегментов в памяти: \t";
		for (int i = 0; i < g_numIterations; ++i) {
			cout << collector.at(i).mean << "\t\t";
		}
		cout << endl << "Максимальное число сегментов: \t\t";
		for (int i = 0; i < g_numIterations; ++i) {
			cout << collector.at(i).maxLength << "\t\t";
		}
		cout << endl << "Среднее число файлов в памяти: \t\t";
		for (int i = 0; i < g_numIterations; ++i) {
			cout << collector.at(i).meanFiles << "\t\t";
		}
		cout << endl << "Максимальное число файлов: \t\t";
		for (int i = 0; i < g_numIterations; ++i) {
			cout << collector.at(i).maxFilesCount << "\t\t";
		}
		cout << endl << endl << endl << endl << endl;
		cout.precision(8);
	}
	{
		cout << "[Тест для системы, в которой берутся минимальные файлы]" << endl;
		array<InfoNode, g_numIterations> collector;
		for (int iterator{ 0 }; iterator < g_numIterations; ++iterator) {
			cout << "[Проход номер " << iterator + 1 << "]" << endl << endl;
			Memory RAM;
			Delay delay(g_maxInterval);
			Files* newFile{ nullptr };
			Computer system;

			if (g_isDetailedOut)
				cout << "Период\t\t" << "Количество сегментов\t\t\t\t" << "Максимальное\t" << "Среднее\t\t" << "Число файлов в памяти" << endl;

			uint64_t totalLength{};
			uint64_t totalFilesInRAM{};
			string outStr;

			for (time_t time = 0; time < g_totalTime; ++time) {
				if (g_isDetailedOut) {
					outStr.clear();
					outStr += to_string(time + 1) += "\t\t";
				}

				system.tick_min(RAM);
				if (delay.getInterval() == 0) {
					newFile = new Files(randR(1, g_maxFileSize));
					getNewDelay(delay);
					RAM.addFile(newFile);
					if (g_isDetailedOut) {
						outStr += to_string(RAM.getSegmentsCount()) += "\t(добавлен новый файл размером ";
						outStr += to_string(newFile->getSize()) += " )\t";
					}
				}
				else {
					delay.tick();
					if (g_isDetailedOut)
						outStr += to_string(RAM.getSegmentsCount()) += "\t\t\t\t\t\t";
				}

				totalFilesInRAM += RAM.getFilesCount();
				totalLength += RAM.getSegmentsCount();

				if (g_isDetailedOut) {
					outStr += to_string(RAM.getMaxSegmentsCount()) += "\t\t";
					outStr += to_string(static_cast<double>(totalLength) / (time + 1)) += "\t";
					outStr += to_string(RAM.getFilesCount());
					cout << outStr << endl;
					Sleep(20);
				}
			}
			collector.at(iterator) = InfoNode(static_cast<double>(totalLength) / g_totalTime, RAM.getMaxSegmentsCount(), static_cast<double>(totalFilesInRAM) / g_totalTime, RAM.getMaxFilesCount());

			cout << "[TIME OUT]" << endl;
			cout << "Максимальный объём занимаемой памяти: " << RAM.getMaxSegmentsCount() << endl;
			cout << "Максимальное число файлов в памяти: " << RAM.getMaxFilesCount() << endl << endl;

			cout << endl << endl;
			Sleep(500);
		}

		cout.precision(4);
		cout << "[Итоги после " << g_numIterations << " Итераций для системы, в которой берутся минимальные файлы]" << endl;
		cout << "Номер\t\t\t\t\t";
		for (int i = 1; i <= g_numIterations; ++i) {
			cout << i << "\t\t";
		}
		cout << endl << "Среднее число сегментов в памяти: \t";
		for (int i = 0; i < g_numIterations; ++i) {
			cout << collector.at(i).mean << "\t\t";
		}
		cout << endl << "Максимальное число сегментов: \t\t";
		for (int i = 0; i < g_numIterations; ++i) {
			cout << collector.at(i).maxLength << "\t\t";
		}
		cout << endl << "Среднее число файлов в памяти: \t\t";
		for (int i = 0; i < g_numIterations; ++i) {
			cout << collector.at(i).meanFiles << "\t\t";
		}
		cout << endl << "Максимальное число файлов: \t\t";
		for (int i = 0; i < g_numIterations; ++i) {
			cout << collector.at(i).maxFilesCount << "\t\t";
		}
		cout << endl << endl << endl;
		cout.precision(8);
	}

	std::cout << "\nДля продолжения нажмите ввод";
	cin.get();
	return 0;
}