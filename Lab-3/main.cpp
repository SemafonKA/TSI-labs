#include <iostream>
#include <string>
#include <random>
#include <array>

using namespace std;

constexpr time_t g_totalTime       = 15;
//constexpr time_t g_totalTime   = 86400;
constexpr int    g_numIterations   = 5;
constexpr int    g_maxFileSize     = 10;
constexpr time_t g_maxInterval     = 10;

int64_t randR(int64_t _from, int64_t _to) {
	static random_device rd;
	if (_from > _to) swap(_from, _to);
	return (rd() % (_to - _from + 1) + _from);
}

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

class Files {
private:
	const int m_maxSize{};

	int m_size{};

public:
	Files(int _maxFileSize) : m_maxSize(_maxFileSize) {}

	friend int getNewFile(Files& _file);

	int getSize() const { return m_size; }
	int getMaxSize() const { return m_maxSize; }
};
int getNewFile(Files& _file) {
	_file.m_size = randR(1, _file.m_maxSize);
	return _file.m_size;
}

class Memory {
private:
	int m_segmentsCount{};
	int m_maxSegmentsCount{};

public:
	int getSegmentsCount() const { return m_segmentsCount; }
	int getMaxSegmentsCount() const { return m_maxSegmentsCount; }
	
	bool isEmpty() const { return m_segmentsCount == 0; }

	int tick() {
		if (m_segmentsCount > 0) {
			--m_segmentsCount;
			return 1;
		}
		else return 0;
	}
	
	int addSegments(const Files& _file){
		m_segmentsCount += _file.getSize();
		m_maxSegmentsCount = max(m_maxSegmentsCount, m_segmentsCount);
		return m_segmentsCount;
	}

};

struct InfoNode {
	uint32_t maxLength{};
	double_t mean{};	

	InfoNode(double_t _mean = 0, uint32_t _maxLength = 0) {
		maxLength = _maxLength;
		mean = _mean;
	}
};

int main() {
	system("chcp 65001"); system("cls");

	array<InfoNode, g_numIterations> collector;
	for (int iterator{ 0 }; iterator < g_numIterations; ++iterator) {
		Memory RAM;
		Files file(g_maxFileSize);
		Delay delay(g_maxInterval);

		cout << "Период\t\t" << "Количество сегментов\t\t\t\t" << "Максимальное\t" << "Среднее" << endl;

		uint64_t totalLength{};
		string outStr;
		for (time_t time = 0; time < g_totalTime; ++time) {
			outStr.clear();
			outStr += to_string(time + 1) += "\t\t";

			if (delay.getInterval() == 0) {
				getNewFile(file); getNewDelay(delay);
				RAM.addSegments(file);
				totalLength += file.getSize();
				outStr += to_string(RAM.getSegmentsCount()) += "\t(добавлен новый файл размером ";
				outStr += to_string(file.getSize()) += " )\t";
			}
			else {
				delay.tick();
				RAM.tick();
				outStr += to_string(RAM.getSegmentsCount()) += "\t\t\t\t\t\t";
			}
			outStr += to_string(RAM.getMaxSegmentsCount()) += "\t\t";
			outStr += to_string(static_cast<double>(totalLength) / (time + 1));
			
			cout << outStr << endl;
		}
		collector[iterator] = InfoNode(static_cast<double>(totalLength) / g_totalTime, totalLength);

		cout << "[TIME OUT]" << endl;
		cout << "Максимальный объём занимаемой памяти: " << RAM.getMaxSegmentsCount() << endl << endl;
	}

	cout.precision(2);
	cout << "Итоги после " << g_numIterations << " Итераций" << endl;
	cout << "Номер\t";
	for (int i = 1; i <= g_numIterations; ++i) {
		cout << i << "\t\t";
	}
	cout << endl << "Среднее\t";
	for (int i = 0; i < g_numIterations; ++i) {
		cout << collector[i].mean << "\t\t";
	}
	cout << endl << "Макс.\t";
	for (int i = 0; i < g_numIterations; ++i){
		cout << collector[i].maxLength << "\t\t";
	}
	cout << endl;

	cout << "\nДля продолжения нажмите ввод";
	cin.get();
	return 0;
}