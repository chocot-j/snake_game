#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <conio.h>
#include <time.h>

typedef struct Coord {
	int x, y;
	struct Coord* prev;
	struct Coord* next;
} Coord;

typedef struct {
	Coord* head;
	Coord* body_front;
	Coord* body_rear;
	int prev_x, prev_y;
	int length;
	char dirc;
} Snake;

typedef enum { NOCURSOR, SOLIDCURSOR, NORMALCURSOR } CURSOR_TYPE;
void gotoxy(int, int);
void setcursortype(CURSOR_TYPE);
void add_body(Snake*);
void control(Snake*, int*, int*);
void move_snake(Snake*, int, int);
void init_snake(Snake*, int, int);
void make_map();
void start_page(int*);
int is_crash(Snake*);
int is_eat(Snake*, int, int);
Coord* make_food(Snake*, Coord*);

int main(void) {
	setcursortype(NOCURSOR);								// 커서 감추기
	system("title snake_game(ver_2)");					// 콘솔 이름
	system("mode con: cols=60 lines=22");					// 콘솔 크기
	srand((unsigned int)time(NULL));						// 랜덤 이용하기 위한 초기화(반복문 안에 있으면 오류남)
	int speed = 60;											// 뱀 속력
	start_page(&speed);
	make_map();

	Snake snake;
	Coord food;
	food.next = food.prev = NULL;
	food.x = 30; food.y = 10;								// 처음 음식 위치
	gotoxy(food.x, food.y);
	printf("♥");
	int x = 10, y = 10;										// 시작 좌표
	char ch = 'd';
	init_snake(&snake, x, y);
	gotoxy(48, 10);
	printf("Score: 0");
	if (speed == 90) {
		gotoxy(48, 8);
		printf("Easy Mode");
	}
	else if (speed == 60) {
		gotoxy(48, 8);
		printf("Normal Mode");
	}
	else {
		gotoxy(48, 8);
		printf("Hard Mode");
	}
	Sleep(1000);

	while (1) {
		if (_kbhit()) {
			ch = _getch();
			switch (ch) {
			case 'w':
				if (snake.dirc != 's')
					snake.dirc = 'w';
				break;
			case 's':
				if (snake.dirc != 'w')
					snake.dirc = 's';
				break;
			case 'a':
				if (snake.dirc != 'd')
					snake.dirc = 'a';
				break;
			case 'd':
				if (snake.dirc != 'a')
					snake.dirc = 'd';
				break;
			case 'q':
				snake.dirc = 'q';
				break;
			}
		}
		control(&snake, &x, &y);
		move_snake(&snake, x, y);
		if (is_eat(&snake, food.x, food.y)) {
			add_body(&snake);
			make_food(&snake, &food);
		}
		if (is_crash(&snake)) {
			system("cls");
			make_map();
			gotoxy(18, 10);
			printf("Game Over");
			gotoxy(48, 10);
			printf("Score: %d", snake.length * 10);
			break;
		}
		gotoxy(48, 10);
		printf("Score: %d", snake.length * 10);
		Sleep(speed);
	}
	while (1) {
		if (_kbhit()) {
			ch = _getch();
			if (ch == 'q')
				break;
		}
	}
	return 0;
}

void init_snake(Snake* snake, int x, int y) {
	snake->head = (Coord*)malloc(sizeof(Coord));
	snake->head->prev = snake->head->next = NULL;
	snake->head->x = x;
	snake->head->y = y;
	gotoxy(snake->head->x, snake->head->y);
	printf("■");

	snake->body_front = (Coord*)malloc(sizeof(Coord));
	snake->body_rear = (Coord*)malloc(sizeof(Coord));
	snake->body_front->prev = snake->body_rear->next = NULL;
	snake->body_front->next = snake->body_rear;
	snake->body_rear->prev = snake->body_front;

	snake->body_front->x = x - 2;
	snake->body_front->y = y;
	snake->body_rear->x = x - 4;
	snake->body_rear->y = y;
	snake->length = 0;
	snake->dirc = 'd';
	gotoxy(snake->body_front->x, snake->body_front->y);
	printf("□");
	gotoxy(snake->body_rear->x, snake->body_rear->y);
	printf("□");
}

void make_map() {
	int map[22][22] = { 0 };
	for (int i = 0; i < 22; i++) {
		map[i][0] = map[0][i] = map[i][21] = map[21][i] = 1;
	}
	for (int i = 0; i < 22; i++) {
		for (int j = 0; j < 22; j++) {
			if (map[i][j] == 1) {
				gotoxy(i * 2, j);
				printf("▒");
			}
		}
	}
}

void control(Snake* snake, int* x, int* y) {
	switch (snake->dirc) {
	case 'w':
		if (*y > 1)
			(*y)--;
		break;
	case 's':
		if (*y < 20)
			(*y)++;
		break;
	case 'a':
		if (*x > 2)
			*x -= 2;
		break;
	case 'd':
		if (*x < 40)
			*x += 2;
		break;
	case 'q':
		system("cls");
		exit(1);
	}
}

void add_body(Snake* snake) {
	Coord* newbody = (Coord*)malloc(sizeof(Coord));
	newbody->next = NULL;
	newbody->prev = snake->body_rear;
	newbody->x = snake->prev_x;										// 이전 꼬리 위치에 새 꼬리 (꼬리 먼저 없어지니까)
	newbody->y = snake->prev_y;

	snake->body_rear->next = newbody;
	snake->body_rear = newbody;
	snake->length++;
	gotoxy(snake->body_rear->x, snake->body_rear->y);
	printf("□");
}

void move_snake(Snake* snake, int x, int y) {
	snake->prev_x = snake->body_rear->x;							// 몸의 꼬리가 먼저 없어짐
	snake->prev_y = snake->body_rear->y;
	gotoxy(snake->body_rear->x, snake->body_rear->y);
	printf("  ");
	Coord* temp_rear = (Coord*)malloc(sizeof(Coord));
	temp_rear = snake->body_rear;
	snake->body_rear = temp_rear->prev;
	snake->body_rear->next = NULL;
	free(temp_rear);
	temp_rear = NULL;

	snake->head->next = snake->body_front;							// 이전 머리가 몸의 처음이 됨
	snake->body_front->prev = snake->head;
	snake->body_front = snake->head;
	gotoxy(snake->body_front->x, snake->body_front->y);
	printf("□");

	Coord* temp_head = (Coord*)malloc(sizeof(Coord));				// 새로운 머리 생성
	temp_head->prev = temp_head->next = NULL;
	temp_head->x = x;
	temp_head->y = y;
	snake->head = temp_head;
	gotoxy(x, y);
	printf("■");
}

int is_crash(Snake* snake) {
	if ((snake->head->x < 2 || snake->head->x >40) || (snake->head->y < 1 || snake->head->y >20)) {
		return 1;
	}
	Coord* temp = snake->body_front;
	while (temp != NULL) {
		int x, y;
		x = temp->x;
		y = temp->y;
		temp = temp->next;
		if (snake->head->x == x && snake->head->y == y) {
			return 1;
		}
	}
	return 0;
}

int is_eat(Snake* snake, int x, int y) {
	if (snake->head->x == x && snake->head->y == y) {
		return 1;
	}
	return 0;
}

Coord* make_food(Snake* snake, Coord* food) {
	food->x = (rand() % 20 + 1) * 2;
	food->y = rand() % 20 + 1;
	if (is_eat(snake, food->x, food->y)) {							// 머리랑 겹치게 생성되면 다시 생성
		make_food(snake, food);
	}
	Coord* temp = snake->body_front;								// 몸이랑 겹치게 생성되면 다시 생성
	while (temp != NULL) {
		if ((food->x == temp->x) && (food->y == temp->y)) {
			make_food(snake, food);
		}
		temp = temp->next;
	}

	gotoxy(food->x, food->y);
	printf("♥");
	return food;
}

void start_page(int* speed) {
	int map[11][24] = {
		{1,1,1,1,0,1,0,0,1,0,1,1,1,1,0,1,0,0,1,0,1,1,1,1},
		{1,0,0,0,0,1,0,0,1,0,1,0,0,1,0,1,0,1,0,0,1,0,0,0},
		{1,1,1,1,0,1,1,0,1,0,1,1,1,1,0,1,1,0,0,0,1,1,1,1},
		{0,0,0,1,0,1,0,1,1,0,1,0,0,1,0,1,0,1,0,0,1,0,0,0},
		{1,1,1,1,0,1,0,0,1,0,1,0,0,1,0,1,0,0,1,0,1,1,1,1},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		{0,1,1,1,1,0,0,1,1,1,1,0,0,1,0,0,1,0,0,1,1,1,1,0},
		{0,1,0,0,0,0,0,1,0,0,1,0,0,1,1,1,1,0,0,1,0,0,0,0},
		{0,1,0,1,1,0,0,1,1,1,1,0,0,1,0,0,1,0,0,1,1,1,1,0},
		{0,1,0,0,1,0,0,1,0,0,1,0,0,1,0,0,1,0,0,1,0,0,0,0},
		{0,1,1,1,1,0,0,1,0,0,1,0,0,1,0,0,1,0,0,1,1,1,1,0},
	};
	for (int i = 0; i < 11; i++) {
		for (int j = 0; j < 24; j++) {
			if (map[i][j] == 1) {
				gotoxy(j * 2 + 6, i + 2);
				printf("■");
			}
		}
	}
	gotoxy(26, 16);
	printf("start");
	gotoxy(26, 17);
	printf("level");
	gotoxy(23, 16);
	printf("▶");
	int big_y = 16;
	int small_y;
	if (*speed == 90) {
		small_y = 19;
	}
	else if (*speed == 60) {
		small_y = 20;
	}
	else {
		small_y = 21;
	}
	char ch;
	while (1) {
		if (_kbhit()) {
			ch = _getch();
			if (ch == 'w') {
				gotoxy(23, 17);
				printf("  ");
				gotoxy(23, 16);
				printf("▶");
				big_y = 16;
			}
			if (ch == 's') {
				gotoxy(23, 16);
				printf("  ");
				gotoxy(23, 17);
				printf("▶");
				big_y = 17;
			}
			if (ch == 13) {
				if (big_y == 16) {
					system("cls");
					break;
				}
				else {
					gotoxy(23, 17);
					printf("  ");
					gotoxy(26, 19);
					printf("Easy");
					gotoxy(26, 20);
					printf("Normal");
					gotoxy(26, 21);
					printf("Hard");
					gotoxy(23, small_y);
					printf("▶");
					while (1) {
						if (_kbhit()) {
							ch = _getch();
							if (ch == 'w' && small_y > 19) {
								small_y--;
							}
							if (ch == 's' && small_y < 21) {
								small_y++;
							}
							gotoxy(23, 19);
							printf("  ");
							gotoxy(23, 20);
							printf("  ");
							gotoxy(23, 21);
							printf("  ");
							gotoxy(23, small_y);
							printf("▶");
							if (ch == 13) {
								if (small_y == 19) {
									*speed = 90;
								}
								else if (small_y == 20) {
									*speed = 60;
								}
								else {
									*speed = 30;
								}
								system("cls");
								for (int i = 0; i < 11; i++) {
									for (int j = 0; j < 24; j++) {
										if (map[i][j] == 1) {
											gotoxy(j * 2 + 6, i + 2);
											printf("■");
										}
									}
								}
								gotoxy(26, 16);
								printf("start");
								gotoxy(26, 17);
								printf("level");
								gotoxy(23, 17);
								printf("▶");
								break;
							}
						}
					}
				}
			}
		}
	}
}

void setcursortype(CURSOR_TYPE c) {
	CONSOLE_CURSOR_INFO CurInfo;
	switch (c) {
	case NOCURSOR:
		CurInfo.dwSize = 1;
		CurInfo.bVisible = FALSE;
		break;
	case SOLIDCURSOR: CurInfo.dwSize = 100;
		CurInfo.bVisible = TRUE;
		break;
	case NORMALCURSOR: CurInfo.dwSize = 20;
		CurInfo.bVisible = TRUE;
		break;
	}
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &CurInfo);
}

void gotoxy(int x, int y) {
	COORD CursorPosition = { x, y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), CursorPosition);
}