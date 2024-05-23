#include <stdio.h>
#include <conio.h>
#include <Windows.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

// ���� ����
#define BLACK	0
#define BLUE1	1
#define GREEN1	2
#define CYAN1	3
#define RED1	4
#define MAGENTA1 5
#define YELLOW1	6
#define GRAY1	7
#define GRAY2	8
#define BLUE2	9
#define GREEN2	10
#define CYAN2	11
#define RED2	12
#define MAGENTA2 13
#define YELLOW2	14
#define WHITE	15

#define STAR1 'O' // player1 ǥ��
#define BLANK ' ' // ' ' ���ϸ� ������ �������� 

#define ESC 0x1b //  ESC ������ ����

#define SPECIAL1 0xe0 // Ư��Ű�� 0xe0 + key ������ �����ȴ�.
#define SPECIAL2 0x00 // keypad ��� 0x00 + key �� �����ȴ�.

#define UP  0x48 // Up key�� 0xe0 + 0x48 �ΰ��� ���� ���´�.
#define DOWN 0x50
#define LEFT 0x4b
#define RIGHT 0x4d

#define WIDTH 80
#define HEIGHT 24

int Delay = 100; // 100 msec delay, �� ���� ���̸� �ӵ��� ��������.
int keep_moving = 1; // 1:����̵�, 0:��ĭ���̵�.
int time_out = 30; // ���ѽð�
int score[2] = { 0 };
int golds[WIDTH][HEIGHT] = { 0 }; // 1�̸� Gold �ִٴ� ��
int goldinterval = 3; // GOLD ǥ�� ����
int called[2];

void removeCursor(void) { // Ŀ���� �Ⱥ��̰� �Ѵ�

	CONSOLE_CURSOR_INFO curInfo;
	GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
	curInfo.bVisible = 0;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
}

void gotoxy(int x, int y) //���� ���ϴ� ��ġ�� Ŀ�� �̵�
{
	COORD pos = { x, y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);// WIN32API �Լ��Դϴ�. �̰� ���ʿ� �����
}

void putstar(int x, int y, char ch)
{
	gotoxy(x, y);
	putchar(ch);
}
void erasestar(int x, int y)
{
	gotoxy(x, y);
	putchar(BLANK);
}

void textcolor(int fg_color, int bg_color)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), fg_color | bg_color << 4);
}
// ȭ�� ������ ���ϴ� �������� �����Ѵ�.
void cls(int bg_color, int text_color)
{
	char cmd[100];
	system("cls");
	sprintf(cmd, "COLOR %x%x", bg_color, text_color);
	system(cmd);
}

void showscore(int player)
{
	textcolor(GREEN2, GRAY2);
	switch (player) {
	case 0: // player 1
		gotoxy(60, 0);
		printf("Player1 : %d", score[player]);
		break;
	case 1: // player 2
		gotoxy(10, 0);
		printf("Player2 : %d", score[player]);
		break;
	}
	textcolor(BLACK, WHITE);
}

void player1(unsigned char ch)
{
	static int oldx = 60, oldy = 10, newx = 60, newy = 10;
	int move_flag = 0;
	static unsigned char last_ch = 0;

	if (called[0] == 0) { // ó�� �Ǵ� Restart
		oldx = 60, oldy = 10, newx = 60, newy = 10;
		putstar(oldx, oldy, STAR1);
		called[0] = 1;
		last_ch = 0;
		ch = 0;
	}
	if (keep_moving && ch == 0)
		ch = last_ch;
	last_ch = ch;

	switch (ch) {
	case UP:
		if (oldy > 1) // 0 �� Status Line
			newy = oldy - 1;
		else { // ���� �ε�ġ�� ������ �ݴ�� �̵�
			newy = oldy + 1;
			last_ch = DOWN;
		}
		move_flag = 1;
		break;
	case DOWN:
		if (oldy < HEIGHT - 1)
			newy = oldy + 1;
		else {
			newy = oldy - 1;
			last_ch = UP;
		}
		move_flag = 1;
		break;
	case LEFT:
		if (oldx > 0)
			newx = oldx - 1;
		else {
			newx = oldx + 1;
			last_ch = RIGHT;
		}
		move_flag = 1;
		break;
	case RIGHT:
		if (oldx < WIDTH - 1)
			newx = oldx + 1;
		else {
			newx = oldx - 1;
			last_ch = LEFT;
		}
		move_flag = 1;
		break;
	}
	if (move_flag) {
		erasestar(oldx, oldy); // ������ ��ġ�� * �� �����
		putstar(newx, newy, STAR1); // ���ο� ��ġ���� * �� ǥ���Ѵ�.
		oldx = newx; // ������ ��ġ�� ����Ѵ�.
		oldy = newy;
		if (golds[newx][newy]) {
			score[0]++;
			golds[newx][newy] = 0;
			showscore(0);
		}
	}
}

#define GOLD	"*"
void show_gold()
{
	int x, y;
	x = rand() % WIDTH;
	y = rand() % (HEIGHT - 1) + 1;  // ���� ����� ���Ѵ�
	textcolor(YELLOW2, BLACK);
	gotoxy(x, y);
	printf(GOLD);
	golds[x][y] = 1;
	textcolor(BLACK, WHITE);
}

void show_time(int remain_time)
{
	gotoxy(30, 0);
	printf("�����ð� : %d", remain_time);
}

void draw_box2(int x1, int y1, int x2, int y2, char* ch)
{
	int x, y;
	int len = strlen(ch);
	for (x = x1; x <= x2; x += len) {
		gotoxy(x, y1);
		printf("%s", ch);
		gotoxy(x, y2);
		printf("%s", ch);
	}
	for (y = y1; y <= y2; y++) {
		gotoxy(x1, y);
		printf("%s", ch);
		gotoxy(x2, y);
		printf("%s", ch);
	}
}

void init()
{
	cls(BLACK, WHITE);
	FILE* ifile;
	char title[32];
	ifile = fopen("title2.txt", "r");

	system("mode can cols=130");
	while (fgets(title, 121, ifile) != NULL)
	{
		printf(title);
	}
	fclose(ifile);
	while (1)
	{
		gotoxy();
		printf("Enter Any Press");
		Sleep(500);
		putchar(BLANK);
		Sleep(500);
		if (getch())
			break;
	}
}

void main()
{
	// Ư��Ű 0xe0 �� �Է¹������� unsigned char �� �����ؾ� ��
	unsigned char ch;
	int run_time, start_time, remain_time, last_remain_time;
	int gold_time = 0;

	srand(time(NULL));
	start_time = time(NULL);
	last_remain_time = remain_time = time_out;	
	init();
	getchar();
	while (1) {
		// �ð� check
		run_time = time(NULL) - start_time;
		remain_time = time_out - run_time;
		if (remain_time < last_remain_time) {
			// �����ð��� ǥ���Ѵ�.
			show_time(remain_time); // �ð��� ���Ҷ��� ���
			last_remain_time = remain_time;
		}
		if (remain_time == 0) // �ð� ����
			break;

		if (kbhit() == 1) {  // Ű���尡 ������ ������
			ch = getch(); // key ���� �д´�
			//
			// ESC ������ ���α׷� ����
			if (ch == ESC)
				break;
			//
			if (ch == SPECIAL1 || ch == SPECIAL2) { // ���� Ư��Ű
				// ���� ��� UP key�� ��� 0xe0 0x48 �ΰ��� ���ڰ� ���´�.
				ch = getch();
				// Player1�� ����Ű�� �����δ�.
				switch (ch) {
				case UP:
				case DOWN:
				case LEFT:
				case RIGHT:
					player1(ch);
					break;
				default:// Ư��Ű ������ ����Ű�� �ƴ� ���
					player1(0);
				}
			}
			else 
			{
				player1(0);
			}
		}
		else {
			// keyboard �� �������� ������ ��� �����δ�.
			// �̵����̴� �������� ��� �̵�
			player1(0);
		}
		Sleep(Delay); // Delay�� ���̸� �ӵ��� ��������
	}
}