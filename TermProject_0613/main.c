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

#define GOLD "＊"
#define P "●"// player1 표시
#define MSTR "¤"
#define BLANK ' ' // ' ' 로하면 흔적이 지워진다 

#define ESC 0x1b //  ESC 누르면 종료

#define SPECIAL1 0xe0 // 특수키는 0xe0 + key 값으로 구성된다.
#define SPECIAL2 0x00 // keypad 경우 0x00 + key 로 구성된다.

#define UP  0x48 // Up key는 0xe0 + 0x48 두개의 값이 들어온다.
#define DOWN 0x50
#define LEFT 0x4b
#define RIGHT 0x4d

int keep_moving = 1; // 1:계속이동, 0:한칸씩이동.
int score = 0;
int called[2];
int map[31][28];
int mt1 = 99;
int mt2 = 99;
int mt3 = 99;
int mt4 = 99;
int p_x, p_y, m1_x, m1_y, m2_x, m2_y, m3_x, m3_y, m4_x, m4_y;

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
void textcolor(int fg_color, int bg_color)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), fg_color | bg_color << 4);
}
void cls(int bg_color, int text_color)
{
	char cmd[100];
	system("cls");
	sprintf(cmd, "COLOR %x%x", bg_color, text_color);
	system(cmd);
}
void showscore()
{
	textcolor(GREEN2, GRAY2);
	gotoxy(60, 0);
	printf("SCORE : %d", score);
	textcolor(BLACK, WHITE);
	
}
void putplayer(int x, int y)
{
	gotoxy(x, y);
	textcolor(YELLOW1, BLACK);
	printf("●");
}
void putmonster(int x, int y)
{
	gotoxy(x, y);
	printf("♣");
}
void putgold(int x, int y)
{
	gotoxy(x, y);
	textcolor(WHITE, BLACK);
	printf("＊");
}
void putblank(int x, int y)
{
	gotoxy(x, y);
	textcolor(BLACK, BLACK);
	putchar(BLANK);
}
void erasestar(int x, int y)
{
	gotoxy(x, y);
	textcolor(BLACK, BLACK);
	putchar(BLANK);
}

void player1(unsigned char ch)
{
	static int oldx = 26, oldy = 23, newx = 26, newy = 23;
	int move_flag = 0;
	static unsigned char last_ch = 0;

	if (called[0] == 0) { // 처음 또는 Restart
		oldx = 26, oldy = 23, newx = 26, newy = 23;
		p_x = 26, p_y = 23;
		putplayer(oldx, oldy);
		called[0] = 1;
		last_ch = 0;
		ch = 0;
	}
	if (keep_moving && ch == 0)
		ch = last_ch;
	last_ch = ch;
	
	switch (ch) {
		
	case UP:
		if (map[oldy - 1][oldx / 2] == 0 || map[oldy - 1][oldx / 2] == 99) // 0 은 Status Line
			newy = oldy - 1;
		else
			break;
		move_flag = 1;
		break;
	case DOWN:
		if (map[oldy + 1][oldx / 2] == 0 || map[oldy + 1][oldx / 2] == 99)
			newy = oldy + 1;
		else
			break;
		move_flag = 1;
		break;
	case LEFT:
		if (map[oldy][oldx / 2 - 1] == 0 || map[oldy][oldx / 2 - 1] == 99)
			newx = oldx - 2;
		else
			break;
		move_flag = 1;
		break;
	case RIGHT:
		if (map[oldy][oldx / 2 + 1] == 0 || map[oldy][oldx / 2 + 1] == 99)
			newx = oldx + 2;
		else
			break;
		move_flag = 1;
		break;
	}
	if (move_flag) {
		erasestar(oldx, oldy); // 마지막 위치의 * 를 지우고
		putplayer(newx, newy); // 새로운 위치에서 * 를 표시한다.
		oldx = newx; // 마지막 위치를 기억한다.
		oldy = newy;
		p_x = newx;
		p_y = newy;
		
		if (map[newy][newx / 2] != 0 && map[newy][newx / 2] != 99) { // 0이 아닌 것을 만나면 정지
			keep_moving = 0;
		}
		else if (map[newy][newx / 2] == 0) {
			score += rand()%3 + 1;
			map[newy][newx / 2] = 99;
			showscore();
		}
	}
	Sleep(50);
}

void monster1()
{
	static int oldx = 12, oldy = 5, newx = 12, newy = 5;
	int move_flag = 0;
	int dir;
	if (called[0] == 0) { // 처음 또는 Restart
		oldx = 12, oldy = 5, newx = 12, newy = 5;
		m1_x = 12, m1_y = 5;
		textcolor(RED1, BLACK);
		putmonster(oldx, oldy);
		called[0] = 1;
	}
	srand(time(NULL));

	dir = rand() % 4;
	do
	{
		if(dir == 0) //UP
		{
			if (map[oldy - 1][oldx / 2] == 0 || map[oldy - 1][oldx / 2] == 99)
				newy = oldy - 1;
			else
				break;
			move_flag = 1;
			break;
		}
		else if (dir == 1) //DOWN
		{
			if (map[oldy + 1][oldx / 2] == 0 || map[oldy + 1][oldx / 2] == 99)
				newy = oldy + 1;
			else
				break;
			move_flag = 1;
			break;
		}
		else if (dir == 2) //LEFT
		{
			if (map[oldy][oldx / 2 - 1] == 0 || map[oldy][oldx / 2 - 1] == 99)
				newx = oldx - 2;
			else
				break;
			move_flag = 1;
			break;
		}
		else if (dir == 3) //RIGHT
		{
			if (map[oldy][oldx / 2 + 1] == 0 || map[oldy][oldx / 2 + 1] == 99)
				newx = oldx + 2;
			else
				break;
			move_flag = 1;
			break;
		}
	} while (move_flag != 1);
	if (move_flag) {
		erasestar(oldx, oldy); // 마지막 위치의 * 를 지우고
		textcolor(WHITE, BLACK);
		if (map[oldy][oldx / 2] == 99)
			putgold(oldx, oldy);
		else if (map[oldy][oldx / 2] == 0)
			erasestar(oldx, oldy);
		textcolor(RED1, BLACK);
		putmonster(newx, newy); // 새로운 위치에서 * 를 표시한다.
		oldx = newx; // 마지막 위치를 기억한다.
		oldy = newy;
		m1_x = newx;
		m1_y = newy;
	}
	Sleep(200);
}
void monster2()
{
	static int oldx = 42, oldy = 5, newx = 42, newy = 5;
	int move_flag = 0;
	int dir;
	if (called[0] == 0) { // 처음 또는 Restart
		oldx = 42, oldy = 5, newx = 42, newy = 5;
		m2_x = 42, m2_y = 5;
		textcolor(BLUE1, BLACK);
		putmonster(oldx, oldy);
		called[0] = 1;
	}
	srand(time(NULL));

	dir = rand() % 4;
	do
	{
		if (dir == 0) //UP
		{
			if (map[oldy - 1][oldx / 2] == 0 || map[oldy - 1][oldx / 2] == 99)
				newy = oldy - 1;
			else
				break;
			move_flag = 1;
			break;
		}
		else if (dir == 1) //DOWN
		{
			if (map[oldy + 1][oldx / 2] == 0 || map[oldy + 1][oldx / 2] == 99)
				newy = oldy + 1;
			else
				break;
			move_flag = 1;
			break;
		}
		else if (dir == 2) //LEFT
		{
			if (map[oldy][oldx / 2 - 1] == 0 || map[oldy][oldx / 2 - 1] == 99)
				newx = oldx - 2;
			else
				break;
			move_flag = 1;
			break;
		}
		else if (dir == 3) //RIGHT
		{
			if (map[oldy][oldx / 2 + 1] == 0 || map[oldy][oldx / 2 + 1] == 99)
				newx = oldx + 2;
			else
				break;
			move_flag = 1;
			break;
		}
	} while (move_flag != 1);
	if (move_flag) {
		erasestar(oldx, oldy); // 마지막 위치의 * 를 지우고
		textcolor(WHITE, BLACK);
		if (map[oldy][oldx / 2] == 99)
			putgold(oldx, oldy);
		else if (map[oldy][oldx / 2] == 0)
			erasestar(oldx, oldy);
		textcolor(BLUE1, BLACK);
		putmonster(newx, newy); // 새로운 위치에서 * 를 표시한다.
		oldx = newx; // 마지막 위치를 기억한다.
		oldy = newy;
		m2_x = newx;
		m2_y = newy;
	}
	Sleep(200);
}
void monster3()
{
	static int oldx = 12, oldy = 26, newx = 12, newy = 26;
	int move_flag = 0;
	int dir;
	if (called[0] == 0) { // 처음 또는 Restart
		oldx = 12, oldy = 26, newx = 12, newy = 26;
		m3_x = 12, m3_y = 26;
		textcolor(MAGENTA1, BLACK);
		putmonster(oldx, oldy);
		called[0] = 1;
	}
	srand(time(NULL));

	dir = rand() % 4;
	do
	{
		if (dir == 0) //UP
		{
			if (map[oldy - 1][oldx / 2] == 0 || map[oldy - 1][oldx / 2] == 99)
				newy = oldy - 1;
			else
				break;
			move_flag = 1;
			break;
		}
		else if (dir == 1) //DOWN
		{
			if (map[oldy + 1][oldx / 2] == 0 || map[oldy + 1][oldx / 2] == 99)
				newy = oldy + 1;
			else
				break;
			move_flag = 1;
			break;
		}
		else if (dir == 2) //LEFT
		{
			if (map[oldy][oldx / 2 - 1] == 0 || map[oldy][oldx / 2 - 1] == 99)
				newx = oldx - 2;
			else
				break;
			move_flag = 1;
			break;
		}
		else if (dir == 3) //RIGHT
		{
			if (map[oldy][oldx / 2 + 1] == 0 || map[oldy][oldx / 2 + 1] == 99)
				newx = oldx + 2;
			else
				break;
			move_flag = 1;
			break;
		}
	} while (move_flag != 1);
	if (move_flag) {
		erasestar(oldx, oldy); // 마지막 위치의 * 를 지우고
		textcolor(WHITE, BLACK);
		if (map[oldy][oldx / 2] == 99)
			putgold(oldx, oldy);
		else if (map[oldy][oldx / 2] == 0)
			erasestar(oldx, oldy);
		textcolor(MAGENTA1, BLACK);
		putmonster(newx, newy); // 새로운 위치에서 * 를 표시한다.
		oldx = newx; // 마지막 위치를 기억한다.
		oldy = newy;
		m3_x = newx;
		m3_y = newy;
	}
	Sleep(200);
}
void monster4()
{
	static int oldx = 42, oldy = 26, newx = 42, newy = 26;
	int move_flag = 0;
	int dir;
	if (called[0] == 0) { // 처음 또는 Restart
		oldx = 42, oldy = 26, newx = 42, newy = 26;
		m4_x = 42, m4_y = 26;
		textcolor(YELLOW2, BLACK);
		putmonster(oldx, oldy);
		called[0] = 1;
	}
	srand(time(NULL));

	dir = rand() % 4;
	do
	{
		if (dir == 0) //UP
		{
			if (map[oldy - 1][oldx / 2] == 0 || map[oldy - 1][oldx / 2] == 99)
				newy = oldy - 1;
			else
				break;
			move_flag = 1;
			break;
		}
		else if (dir == 1) //DOWN
		{
			if (map[oldy + 1][oldx / 2] == 0 || map[oldy + 1][oldx / 2] == 99)
				newy = oldy + 1;
			else
				break;
			move_flag = 1;
			break;
		}
		else if (dir == 2) //LEFT
		{
			if (map[oldy][oldx / 2 - 1] == 0 || map[oldy][oldx / 2 - 1] == 99)
				newx = oldx - 2;
			else
				break;
			move_flag = 1;
			break;
		}
		else if (dir == 3) //RIGHT
		{
			if (map[oldy][oldx / 2 + 1] == 0 || map[oldy][oldx / 2 + 1] == 99)
				newx = oldx + 2;
			else
				break;
			move_flag = 1;
			break;
		}
	} while (move_flag != 1);
	if (move_flag) {
		erasestar(oldx, oldy); // 마지막 위치의 * 를 지우고
		textcolor(WHITE, BLACK);
		if (map[oldy][oldx / 2] == 99)
			putgold(oldx, oldy);
		else if (map[oldy][oldx / 2] == 0)
			erasestar(oldx, oldy);
		textcolor(YELLOW2, BLACK);
		putmonster(newx, newy); // 새로운 위치에서 * 를 표시한다.
		oldx = newx; // 마지막 위치를 기억한다.
		oldy = newy;
		m4_x = newx;
		m4_y = newy;
	}
	Sleep(200);
}

int die()
{
	if (p_x == m1_x && p_y == m1_y)
		return 1;
	else if (p_x == m2_x && p_y == m2_y)
		return 1;
	else if (p_x == m3_x && p_y == m3_y)
		return 1;
	else if (p_x == m4_x && p_y == m4_y)
		return 1;
	else
		return 0;
}

void init()
{
	system("mode con cols=110 lines=40");
	cls(BLACK, YELLOW1);
	removeCursor();

	gotoxy(5, 10);
	printf("■■■■　　　　　■　　　　　 　■■■■　　　　 　　■　　　　　■　　　　■　　　　■　　　■■");
	gotoxy(5, 11);
	printf("■■■■■　  　 ■■　　　　  ■■■■■■　　　　 　■■　　　■■　  　 ■■　　   ■■　　■■");
	gotoxy(5, 12);
	printf("■■　■■      ■■■　　　  ■■■■■　　　　　　  ■■■　■■■      ■■■　　　■■■　■■");
	gotoxy(5, 13);
	printf("■■■■■　   ■■■■　    ■■■■　　　 　■■■　■■■■■■■     ■■■■　　 ■■■■■■");
	gotoxy(5, 14);
	printf("■■■■　　　■■　■■　 　 ■■■■■　　　　　 　 ■■■■■■■　　■■　■■　　■■■■■■");
	gotoxy(5, 15);
	printf("■■　　　   ■■■■■■　　  ■■■■■■　　　　   ■■■■■■■   ■■■■■■   ■■■■■■");
	gotoxy(5, 16);
	printf("■■　　　　■■■■■■■　　   ■■■■　　　　     ■■■■■■■　■■■■■■■　■■■■■■");

	while (1)
	{
		gotoxy(43, 30);
		printf("Press Any Key to Start");
		Sleep(500);
		gotoxy(43, 30);
		printf("                       ");
		Sleep(500);
		if (kbhit())
			break;
	}
	cls(BLACK, WHITE);
}
//1 = ━ / 2 = ┃ / 3 = ┏ / 4 = ┓ / 5 = ┛ / 6 = ┗ / 7 = ┣ / 8 = ┳ / 9 = ┫ / 10 = ┻ / 11 = ╋ 
int startmap[31][28] =
{
3,1,1,1,1,1,1,1,1,1,1,1,1,4,3,1,1,1,1,1,1,1,1,1,1,1,1,4,
2,0,0,0,0,0,0,0,0,0,0,0,0,2,2,0,0,0,0,0,0,0,0,0,0,0,0,2,
2,0,3,1,1,4,0,3,1,1,1,4,0,2,2,0,3,1,1,1,4,0,3,1,1,4,0,2,
2,98,2,99,99,2,0,2,99,99,99,2,0,2,2,0,2,99,99,99,2,0,2,99,99,2,98,2,
2,0,6,1,1,5,0,6,1,1,1,5,0,6,5,0,6,1,1,1,5,0,6,1,1,5,0,2,
2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,
2,0,3,1,1,4,0,3,4,0,3,1,1,1,1,1,1,4,0,3,4,0,3,1,1,4,0,2,
2,0,6,1,1,5,0,2,2,0,6,1,1,4,3,1,1,5,0,2,2,0,6,1,1,5,0,2,
2,0,0,0,0,0,0,2,2,0,0,0,0,2,2,0,0,0,0,2,2,0,0,0,0,0,0,2,
6,1,1,1,1,4,0,2,6,1,1,4,0,2,2,0,3,1,1,5,2,0,3,1,1,1,1,5,
99,99,99,99,99,2,0,2,3,1,1,5,0,6,5,0,6,1,1,4,2,0,2,99,99,99,99,99,
99,99,99,99,99,2,0,2,2,99,99,99,99,99,99,99,99,99,99,2,2,0,2,99,99,99,99,99,
99,99,99,99,99,2,0,2,2,99,3,1,1,1,1,1,1,4,99,2,2,0,2,99,99,99,99,99,
99,99,99,99,99,2,0,6,5,99,2,99,99,99,99,99,99,2,99,6,5,0,2,99,99,99,99,99,
99,99,99,99,99,2,0,99,99,99,2,99,99,99,99,99,99,2,99,99,99,0,2,99,99,99,99,99,
99,99,99,99,99,2,0,3,4,99,2,99,99,99,99,99,99,2,99,3,4,0,2,99,99,99,99,99,
99,99,99,99,99,2,0,2,2,99,6,1,1,1,1,1,1,5,99,2,2,0,2,99,99,99,99,99,
99,99,99,99,99,2,0,2,2,99,99,99,99,99,99,99,99,99,99,2,2,0,2,99,99,99,99,99,
99,99,99,99,99,2,0,2,2,99,3,1,1,1,1,1,1,4,99,2,2,0,2,99,99,99,99,99,
3,1,1,1,1,5,0,6,5,99,6,1,1,4,3,1,1,5,99,6,5,0,6,1,1,1,1,4,
2,0,0,0,0,0,0,0,0,0,0,0,0,2,2,0,0,0,0,0,0,0,0,0,0,0,0,2,
2,0,3,1,1,4,0,3,1,1,1,4,0,2,2,0,3,1,1,1,4,0,3,1,1,4,0,2,
2,0,6,1,4,2,0,6,1,1,1,5,0,6,5,0,6,1,1,1,5,0,2,3,1,5,0,2,
2,98,0,0,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,0,0,98,2,
6,1,4,0,2,2,0,3,4,0,3,1,1,1,1,1,1,4,0,3,4,0,2,2,0,3,1,5,
3,1,5,0,6,5,0,2,2,0,6,1,1,4,3,1,1,5,0,2,2,0,6,5,0,6,1,4,
2,0,0,0,0,0,0,2,2,0,0,0,0,2,2,0,0,0,0,2,2,0,0,0,0,0,0,2,
2,0,3,1,1,1,1,5,6,1,1,4,0,2,2,0,3,1,1,5,6,1,1,1,1,4,0,2,
2,0,6,1,1,1,1,1,1,1,1,5,0,6,5,0,6,1,1,1,1,1,1,1,1,5,0,2,
2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,
6,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,5
};
int map[31][28];
void map_draw()
{
	int x, y;
	int i = 0;
	for (y = 0; y < 31; y++) {
		gotoxy(0, y); // y축 처음 위치만 확보하고, 라인 단위로 overwirte한다.
		for (x = 0; x < 28; x++) {

			gotoxy((x * 2), y);
			switch (map[y][x]) {
			case 0:
				textcolor(WHITE, BLACK);
				printf("＊");
				break;
			case 1:
				textcolor(BLUE1, BLACK);
				printf("━");
				break;
			case 2:
				textcolor(BLUE1, BLACK);
				printf("┃");
				break;
			case 3:
				textcolor(BLUE1, BLACK);
				printf("┏");
				break;
			case 4:
				textcolor(BLUE1, BLACK);
				printf("┓");
				break;
			case 5:
				textcolor(BLUE1, BLACK);
				printf("┛");
				break;
			case 6:
				textcolor(BLUE1, BLACK);
				printf("┗");
				break;
			case 7:
				textcolor(BLUE1, BLACK);
				printf("┣");
				break;
			case 8:
				textcolor(BLUE1, BLACK);
				printf("┳");
				break;
			case 9:
				textcolor(BLUE1, BLACK);
				printf("┫");
				break;
			case 10:
				textcolor(BLUE1, BLACK);
				printf("┻");
				break;
			case 11:
				textcolor(BLUE1, BLACK);
				printf("╋");
				break;
			case 98:
				textcolor(WHITE, BLACK);
				printf("◎");
				break;
			default:
				printf(" ");
				break;
			}

		}
	}
	Sleep(100);
}
void resetgame()
{
	score = 0;
	called[0] = 0;
	cls(BLACK, WHITE);
	
}
void copyarray(int a1[][28], int a2[][28])
{
	for (int i = 0; i <= 31; i++)
	{
		for (int j = 0; j <= 28; j++)
		{
			a2[i][j] = a1[i][j];
		}
	}

}
void main()
{
	// 특수키 0xe0 을 입력받으려면 unsigned char 로 선언해야 함
	unsigned char ch;
	unsigned char ch1 = "a";
	while (1)
	{
		ch1 = 'a';
		srand(time(NULL));

		init();
		copyarray(startmap, map);
		map_draw();
		while (1) {
			if (kbhit() == 1) {  // 키보드가 눌려져 있으면
				ch = getch(); // key 값을 읽는다

				if (ch == ESC)
					break;
				if (ch == SPECIAL1 || ch == SPECIAL2) { // 만약 특수키
					// 예를 들어 UP key의 경우 0xe0 0x48 두개의 문자가 들어온다.
					ch = getch();
					switch (ch) {
					case UP:
					case DOWN:
					case LEFT:
					case RIGHT:
						player1(ch);
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
			monster1();
			monster2();
			monster3();
			monster4();
			if (die() == 1)
			{
				gotoxy(64,20);
				textcolor(WHITE, BLACK);
				printf("다시 시작하려면 R를 눌러주세요");
				ch1 = getch();
			}
			if (ch1 == 'r' || ch1 == 'R' || ch == ESC)
			{
				resetgame();
				break;
			}
		}
		if (ch == ESC)
			break;
	}
}