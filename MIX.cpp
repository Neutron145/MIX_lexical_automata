#include <iostream>
#include <fstream>
#include <cstring>
#include "declarations.h"
#include <vector>

using namespace std;

//Класс слова, которое является ячейкой памяти. Может быть упаковано или распаковано
class Word {
private:
	bool _isPacked;
	int* _cells;

public:
	Word(bool isPacked = false, int* cells = nullptr) {
		_isPacked = isPacked;
		_cells = new int[6];
		if (cells != nullptr) {
			if (_isPacked && cells != nullptr) {
				int i = 0;
				for (; i < 4; _cells[i] = cells[i++]);
				for (; i < 6; _cells[i++] = 0);
			}
			else if (cells != nullptr) {
				for (int i = 0; i < 6; _cells[i] = cells[i++]);
			}
			else for (int i = 0; i < 6; _cells[i++] = 0);
		}
	}
	~Word() {

	}
	int getADDRS() { return _cells[0]; }
	int getSpec() { return _cells[1]; }
	int getMod() { return _cells[2]; }
	int getOp() { return _cells[3]; }
};

//Регистр rA
int A[6];
//Регистр rX
int X[6];
//Индексные регистры
int I1[2], I2[2], I3[2], I4[2], I5[2], I6[2];
//Адрес перехода
int rJ[2];

//Память MIX
Word mem[100];

//Список слов и их лексем
struct lexemList {
	//Тип лексемы
	lexType type;
	//Само слово
	char* word;
	//Указатель на следующий элемент списка
	lexemList* next;
	lexemList(lexType t = lWl, char* w = nullptr) {
		type = t;
		word = w;
		next = nullptr;
	}
};

//Список меток
struct markList {
	char* mark;
	markList* next;
	markList(char* m = nullptr) {
		mark = m;
		next = nullptr;
	}
};

//Список меток
markList* marks = new markList;

markList* pushback(markList*& head, markList* newLex) {
	markList* p = head;
	while (p->next) p = p->next;
	p->next = newLex;
	return p;
}

void pop(markList* head) {
	if (head) {
		while (head->next) {
			markList* l = head->next;
			head->next = l->next;
			delete l;
		}
		delete head;
	}
}

//Добавление нового элемента в конец списка лексем
lexemList* pushback(lexemList*& head, lexemList* newLex) {
	lexemList* p = head;
	while (p->next) p = p->next;
	p->next = newLex;
	return p;
}

//Удаление списка лексем
void pop(lexemList* head) {
	if (head) {
		while (head->next) {
			lexemList* l = head->next;
			head->next = l->next;
			delete l;
		}
		delete head;
	}
}

//Вывод списка лексем
void print(lexemList* head, ofstream& fout) {
	lexemList* p = head->next;
	while (p) {
		if (p->word) fout << p->word << "[" << lexTypeStr[p->type] << "] ";
		if (p->type == lEnd) fout << '\n';
		p = p->next;
	}
	fout << '\n';
}

//Определение типа символа
signals checkSymbol(char c) {
	if (c >= '0' && c <= '9') return Digits;
	else if (c >= 'A' && c <= 'Z') return Alpha;
	else if (c == ',') return Comma;
	else if (c == '(') return Bl;
	else if (c == ')') return Br;
	else if (c == '*') return Star;
	else if (c == '+') return Plus;
	else if (c == ':') return Colon;
	else if (c == ' ' || c == '\0' || c == '\n' || c == '\r') return Space;
	else return Wrong;
}

//Определние лексемы слова
lexType checkLexType(lexStates state) {
	switch (state)
	{
	case lA:
		return lMark;
		break;
	case lB:
		return lAdrrs;
		break;
	case lD:
		return lSpec;
		break;
	case lI:
		return lMod;
		break;
	default:
		return lWl;
		break;
	}
}

//Переводит слово в нижний регистр
char* toLower(char* word) {
	while (*(word - 1)) {
		*word = tolower(*word);
		word++;
	}
	return word;
}

//------------------Лексический анализатор------------------
lexemList* lexAnalyzer(char* text) {
	//Список для хранения всех лексем
	lexemList* head = new lexemList();
	//Указатель на начало слова
	char* wordBeg = text;
	int p = 0;
	//Текущее и прошлое состояние автомата
	lexStates curState = lS, lastState;
	do {
		//Смена состояния
		lastState = curState;
		curState = lexTable[checkSymbol(*text)][curState];
		//Добавление лексемы в список
		if (curState == lS && lastState != lS) {
			char* word = new char[text - wordBeg + 1];
			strncpy(word, wordBeg, text - wordBeg);
			word[text - wordBeg] = '\0';
			lexemList* lex = new lexemList(checkLexType(lastState), word);
			if (lex->type == lMark) {
				toLower(lex->word);
				for (int i = 0; i < 64; i++) {
					for (int j = 0; j < 6; j++) {
						if (opTypeStr[i][j] != nullptr) {
							if (strcmp(lex->word, opTypeStr[i][j]) == 0) {
								lex->type = lOp;
							}
						}
					}
				}
			}


			pushback(head, lex);
		}
		else text++;
		if (*text == '\n') {
			pushback(head, new lexemList(lEnd, (char*)" "));
		}
		//Отмечает лексемой конец строки
		if (curState == lS) wordBeg = text;
	} while (*(text-1));
	pushback(head, new lexemList(lEnd, (char*)" "));
	return head;
}


//------------------Синтаксический анализатор------------------
int syntaxAnalyzer(lexemList* lexems, ofstream& fout) {
	//Текущее и прошлое состояние автомата
	syntStates curState = sS, lastState = sS;
	//Указатель на лексему для итерирования
	lexemList* p = lexems->next;
	//Номер текущей лексемы
	int line = 0;
	while (p) {
		//Смена состояния автомата
		lastState = curState;
		curState = syntTable[p->type][curState];
		//Фиксирование ошибки
		if (curState == Er) {
			//Определение строки 
			fout << "Error on line " << ++line << "\nExpected: ";
			for (int lexType = lSpec; lexType != lEnd; lexType++) {
				if (syntTable[lexType][lastState] != Er) fout << lexTypeStr[lexType] << ' ';
			}
			fout << '\n';
			return line;
		}
		if (p->type == lEnd) line++;
		p = p->next;
	}
	fout << "OK" << '\n';
	return -1;
}


//--------------------Перевод в машинный код---------------------
void toMachineCode(lexemList* lexems, ofstream& fout) {
	//Указатель для итерации по списку лексем
	lexemList* p = lexems->next;
	//Упакованное слово, в котором хранится команда в машинном коде
	int command[4];
	//Значения I и F по умолчанию 
	command[1] = 0;
	command[2] = 5;
	//Номер команды для записи в память
	int pos = 0;
	while (p) {
		//Перевод команды в машинный код
		switch (p->type)
		{
		case lAdrrs:  //ADDRS
			command[0] = atoi(p->word);
			break;
		case lSpec:  //I
			command[1] = (int)p->word[1] - '0';
			break;
		case lMod:  //F
			command[2] = 8 * ((int)p->word[1] - '0') + ((int)p->word[3] - '0');
			break;
		case lEnd:
			break;
		default:	//OP
			
			toLower(p->word);
			for (int i = 0; i < 64; i++) {
				for (int j = 0; j < 6; j++) {
					if (opTypeStr[i][j] != nullptr) {
						if (strcmp(p->word, opTypeStr[i][j]) == 0) {
							command[3] = i;
							if (i == 5) {
								command[2] = j;
							}
						}
					}
				}
			}

			break;
		}
		//Если проитерировали всю команду, то создаем экземпляр Word и добавляем его в память
		if (p->type == lEnd) {
			Word w = Word(true, command);
			fout << "|" << w.getADDRS() << "\t|" << w.getSpec() << "\t|" <<
				w.getMod() << "\t|" << w.getOp() << "\t|\n";
			mem[pos] = w;
			cout << "|" << mem[pos].getADDRS() << "\t|" << mem[pos].getSpec() << "\t|" <<
				mem[pos].getMod() << "\t|" << mem[pos].getOp() << "\t|\n";
			pos++;
			//Сбрасываем значения по умолчанию 
			command[1] = 0;
			command[2] = 5;
		}

		p = p->next;
	}

}

int main()
{	
	//------------------Чтение данных------------------
	ifstream fin("input.txt", ios::binary);
	size_t size = 0;
	fin.seekg(0, ios::end);
	size = fin.tellg();
	fin.seekg(0, ios::beg);
	char* text = new char[size + 1];
	fin.getline(text, size + 1, '\0');
	fin.close();

	
	//------------Лексический и синтаксический анализ данных------------
	ofstream fout("output.txt", ios::binary);
	lexemList* head = lexAnalyzer(text);
	int pos = syntaxAnalyzer(head, fout);

	//-------------Перевод в машинный код---------------
	if (pos == -1) {
		toMachineCode(head, fout);
	}

	//-------------Выполнение машинного кода--------------


	print(head, fout);
	fout.close();


	pop(head);
	return 0;
}
