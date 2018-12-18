/* 
 * project	: 산성비 게임
 * description  : fork, pthread.h, curese.h 함수를 이용해 구현하였습니다.
 */

#include <unistd.h>
#include <termios.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <curses.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <pthread.h>

typedef struct word* wordptr;
typedef struct word{
        char str[30];
        int row;
        int col;
}word;

void create();
void firstscreen();
int menu();
void play();
void movestr();
wordptr random_input();
void answ_input();

wordptr Word_Arr;
int done=0;
wordptr list[300];
int listpoint=0;


int main(){
        int choice;
	int loop=1;
	int pid;

        firstscreen();
	while(loop){
        	choice = menu();        //int 형으로 return 하니 이를 통해 이후 조작 가능
       		if( (pid=fork())==-1){
			perror("fork");
			exit(1);
		}
		else if(pid==0){
			switch(choice){
        		        case 1:
					play();
					exit(0);
					break;
        		        case 2:
//      		                ranking();
					exit(0);
        		                break;
        		        default:
					exit(0);
        		                break;
			}
        	}
		else{
			if(choice==3)
				loop=0;
			wait(NULL);
		}
			
	}
        return 0;
}

void firstscreen(){

        initscr();
        clear();
        move(LINES/2-2, COLS/2-12);
        addstr("====rainy typing game====");
	move(LINES/2-1, COLS/2-7);
        addstr("system programing");
	move(LINES/2, COLS/2-5);
        addstr("team project");
	move(LINES-3 , 0);	
        addstr("member : Kim hyeonyu, Song yeonwuk, Kim kyeonglae, Hong heokjin");
	move(LINES-1, 0);
	standout();
	addstr("press any key to continue");
	standend();
        refresh();
        getch();
        endwin();
}

/* 메뉴 선정 화면 
 * 종료 시그널을 무시하도록 handler 변경
 * echo를 끄고 사용자에게 1,2,3중 하나를 선택하도록 한다.
 * 해당 숫자가 입력될 때까지 입력 받는다.
 */
int menu(){
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	
	char select=0;
		
	initscr();
	noecho();
	clear();
		move(LINES/2-4, COLS/2-12);
		addstr("========================");
		move(LINES/2-3, COLS/2-12);
                addstr("          menu          ");
		move(LINES/2-2, COLS/2-12);
                addstr("========================");
                move(LINES/2-1, COLS/2-12);
                addstr("      1.Start game      ");
		move(LINES/2, COLS/2-12);
                addstr("      2.Ranking         ");
		move(LINES/2+1, COLS/2-12);
                addstr("      3.End             ");
	refresh();
		move(LINES-1, 0);
		standout();
		addstr("enter the number of a menu");
		standend();
	refresh(); 
	select = getch();
	
	while(1){
		if(select==49||select==50||select==51)
			break;
		move(LINES-1, 0);
		standout();
		addstr("Wrong input. Enter again  ");
		standend();
		refresh();
		select = getch();
	}
	clear();
	endwin();
	
	return select-48;
                
}
void play(){
        int delay=2000000, cnt=1;
	char name[30];
	pthread_t t1;
	srand(time(NULL));
        initscr();
	create();
        noecho();
        clear();
	
//	pthread_create(&t1, NULL, answ_input, NULL);
        while(done==0){
		movestr();
		usleep(delay-(cnt/5)*(delay/20));
		cnt++;
	}
//	pthread_join(t1, NULL);

	clear();
	mvaddstr(LINES/2-1, COLS/2-1, "Game Over");
	mvaddstr(LINES/2, COLS/2-5, "Enter your name");
	refresh();
	scanf("%s", name);
        endwin();

}

void movestr()
{
	int i;
        list[listpoint++]=random_input();
        
        for (i=0;i<listpoint;i++){
                if(list[i]->row==LINES-1)
                        done=1;
		else{
			move(list[i]->row-1, list[i]->col);
                        addstr("                      ");
                	move(list[i]->row, list[i]->col);
                	addstr(list[i]->str);
                	list[i]->row+=1;
		}
	}
	refresh();
}
/*
void answ_input(){
	mvaddstr(LINES-2


}*/
void create(){
        FILE *fp = fopen("WordData.txt","r");
        int size = 1000;
        wordptr temp_Arr;

        Word_Arr = (word*)malloc(sizeof(word)*size);

        temp_Arr = Word_Arr;
        int random;
        for(int i=0; i<size; i++)
        {
                fscanf(fp, "%s", &temp_Arr->str);
                random = rand()%(COLS-20); //떨어지는 위치
                temp_Arr->col = random;
                temp_Arr->row = 0;
                temp_Arr++;
        }
}

wordptr random_input(){
        wordptr temp_Arr;
        int random;
        int size = 1000;

        random = rand()%(size-1) + 1; //맨 앞 a는 빼고 보냄

        temp_Arr = Word_Arr;
        for(int i=0; i<random; i++)
                temp_Arr++;
        //printf("%d\n", random);
        //printf("%s %d %d\n", &temp_Arr->str, temp_Arr->row, temp_Arr->col);
        return temp_Arr;
}
