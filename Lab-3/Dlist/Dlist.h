///////////////////////////////////////////////////////////////////
//                                                               //
//                Doubly linked list v.1.0.3                     //
//                Project made by SemafonKA                      //
//          Me on gitHub (https://github.com/SemafonKA)          //
//                                                               //
///////////////////////////////////////////////////////////////////

#pragma once
#include <iostream>
#include <string>

template<typename T>
inline void swap(T& first, T& second) {
	T tmp = first;
	first = second;
	second = tmp;
}

enum class Mode {
	LIST                = 0,
	LIST_ADRESS         = 1,
	LIST_ADR_FULL       = 2,
	LIST_NUMBER         = 3,
	LIST_NUM_ADR        = 4,
	LIST_NUM_ADR_FULL   = 5,
	NO_OUT              = 6
};
template<typename T>
class Dlist {
private:
	class Node {
	public:
		T data{};
		Node* next{};
		Node* prev{};
	};

	Node* m_front{};
	Node* m_back{};

	int m_listSize{};

	/* Класс вывода данных в консоль */
	class OutClass {
	private:
		Dlist* m_object{};			// Привязка к объекту списка
		Mode m_mode{ Mode::LIST };	// Режим вывода

	public:
		OutClass(Dlist* object) {
			m_object = object;
		}

		const int defaultLength = 15;		// Количество данных, выводимых в строку по умолчанию
		int lineLength{ defaultLength };	// Количество данных, выводимых в строку

		/* Методы изменения режима вывода */
		void mode(Mode newMode) { m_mode = newMode; }
		Mode mode(void) { return m_mode; }

		/* Метод вывода данных указанного диапазона */
		bool operator() (int start = 0, int end = -1) const {
			if (end == -1) end = m_object->m_listSize - 1;
			if (m_mode != Mode::NO_OUT && !m_object->isEmpty() && end < m_object->m_listSize) {
				Node* currentMember = m_object->listSearch(start);
				std::cout << std::endl;
				for (int i = start; i <= end; ++i) {
					if (m_mode == Mode::LIST_NUMBER || m_mode == Mode::LIST_NUM_ADR ||
						m_mode == Mode::LIST_NUM_ADR_FULL) {
						std::cout << i << ")\t";
					}

					std::cout << currentMember->data << "\t";
					if (m_mode == Mode::LIST_ADRESS || m_mode == Mode::LIST_ADR_FULL ||
						m_mode == Mode::LIST_NUM_ADR || m_mode == Mode::LIST_NUM_ADR_FULL) {
						std::cout << "Звено: [" << currentMember << "]\t";
					}
					if (m_mode == Mode::LIST_ADR_FULL || m_mode == Mode::LIST_NUM_ADR_FULL) {
						std::cout << "Предыдущее: [" << currentMember->prev << "]\t" <<
							"Следующее: [" << currentMember->next << "]";
					}

					if ((i + 1) % lineLength == 0 && m_mode == Mode::LIST || m_mode != Mode::LIST) std::cout << std::endl;
					currentMember = currentMember->next;
				}
				std::cout << std::endl;
				return true;
			}
			else return false;
		}
	};

	void listCheck() const {
		Node* member = m_front;
		for (int i = 0; i < m_listSize; ++i) {
			if (member == nullptr) {
				throw std::logic_error("ERR: error in resize list");
			}
			member = member->next;
		}
	}

	Node* listSearch(int pos) const {
		if (pos >= m_listSize) {
			throw std::logic_error("ERR: out of range");
		}
		if (pos < 0)				return nullptr;
		if (pos == 0)				return m_front;
		if (pos == m_listSize - 1)	return m_back;

		Node* member{};
		if (pos <= m_listSize / 2) {
			member = m_front;

			for (int i = 0; i < pos; ++i) {
				member = member->next;
			}
		}
		else {
			member = m_back;
			for (int i = m_listSize - 1; i > pos; --i) {
				member = member->prev;
			}
		}

		return member;
	}

public:
	Dlist() {}
	Dlist(const Dlist<T>& _right) {
		for (int i = 0; i < _right.size(); ++i) {
			this->push_back(_right.at(i));
		}
	}
	~Dlist() { clear();	}

	/* Поток вывода данных */
	OutClass out{ this };

	/* Удаляет все члены списка */
	void clear() {
		if (!isEmpty()) {
			Node* member = m_front;
			Node* nextMember;
			for (int i = 0; i < m_listSize; i++) {
				nextMember = member->next;
				delete member;
				member = nextMember;
			}
			m_front = nullptr;
			m_back = nullptr;
			m_listSize = 0;
		}
	}

	/* Вернуть значение списка в позиции pos */
	T& pos_back(int pos) {
		Node* member = listSearch(pos);
		return member->data;
	}
	/* Вернуть значение списка в позиции pos */
	const T& pos_back(int pos) const {
		Node* member = listSearch(pos);
		return member->data;
	}
	/* Вернуть значение списка в позиции pos */
	T& operator[] (int pos) {
		return pos_back(pos);
	}
	/* Вернуть значение списка в позиции pos */
	const T& operator[] (int pos) const {
		return pos_back(pos);
	}
	/* Вернуть значение списка в позиции pos */
	T& at(int pos) {
		return pos_back(pos);
	}
	/* Вернуть значение списка в позиции pos */
	const T& at(int pos) const {
		return pos_back(pos);
	}

	/* Установить значение списка в позиции pos */
	Dlist& set(int pos, T data) {
		Node* member = listSearch(pos);
		member->data = data;
		return (*this);
	}

	/* Вставить новый член списка после позиции pos */
	Dlist& push(int pos, T data = T{}) {
		Node tmpNode{ T{}, m_front };								// Звено, предшествующее первому звену 
		Node* member = (pos == -1) ? &tmpNode : listSearch(pos);	// Звено, предшествующее новому звену	
		Node* nextMember = member->next;

		member->next = new Node;
		member->next->prev = (pos == -1) ? (nullptr) : (member);
		member->next->data = data;

		member->next->next = nextMember;
		if (nextMember == nullptr) { m_back = member->next; }
		else if (pos == -1) { nextMember->prev = member->next; }
		if (pos == -1) { m_front = member->next; }
		++m_listSize;

		return (*this);
	}

	/* Удалить член списка из заданной позиции pos */
	T pop(int pos) {
		if (isEmpty()) {
			throw std::logic_error("ERR: list is empty!");
		}
		Node* prevMember = listSearch(pos - 1);
		if (prevMember != nullptr) {				// Если нужное звено находится не в начале
			Node* member = prevMember->next;
			T data = member->data;

			prevMember->next = member->next;
			if (pos != m_listSize - 1) member->next->prev = member->prev;
			delete member;

			if (pos == m_listSize - 1) m_back = prevMember;
			--m_listSize;

			return data;
		}
		else {										//	if (prevMember != nullptr)									
			T data = m_front->data;
			Node* nextMember = m_front->next;
			delete m_front;

			m_front = nextMember;
			if (m_listSize > 1) m_front->prev = nullptr;
			--m_listSize;

			return data;
		}
	}

	/* Изменить размер списка */
	int resize(unsigned int _size) {
		if (_size == m_listSize)   return 2;
		if (_size == 0) { clear(); }

		if (_size > m_listSize) {
			while (m_listSize != _size) push_back();
		}
		else if (_size < m_listSize) {
			while (m_listSize != _size) pop_back();
		}
	}

	/* Возвращает размер списка */
	unsigned int size() const { return m_listSize; }

	/* Возвращает, пустой ли список или нет */
	bool isEmpty() const { return m_listSize == 0; }

	/* Добавляет новое звено в начало списка */
	Dlist& push_front(T data = T{}) { return push(-1, data); }

	/* Добавляет новое звено в конец списка */
	Dlist& push_back(T data = T{}) { return push(m_listSize - 1, data); }

	/* Возвращает значение в последнем звене списка (ссылку) */
	T& back() { return pos_back(m_listSize - 1); }

	/* Возвращает значение в первом звене списка (ссылку) */
	T& front() { return pos_back(0); }

	/* Удаляет последнее звено списка */
	T pop_back() { return pop(m_listSize - 1); }

	/* Удаляет первое звено списка */
	T pop_front() { return pop(0); }

	/* Меняет два звена списка местами (без копирования) */
	bool swap(int pos1, int pos2) {
		if (pos1 == pos2)			return true;
		if (pos1 > pos2)			swap(pos1, pos2);
		if (pos2 >= m_listSize)		return false;
		if (pos1 < 0)				return false;

		Node* elem1 = listSearch(pos1);
		Node* elem2 = listSearch(pos2);
		Node* prev1 = elem1->prev;
		Node* next1 = elem1->next;
		Node* prev2 = elem2->prev;
		Node* next2 = elem2->next;

		bool neighbors{ next1 == elem2 };
		elem1->next = next2;
		if (!neighbors) elem1->prev = prev2;
		else elem1->prev = elem2;

		if (!neighbors) elem2->next = next1;
		else elem2->next = elem1;
		elem2->prev = prev1;

		if (prev1) prev1->next = elem2;
		else m_front = elem2;
		if (!neighbors) next1->prev = elem2;

		if (next2) next2->prev = elem1;
		else m_back = elem1;
		if (!neighbors) prev2->next = elem1;

		return true;
	}

	/* Копирует список из другого списка (правого операнда) */
	Dlist& operator=(const Dlist<T>& _right) {
		if (this == &_right) return *this;

		this->clear();
		for (int i = 0; i < _right.size(); ++i) {
			this->push_back(_right.at(i));
		}
		return *this;
	}
};


/* Построение пирамиды для сортировки */
template <typename T>
void pyramid(Dlist<T>& mas, const int SIZE, bool isPyramidBuilded = false) {
	int k_max{}, mid{ SIZE / 2 - 1 };
	int i{}, k{}, tmp{};
	for (i = mid; i >= 0; i--) {
		if (isPyramidBuilded) i = 0;													// Если пирамида уже была построена, остаётся просеивать только первый элемент
		for (k = i; k <= mid;) {
			if (k == mid && SIZE % 2 == 0) k_max = k * 2 + 1;
			else k_max = mas[k * 2 + 1] >= mas[k * 2 + 2] ? k * 2 + 1 : k * 2 + 2;
			if (mas[k] < mas[k_max]) {
				mas.swap(k, k_max);
				k = k_max;
			}
			else break;
		}
	}
}
/* Пирамидальная сортировка */
template <typename T>
int sort(Dlist<T>& _list) {
	bool isPyramidBuilded = false;
	int i{};
	for (i = _list.size(); i > 1; i--) {
		pyramid(_list, i, isPyramidBuilded);
		isPyramidBuilded = true;
		_list.swap(0, i - 1);
	}
	return 1;
}