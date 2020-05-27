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
const     bool   g_isDetailedOut     = true;
constexpr time_t g_totalTime         = 15;
#else
const     bool   g_isDetailedOut     = false;
constexpr time_t g_totalTime         = 86400;
#endif
constexpr size_t g_numIterations     = 7;
constexpr size_t g_maxFileSize       = 10;
constexpr time_t g_maxInterval       = 10;

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

/* сам файл */
class Files {
private:
	const size_t m_maxSize{};

	size_t m_size{};

public:
	Files(size_t _maxFileSize) : m_maxSize(_maxFileSize) {}

	friend size_t getNewFile(Files& _file);

	size_t getSize() const { return m_size; }
	size_t getMaxSize() const { return m_maxSize; }
};
size_t getNewFile(Files& _file) {
	_file.m_size = randR(1, _file.m_maxSize);
	return _file.m_size;
}

/* Память, которая хранит в себе файлы посегментно */
class Memory {
private:
	size_t m_segmentsCount{};
	size_t m_maxSegmentsCount{};

public:
	size_t getSegmentsCount() const { return m_segmentsCount; }
	size_t getMaxSegmentsCount() const { return m_maxSegmentsCount; }
	
	bool isEmpty() const { return m_segmentsCount == 0; }

	int tick() {
		if (m_segmentsCount > 0) {
			--m_segmentsCount;
			return 1;
		}
		else return 0;
	}
	
	size_t addSegments(const Files& _file){
		m_segmentsCount += _file.getSize();
		m_maxSegmentsCount = max(m_maxSegmentsCount, m_segmentsCount);
		return m_segmentsCount;
	}

};

/* структура для коллекционирования данных в итоговую таблицу */
struct InfoNode {
	uint32_t maxLength{};
	double_t mean{};	

	InfoNode(double_t _mean = 0, uint32_t _maxLength = 0) {
		mean = _mean;
		maxLength = _maxLength;
	}
};

/* работа с графиком */
class Chart {
public:
	static constexpr   size_t maxWidth                = 20;
	static constexpr   size_t maxHeigth               = 25;
	static constexpr   size_t maxDatalistCapacity     = 120;
	static constexpr   size_t errorValue              = -1;

	const bool isOptimised = false;

	struct Node;
	using  chart_t = Dlist<Node>;

private:
	size_t   _maxBufferSize{};
	chart_t  _chart;

	/* структура для коллекционирования значений в график*/
	struct Node {
		time_t  time{};
		int     length{};

		Node(time_t _time = 0, int _length = 0) : time(_time), length(_length) {}
		
		friend std::ostream& operator<< (std::ostream& _out, Node _elem) {
			_out << "Время: " << _elem.time << "\t | \t значение: " << _elem.length;
			return _out;
		}
	};

public:
	Chart() {
		_chart.out.mode(Mode::LIST_NUMBER);
	}

	static int64_t map(int64_t _num, int64_t _inMin, int64_t _inMax, int64_t _outMin, int64_t _outMax) {
		return (_num - _inMin) * (_outMax - _outMin) / (_inMax - _inMin) + _outMin;
	}

	int pushNode(time_t _time, size_t _bufferSize) {
		if (_maxBufferSize < _bufferSize) _maxBufferSize = _bufferSize;

		if (isOptimised) {
			/* Вызывается метод оптимизации списка */
		}
		_chart.push_back(Node(_time, _bufferSize));

		return 1;
	}

	inline size_t size(void) const {
		return _chart.size();
	}

	size_t getValue(time_t _time) const {
		if (!isOptimised && _time < size()) {
			return _chart.pos_back(_time).length;
		}

		return errorValue;
	}

	void out_values(void) const{		
		_chart.out();
	}

	// Графические символы: █ ▀ ▄
	void out_graphic(void) {
		size_t width   = min(size(), maxWidth);
		size_t heigth  = min(_maxBufferSize, maxHeigth);
		time_t totalTime = _chart.back().time;

		vector<string> outMatrix;
		outMatrix.resize(heigth);
		outMatrix[0] = "\t";
		for (size_t i = 1; i < outMatrix.size(); ++i) {
			outMatrix[i] = to_string(map(i - 1, 0, heigth - 2, 0, _maxBufferSize)) + "\t";
		}

		time_t currentTime{};
		size_t currentBufferSize{};
		size_t bufferPosOnChart{};
		for (size_t i = 1; i < width; ++i) {
			currentTime          = map(i - 1, 0, width - 2, 1, totalTime);
			currentBufferSize    = getValue(currentTime);
			bufferPosOnChart     = map(currentBufferSize, 0, _maxBufferSize, 1, heigth - 1);

			outMatrix[0] += to_string(currentTime) + "\t";

			for (size_t j = 1; j < outMatrix.size(); ++j) {
				if (j == bufferPosOnChart) outMatrix[j] += "█";
				outMatrix[j] += "\t";
			}
		}


		cout << endl;
		for (int i = outMatrix.size() - 1; i >= 0; i--) {
			cout << outMatrix[i] << endl;
			Sleep(5);
		}
	}
};


int main() {
	system("chcp 65001"); system("mode con cols=180 lines=3000");  system("cls");
#ifdef CHART_TEST
	
#else
	array<InfoNode, g_numIterations> collector;
	for (int iterator{ 0 }; iterator < g_numIterations; ++iterator) {
		cout << "[Проход номер " << iterator + 1 << "]" << endl << endl;
		Memory RAM;
		Files file(g_maxFileSize);
		Delay delay(g_maxInterval);
		Chart chart;

		if (g_isDetailedOut) 
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
			chart.pushNode(time + 1, RAM.getSegmentsCount());

			outStr += to_string(RAM.getMaxSegmentsCount()) += "\t\t";
			outStr += to_string(static_cast<double>(totalLength) / (time + 1));
			
			if (g_isDetailedOut) cout << outStr << endl;
		}
		collector.at(iterator) = InfoNode(static_cast<double>(totalLength) / g_totalTime, totalLength);

		cout << "[TIME OUT]" << endl;
		cout << "Максимальный объём занимаемой памяти: " << RAM.getMaxSegmentsCount() << endl << endl;

		cout << "[СВОДНЫЙ ГРАФИК РАБОТЫ ПРОГРАММЫ]" << endl << endl;
		chart.out_graphic();
		cout << endl << endl;
		Sleep(1000);
	}

	cout.precision(2);
	cout << "[Итоги после " << g_numIterations << " Итераций]" << endl;
	cout << "Номер\t";
	for (int i = 1; i <= g_numIterations; ++i) {
		cout << i << "\t\t";
	}
	cout << endl << "Среднее\t";
	for (int i = 0; i < g_numIterations; ++i) {
		cout << collector.at(i).mean << "\t\t";
	}
	cout << endl << "Макс.\t";
	for (int i = 0; i < g_numIterations; ++i){
		cout << collector.at(i).maxLength << "\t\t";
	}
	cout << endl;
#endif
	
	std::cout << "\nДля продолжения нажмите ввод";
	cin.get();
	return 0;
}