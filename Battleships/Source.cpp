#include "stdafx.h"
#include <iostream>
#include <conio.h>
#include <windows.h>

using namespace std;

const int WIDTH = 10; // ширина поля
const int HEIGHT = 10; // высота поля

const int EMPTY = -1; // пустая клетка
const int UNKNOWN = 0; // неизвестная клетка
const int BOOMED = 1; // клетка с раненым кораблём
const int SHIP = 2; // корабль

const int Xo = 5; // начальная клетка по горизонтали
const int Yo = 5; // начальная клетка по вертикали

const int SHIPS_COUNT = 10;
const int SHIPS_SIZES[] = { 4, 3, 3, 2, 2, 2, 1, 1, 1, 1 }; // размеры кораблей

struct Ship {
	int length; // длина корабля
	int x; // координата по горизонтали
	int y; // координата по вертикали
	int pos; // положение корабля

	int status; // количество неубитых частей корабля
};

int field[HEIGHT][WIDTH]; // поле
Ship ships[SHIPS_COUNT]; // корабли

// вывод поля на экран
void PrintField() {
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE); // получаем поток консоли

	SetConsoleCursorPosition(handle, { (short)Xo, (short)(Yo - 1) }); //  перемещаемся в начальную клетку

	cout << "+";
	for (int j = 0; j < WIDTH; j++)
		cout << '-';
	cout << "+";

	for (int i = 0; i < HEIGHT; i++) {
		SetConsoleCursorPosition(handle, { (short)Xo, (short)(Yo + i) }); //  перемещаемся в начальную клетку

		cout << '|';

		for (int j = 0; j < WIDTH; j++) {
			if (field[i][j] == UNKNOWN) {
				cout << ' ';
			}
			else if (field[i][j] == EMPTY) {
				cout << '.';
			}
			else if (field[i][j] == SHIP) {
				cout << '#';
			}
			else {
				cout << "@";
			}
		}

		cout << '|';
	}

	SetConsoleCursorPosition(handle, { (short)Xo, (short)(Yo + HEIGHT) }); //  перемещаемся в начальную клетку

	cout << "+";
	for (int j = 0; j < WIDTH; j++)
		cout << '-';
	cout << "+";
}

// можно ли разместить корабль ship в точку (x, y)
bool CanPlaceShip(Ship ship, int x, int y) {
	if (x < 0 || y < 0)
		return false;

	if (ship.pos == 0) { // корабль горизонтальный
		if ((y >= HEIGHT) || (x + ship.length > WIDTH))
			return false;
	}
	else { // корабль вертикальный
		if ((x >= WIDTH) || (y + ship.length > HEIGHT))
			return false;
	}

	int x1 = x - 1;
	int x2 = x + (ship.pos == 0 ? ship.length : 1);

	int y1 = y - 1;
	int y2 = y + (ship.pos == 1 ? ship.length : 1);

	for (int i = y1; i <= y2; i++)
		for (int j = x1; j <= x2; j++)
			if (i >= 0 && i < HEIGHT && j >= 0 && j < WIDTH && field[i][j] != UNKNOWN)
				return false;

	return true;
}

// удаление корабля с поля
void RemoveShip(Ship ship) {
	for (int j = 0; j < ship.length; j++) {
		int x = ship.x + !ship.pos * j;
		int y = ship.y + ship.pos * j;

		field[y][x] = UNKNOWN;
	}
}

// отрисовка корабля на поле
void DrawShip(Ship ship) {
	for (int j = 0; j < ship.length; j++) {
		int x = ship.x + !ship.pos * j;
		int y = ship.y + ship.pos * j;

		field[y][x] = SHIP;
	}

	PrintField(); // отрисока поля
}

// поиск пути для корабля
void FindWay(Ship& ship, int step) {
	int index = ship.y * WIDTH + ship.x; // находим индекс на поле для места

	do {
		index += step; // переходим к следующему месту на поле

		// обрабатываем граничные случаи перехода
		if (index < 0) {
			if (step == -1) {
				index = WIDTH * HEIGHT - 1;
			}
			else if (step == -WIDTH) {
				index = index - 1 + WIDTH * HEIGHT;
			}
		}

		if (index >= WIDTH * HEIGHT) {
			if (step == 1) {
				index = 0;
			}
			else if (step == WIDTH) {
				index = index + 1 - WIDTH * HEIGHT;
			}
		}

	} while (!CanPlaceShip(ship, index % WIDTH, index / WIDTH)); // повторяем, пока не найдём место для размещения

	// обновляем координаты корабля
	ship.x = index % WIDTH;
	ship.y = index / WIDTH;
}

// размещение корабля
void PlaceShip(Ship& ship) {
	do {
		RemoveShip(ship); // удаляем корабль с поля

		char c = _getch(); // считыванем клавишу

		// если вверх
		if (c == 72) {
			FindWay(ship, -WIDTH);
		}
		else if (c == 80) { // если вниз
			FindWay(ship, WIDTH);
		}
		else if (c == 75) { // если влево
			FindWay(ship, -1);
		}
		else if (c == 77) { // если вправо
			FindWay(ship, 1);
		}
		else if (c == 'r') {
			ship.pos = 1 - ship.pos;

			if (!CanPlaceShip(ship, ship.x, ship.y))
				ship.pos = 1 - ship.pos;
		}
		else if (c == 13) { // если ввод
			if (CanPlaceShip(ship, ship.x, ship.y))
				return;
		}

		DrawShip(ship); // рисуем корабль на поле
	} while (1);
}

// расстановка кораблей
void SetShips() {
	for (int i = 0; i < SHIPS_COUNT; i++) {
		ships[i].length = SHIPS_SIZES[i]; // сохраняем размер корабля
		ships[i].status = SHIPS_SIZES[i]; // изначально все корабли полностью живы
		ships[i].x = WIDTH - 1;
		ships[i].y = HEIGHT - 1;

		FindWay(ships[i], 1); // ищем досутпное место для корабля

		DrawShip(ships[i]); // рисуем корабль
		PlaceShip(ships[i]); // размещаем корабль
		DrawShip(ships[i]); // отрисовываем заного поставленный корабль
	}
}

// поиск индекса подбитого корабля
int GetShipIndex(int x, int y) {
	for (int i = 0; i < SHIPS_COUNT; i++) {
		for (int j = 0; j < ships[i].length; j++) {
			int x0 = ships[i].x + !ships[i].pos * j;
			int y0 = ships[i].y + ships[i].pos * j;

			if (x0 == x && y0 == y)
				return i;
		}
	}

	return -1;
}

// смерть корабля (обводка на поле)
void DieShip(Ship ship) {
	int x1 = ship.x - 1;
	int x2 = ship.x + (ship.pos == 0 ? ship.length : 1);

	int y1 = ship.y - 1;
	int y2 = ship.y + (ship.pos == 1 ? ship.length : 1);

	for (int i = y1; i <= y2; i++)
		for (int j = x1; j <= x2; j++)
			if (i >= 0 && i < HEIGHT && j >= 0 && j < WIDTH)
				field[i][j] = field[i][j] == BOOMED ? BOOMED : EMPTY;
}

// проверка на выстреленную клетку в точке (x, y)
bool IsBoom(int y, int x) {
	if (x < 0 || y < 0 || x >= WIDTH || y >= HEIGHT)
		return false;

	return field[y][x] == BOOMED;
}

void FindPoint(int& x, int& y) {
	for (int index = 0; index < WIDTH * HEIGHT; index++) {
		int i = index / WIDTH;
		int j = index % WIDTH;

		// если клетка неизведана
		if (field[i][j] == UNKNOWN || field[i][j] == SHIP) {
			x = j;
			y = i;

			if (IsBoom(i - 1, j - 1))
				return;

			if (IsBoom(i - 1, j))
				return;

			if (IsBoom(i - 1, j + 1))
				return;

			if (IsBoom(i, j - 1))
				return;

			if (IsBoom(i, j + 1))
				return;

			if (IsBoom(i + 1, j - 1))
				return;

			if (IsBoom(i + 1, j))
				return;

			if (IsBoom(i + 1, j + 1))
				return;
		}
	}

	do {
		x = rand() % WIDTH;
		y = rand() % HEIGHT;
	} while (field[y][x] == EMPTY || field[y][x] == BOOMED);
}

void Game() {
	int status = 0; // текущее количество убитых клеток корабля
	int total = 0;
	int moves = 0; // число ходов

	// находим общее количество клеток, которые нужно подбить
	for (int i = 0; i < SHIPS_COUNT; i++)
		total += SHIPS_SIZES[i];

	do {
		int x, y;
		moves++;

		FindPoint(x, y); // ищем место для удара

		if (field[y][x] == SHIP) { // если попали в корабль
			int index = GetShipIndex(x, y); // ищем корабль, в который попали

			ships[index].status--; // уменьшаем на 1 статус корабля

			// если корабль потоплен
			if (ships[index].status == 0)
				DieShip(ships[index]); // обрабатываем его "смерть"

			status++; // увеличиваем количество сбитых ячеек
			field[y][x] = BOOMED; // подбитая клетка
		}
		else { // иначе промах
			field[y][x] = EMPTY;
		}

		PrintField(); // выводим поле

		// выводим статистику
		HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE); // получаем поток консоли
		SetConsoleCursorPosition(handle, { (short)Xo + WIDTH + 4, (short)(Yo - 1) });
		cout << "Status:";
		SetConsoleCursorPosition(handle, { (short)Xo + WIDTH + 4, (short)(Yo) });
		cout << "Died: " << status << " / " << total; // выводим статус кораблей
		SetConsoleCursorPosition(handle, { (short)Xo + WIDTH + 4, (short)(Yo + 1) });
		cout << "Last move: " << x << " " << y; // выводим последний ход
		SetConsoleCursorPosition(handle, { (short)Xo + WIDTH + 4, (short)(Yo + 2) });
		cout << "Total moves: " << moves; // выводим количество ходов
		SetConsoleCursorPosition(handle, { 0, 0 });
		_getch(); // ожидаем нажатия клавишы
	} while (status < total); // повторяем, пока не будут сбиты все корабли
}

int main() {
	SetShips(); // расставляем корабли
	PrintField(); // выводим поле

	Game(); // выполняем игру
	system("pause");

	return 0;
}

