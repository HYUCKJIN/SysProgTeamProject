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
#include <ctype.h>

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
void *answ_input();
void cmplist(char []);
int readranking(int *, char[][100]);
void sort(int *, char[][100], int);
void printrank(int *, char[][100], int);
void ranking();

pthread_mutex_t counter_lock = PTHREAD_MUTEX_INITIALIZER;
wordptr Word_Arr;
int done=0;
wordptr list[300];
int listpoint=0;
char noansw[30]="nthansw";
int score=0;
char scorebuf[3]={0};

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
	      		                ranking();
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
	FILE *fpout;
        int delay=1000000, cnt=1;
	char name[30],ch[4]={0}, ch2[4]={0};
	pthread_t t1;
	srand(time(NULL));
        initscr();
	create();
        noecho();
        clear();

	cnt=rand()%1000;
	ch[0]=cnt/100+48;
	ch[1]=(cnt%100)/10+48;
	ch[2]=cnt%10+48;
	ch[3]='\0';
	cnt=1;
	mvaddstr(LINES/2-1, COLS/2-10, "Your ID : player");
	addstr(ch);
	refresh();
	sleep(2);
	clear();
	pthread_create(&t1, NULL, answ_input, NULL);
        while(done==0){
		movestr();
		if(cnt>47)
			usleep(200000);
		else{
			usleep(delay-(cnt/6)*(delay/10));
			cnt++;
		}
	}
	pthread_mutex_lock(&counter_lock);
	clear();
	mvaddstr(LINES/2-2, COLS/2-5, "Game over");
	mvaddstr(LINES/2-1, COLS/2-6, "ID : player");
	addstr(ch);
	mvaddstr(LINES/2, COLS/2-7,   "SCORE : ");
	sprintf(ch2, "%d", score);
	addstr(ch2);
	standout();
	mvaddstr(LINES-1, 0, "Press any key");
	standend();
	getch();
	pthread_mutex_unlock(&counter_lock);
        endwin();
	
	fpout=fopen("ranking.txt", "a");
	fprintf(fpout, "%d player%s\n", score, ch);
	score=0;
}

void movestr()
{
	int i;
        list[listpoint++]=random_input();
        int yn;
        for (i=0;i<listpoint;i++){
		yn=strcmp(list[i]->str, noansw);	
                if(list[i]->row==LINES-4&&yn!=0)
                        done=1;
		else{
			pthread_mutex_lock(&counter_lock);
			if(yn!=0 || list[i]->row<LINES-4)
				mvaddstr(list[i]->row-1, list[i]->col, "                      ");
			if(yn!=0)
                		mvaddstr(list[i]->row, list[i]->col, list[i]->str);
				move(LINES-1, 0);
                	refresh();
			pthread_mutex_unlock(&counter_lock);
                	list[i]->row+=1;
		}
	}
}

void *answ_input(){
	int i, answptr=0;
	char answ[30]={0}, c;

	pthread_mutex_lock(&counter_lock);
        for(i=0; i<COLS-1; i++)
                mvaddstr(LINES-3, i, "-");
	refresh();
	pthread_mutex_unlock(&counter_lock);

	while(done==0){
		pthread_mutex_lock(&counter_lock);
		mvaddstr(LINES-2, 0, "ANSWER :");
                mvaddstr(LINES-2, COLS-16,"SCORE : ");
		sprintf(scorebuf, "%d", score);
		addstr(scorebuf);
                refresh();
		pthread_mutex_unlock(&counter_lock);
		c=getch();
		pthread_mutex_lock(&counter_lock);
		if(c=='\n'){
			answ[answptr]='\0';
			cmplist(answ);
			for(i=0;i<50;i++)
				answ[i]='\0';
			answptr=0;
			mvaddstr(LINES-2, 0, "ANSWER :                 ");
			mvaddstr(LINES-2, COLS-16,"SCORE : ");
			sprintf(scorebuf, "%d", score);
                	addstr(scorebuf);
			refresh();
		}
		else if(c==127&&answptr>0){
			answ[--answptr]='\0';
			mvaddstr(LINES-2, 0, "ANSWER : ");
			addstr(answ);
			addstr(" ");
                        mvaddstr(LINES-2, COLS-16,"SCORE : ");
			sprintf(scorebuf, "%d", score);
                	addstr(scorebuf);
			refresh();
		}
		else{
			answ[answptr++]=c;	
			mvaddstr(LINES-2, 0, "ANSWER : ");
                        addstr(answ);
                        mvaddstr(LINES-2, COLS-16,"SCORE : ");
			sprintf(scorebuf, "%d", score);
                	addstr(scorebuf);
                        refresh();
		}	
		move(LINES-1, 0);
		refresh();
		pthread_mutex_unlock(&counter_lock);
	}
}

void cmplist(char answ[]){
	int i;
	for(i=0;i<listpoint;i++){
		if(strcmp(answ,list[i]->str)==0){
			strcpy(list[i]->str, noansw);
			score++;
			break;
		}
	}
}
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
        return temp_Arr;
}

void ranking() {
        int rank[32];
        int people;
        char name[30][100];
        int i;

        people = readranking(rank, name);

        sort(rank, name, people);

        printrank(rank, name, people);
}

int readranking(int *rank, char name[][100]) {
        FILE* fp;
        int cnt = 0;

        close(0);
        if((fp = fopen("ranking.txt", "r")) == NULL) {
                perror("ranking.txt");
                exit(1);
        }

        while(!feof(fp)) {
                fscanf(fp, "%d %s\n", rank + cnt, name[cnt]);
                cnt++;
        }

        return cnt;
}

void sort(int *rank, char name[][100], int cnt) {
        int i, j, temp;
        char ntemp[100];

        for(i = 0; i < cnt - 1; i++) {
                for(j = i + 1; j < cnt; j++) {
                        if(rank[i] < rank[j]) {
                                temp = rank[i];
                                rank[i] = rank[j];
                                rank[j] = temp;
                                strcpy(ntemp, name[i]);
                                strcpy(name[i], name[j]);
                                strcpy(name[j], ntemp);
                        }
                }
        }
}

void printrank(int *rank, char name[][100], int cnt) {
        signal(SIGINT, SIG_IGN);
        signal(SIGQUIT, SIG_IGN);

        int i;
        char num[2], temp[32][144];
        char m = 0;

        initscr();
        noecho();
        clear();

        move(LINES/2-4, COLS/2-12);
        addstr("======ranking======");

        for(i = 0; i < cnt; i++) {
                sprintf(temp[i], "%d", rank[i]);
                move(LINES/2-2+i, COLS/2-12);
                sprintf(num, "%d", i + 1);
                addstr(num);
                addstr(". ");
                addstr(name[i]);
                addstr(": ");
                addstr(temp[i]);
        }
        move(LINES-1, 0);
        standout();
        addstr("This page will be shut down in 5 seconds");
        standend();
        refresh();
        sleep(5);
        endwin();
}

