#include <stdio.h>
#include <conio.h>
#include <Windows.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

// 색상 정의
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

#define STAR1 'O' // player1 표시
#define BLANK ' ' // ' ' 로하면 흔적이 지워진다 

#define ESC 0x1b //  ESC 누르면 종료

#define SPECIAL1 0xe0 // 특수키는 0xe0 + key 값으로 구성된다.
#define SPECIAL2 0x00 // keypad 경우 0x00 + key 로 구성된다.

#define UP  0x48 // Up key는 0xe0 + 0x48 두개의 값이 들어온다.
#define DOWN 0x50
#define LEFT 0x4b
#define RIGHT 0x4d

#define WIDTH 80
#define HEIGHT 24

int Delay = 100; // 100 msec delay, 이 값을 줄이면 속도가 빨라진다.
int keep_moving = 1; // 1:계속이동, 0:한칸씩이동.
int time_out = 30; // 제한시간
int score[2] = { 0 };
int golds[WIDTH][HEIGHT] = { 0 }; // 1이면 Gold 있다는 뜻
int goldinterval = 3; // GOLD 표시 간격
int called[2];

void removeCursor(void) { // 커서를 안보이게 한다

	CONSOLE_CURSOR_INFO curInfo;
	GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
	curInfo.bVisible = 0;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
}

void gotoxy(int x, int y) //내가 원하는 위치로 커서 이동
{
	COORD pos = { x, y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);// WIN32API 함수입니다. 이건 알필요 없어요
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
// 화면 지우기고 원하는 배경색으로 설정한다.
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

	if (called[0] == 0) { // 처음 또는 Restart
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
		if (oldy > 1) // 0 은 Status Line
			newy = oldy - 1;
		else { // 벽에 부딛치면 방향을 반대로 이동
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
		erasestar(oldx, oldy); // 마지막 위치의 * 를 지우고
		putstar(newx, newy, STAR1); // 새로운 위치에서 * 를 표시한다.
		oldx = newx; // 마지막 위치를 기억한다.
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
	y = rand() % (HEIGHT - 1) + 1;  // 제일 상단은 피한다
	textcolor(YELLOW2, BLACK);
	gotoxy(x, y);
	printf(GOLD);
	golds[x][y] = 1;
	textcolor(BLACK, WHITE);
}

void show_time(int remain_time)
{
	gotoxy(30, 0);
	printf("남은시간 : %d", remain_time);
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
	// 특수키 0xe0 을 입력받으려면 unsigned char 로 선언해야 함
	unsigned char ch;
	int run_time, start_time, remain_time, last_remain_time;
	int gold_time = 0;

	srand(time(NULL));
	start_time = time(NULL);
	last_remain_time = remain_time = time_out;	
	init();
	getchar();
	while (1) {
		// 시간 check
		run_time = time(NULL) - start_time;
		remain_time = time_out - run_time;
		if (remain_time < last_remain_time) {
			// 남은시간을 표시한다.
			show_time(remain_time); // 시간이 변할때만 출력
			last_remain_time = remain_time;
		}
		if (remain_time == 0) // 시간 종료
			break;

		if (kbhit() == 1) {  // 키보드가 눌려져 있으면
			ch = getch(); // key 값을 읽는다
			//
			// ESC 누르면 프로그램 종료
			if (ch == ESC)
				break;
			//
			if (ch == SPECIAL1 || ch == SPECIAL2) { // 만약 특수키
				// 예를 들어 UP key의 경우 0xe0 0x48 두개의 문자가 들어온다.
				ch = getch();
				// Player1은 방향키로 움직인다.
				switch (ch) {
				case UP:
				case DOWN:
				case LEFT:
				case RIGHT:
					player1(ch);
					break;
				default:// 특수키 이지만 방향키가 아닌 경우
					player1(0);
				}
			}
			else 
			{
				player1(0);
			}
		}
		else {
			// keyboard 가 눌려지지 않으면 계속 움직인다.
			// 이동중이던 방향으로 계속 이동
			player1(0);
		}
		Sleep(Delay); // Delay를 줄이면 속도가 빨라진다
	}
}