#include <iostream>
#include <string>
#include <random>

using namespace std;

constexpr int g_totalTime = /*15*/86400;
constexpr int g_maxFileSize = 10;
constexpr int g_maxInterval = 10;

int64_t randR(int64_t _from, int64_t _to) {
	static random_device rd;
	if (_from > _to) swap(_from, _to);
	return (rd() % (_to - _from + 1) + _from);
}

class Delay {
private:
	const int m_maxInterval;

	int m_delayTime{};

public:
	Delay(int _maxInterval = 10) : m_maxInterval(_maxInterval) {}

	int getMaxInterval() const { return m_maxInterval; }
	int getInterval() const { return m_delayTime; }

	int tick() {
		if (m_delayTime > 0) {
			--m_delayTime;
			return 1;
		}
		else return 0;
	}
	friend int getNewDelay(Delay& _delay);
};
int getNewDelay(Delay& _delay) {
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

int main() {
	system("chcp 65001"); system("cls");

	Memory RAM;
	Files file(g_maxFileSize);
	Delay delay(g_maxInterval);

	cout << "Время\t\t" << "Количество сегментов" << endl;

	string outStr;
	for (int time = 0; time < g_totalTime; ++time) {
		outStr.clear();
		outStr += to_string(time + 1) += "\t\t";

		if (delay.getInterval() == 0) {
			getNewFile(file); getNewDelay(delay);
			RAM.addSegments(file);
			outStr += to_string(RAM.getSegmentsCount()) += "\t(добавлен новый файл размером ";
			outStr += to_string(file.getSize()) += " )";
		}
		else {
			delay.tick();
			RAM.tick();
			outStr += to_string(RAM.getSegmentsCount());
		}
		cout << outStr << endl;
	}

	cout << "[TIME OUT]" << endl;
	cout << "Максимальный объём занимаемой памяти: " << RAM.getMaxSegmentsCount() << endl;

	cout << "\nДля продолжения нажмите ввод";
	cin.get();
	return 0;
}