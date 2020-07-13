#include "stdafx.h"
#include <iostream>
#include <conio.h>
#include <windows.h>

using namespace std;

const int WIDTH = 10; // ������ ����
const int HEIGHT = 10; // ������ ����

const int EMPTY = -1; // ������ ������
const int UNKNOWN = 0; // ����������� ������
const int BOOMED = 1; // ������ � ������� �������
const int SHIP = 2; // �������

const int Xo = 5; // ��������� ������ �� �����������
const int Yo = 5; // ��������� ������ �� ���������

const int SHIPS_COUNT = 10;
const int SHIPS_SIZES[] = { 4, 3, 3, 2, 2, 2, 1, 1, 1, 1 }; // ������� ��������

struct Ship {
	int length; // ����� �������
	int x; // ���������� �� �����������
	int y; // ���������� �� ���������
	int pos; // ��������� �������

	int status; // ���������� �������� ������ �������
};

int field[HEIGHT][WIDTH]; // ����
Ship ships[SHIPS_COUNT]; // �������

// ����� ���� �� �����
void PrintField() {
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE); // �������� ����� �������

	SetConsoleCursorPosition(handle, { (short)Xo, (short)(Yo - 1) }); //  ������������ � ��������� ������

	cout << "+";
	for (int j = 0; j < WIDTH; j++)
		cout << '-';
	cout << "+";

	for (int i = 0; i < HEIGHT; i++) {
		SetConsoleCursorPosition(handle, { (short)Xo, (short)(Yo + i) }); //  ������������ � ��������� ������

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

	SetConsoleCursorPosition(handle, { (short)Xo, (short)(Yo + HEIGHT) }); //  ������������ � ��������� ������

	cout << "+";
	for (int j = 0; j < WIDTH; j++)
		cout << '-';
	cout << "+";
}

// ����� �� ���������� ������� ship � ����� (x, y)
bool CanPlaceShip(Ship ship, int x, int y) {
	if (x < 0 || y < 0)
		return false;

	if (ship.pos == 0) { // ������� ��������������
		if ((y >= HEIGHT) || (x + ship.length > WIDTH))
			return false;
	}
	else { // ������� ������������
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

// �������� ������� � ����
void RemoveShip(Ship ship) {
	for (int j = 0; j < ship.length; j++) {
		int x = ship.x + !ship.pos * j;
		int y = ship.y + ship.pos * j;

		field[y][x] = UNKNOWN;
	}
}

// ��������� ������� �� ����
void DrawShip(Ship ship) {
	for (int j = 0; j < ship.length; j++) {
		int x = ship.x + !ship.pos * j;
		int y = ship.y + ship.pos * j;

		field[y][x] = SHIP;
	}

	PrintField(); // �������� ����
}

// ����� ���� ��� �������
void FindWay(Ship& ship, int step) {
	int index = ship.y * WIDTH + ship.x; // ������� ������ �� ���� ��� �����

	do {
		index += step; // ��������� � ���������� ����� �� ����

		// ������������ ��������� ������ ��������
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

	} while (!CanPlaceShip(ship, index % WIDTH, index / WIDTH)); // ���������, ���� �� ����� ����� ��� ����������

	// ��������� ���������� �������
	ship.x = index % WIDTH;
	ship.y = index / WIDTH;
}

// ���������� �������
void PlaceShip(Ship& ship) {
	do {
		RemoveShip(ship); // ������� ������� � ����

		char c = _getch(); // ���������� �������

		// ���� �����
		if (c == 72) {
			FindWay(ship, -WIDTH);
		}
		else if (c == 80) { // ���� ����
			FindWay(ship, WIDTH);
		}
		else if (c == 75) { // ���� �����
			FindWay(ship, -1);
		}
		else if (c == 77) { // ���� ������
			FindWay(ship, 1);
		}
		else if (c == 'r') {
			ship.pos = 1 - ship.pos;

			if (!CanPlaceShip(ship, ship.x, ship.y))
				ship.pos = 1 - ship.pos;
		}
		else if (c == 13) { // ���� ����
			if (CanPlaceShip(ship, ship.x, ship.y))
				return;
		}

		DrawShip(ship); // ������ ������� �� ����
	} while (1);
}

// ����������� ��������
void SetShips() {
	for (int i = 0; i < SHIPS_COUNT; i++) {
		ships[i].length = SHIPS_SIZES[i]; // ��������� ������ �������
		ships[i].status = SHIPS_SIZES[i]; // ���������� ��� ������� ��������� ����
		ships[i].x = WIDTH - 1;
		ships[i].y = HEIGHT - 1;

		FindWay(ships[i], 1); // ���� ��������� ����� ��� �������

		DrawShip(ships[i]); // ������ �������
		PlaceShip(ships[i]); // ��������� �������
		DrawShip(ships[i]); // ������������ ������ ������������ �������
	}
}

// ����� ������� ��������� �������
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

// ������ ������� (������� �� ����)
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

// �������� �� ������������ ������ � ����� (x, y)
bool IsBoom(int y, int x) {
	if (x < 0 || y < 0 || x >= WIDTH || y >= HEIGHT)
		return false;

	return field[y][x] == BOOMED;
}

void FindPoint(int& x, int& y) {
	for (int index = 0; index < WIDTH * HEIGHT; index++) {
		int i = index / WIDTH;
		int j = index % WIDTH;

		// ���� ������ ����������
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
	int status = 0; // ������� ���������� ������ ������ �������
	int total = 0;
	int moves = 0; // ����� �����

	// ������� ����� ���������� ������, ������� ����� �������
	for (int i = 0; i < SHIPS_COUNT; i++)
		total += SHIPS_SIZES[i];

	do {
		int x, y;
		moves++;

		FindPoint(x, y); // ���� ����� ��� �����

		if (field[y][x] == SHIP) { // ���� ������ � �������
			int index = GetShipIndex(x, y); // ���� �������, � ������� ������

			ships[index].status--; // ��������� �� 1 ������ �������

			// ���� ������� ��������
			if (ships[index].status == 0)
				DieShip(ships[index]); // ������������ ��� "������"

			status++; // ����������� ���������� ������ �����
			field[y][x] = BOOMED; // �������� ������
		}
		else { // ����� ������
			field[y][x] = EMPTY;
		}

		PrintField(); // ������� ����

		// ������� ����������
		HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE); // �������� ����� �������
		SetConsoleCursorPosition(handle, { (short)Xo + WIDTH + 4, (short)(Yo - 1) });
		cout << "Status:";
		SetConsoleCursorPosition(handle, { (short)Xo + WIDTH + 4, (short)(Yo) });
		cout << "Died: " << status << " / " << total; // ������� ������ ��������
		SetConsoleCursorPosition(handle, { (short)Xo + WIDTH + 4, (short)(Yo + 1) });
		cout << "Last move: " << x << " " << y; // ������� ��������� ���
		SetConsoleCursorPosition(handle, { (short)Xo + WIDTH + 4, (short)(Yo + 2) });
		cout << "Total moves: " << moves; // ������� ���������� �����
		SetConsoleCursorPosition(handle, { 0, 0 });
		_getch(); // ������� ������� �������
	} while (status < total); // ���������, ���� �� ����� ����� ��� �������
}

int main() {
	SetShips(); // ����������� �������
	PrintField(); // ������� ����

	Game(); // ��������� ����
	system("pause");

	return 0;
}

