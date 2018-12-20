#pragma once

#include<stdio.h>
#include<string.h>
#include<fcntl.h>
#include<unistd.h>
#include<stdlib.h>
#include<curses.h>
#include<signal.h>

int readranking(int *, char[][100]);
void sort(int *, char[][100], int);
void printrank(int *, char[][100], int);

int main() { //menu complie -> change main name => print_ranking
	int rank[32];
	int people;
	char name[30][100];
	int i;

	people = readranking(rank, name);

	sort(rank, name, people);

	printrank(rank, name, people);

	return 0;
}

int readranking(int *rank, char name[][100]) {
	FILE* fp;
	int cnt = 0;
	int stdin_copy = dup(0);

	close(0);
	if((fp = fopen("ranking.txt", "r")) == NULL) {
		perror("ranking.txt");
		exit(1);
	}

	while(!feof(fp)) {
		fscanf(fp, "%d %s\n", rank + cnt, name[cnt]);
		cnt++;
	}

	fclose(fp);
	dup2(stdin_copy, 0);
	close(stdin_copy);

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
	refresh();

	move(LINES-1, 0);
	standout();
	addstr("Press any key.  ");
	standend();
	refresh();
	getch();

	endwin();
}
