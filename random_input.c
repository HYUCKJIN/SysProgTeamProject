//단어 뽑기 함수
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

typedef struct word* wordptr;
typedef struct word{
	int row;
	int column;
	char str[30];
}word;

void create();
wordptr random_input();

wordptr Word_Arr;

int main(){
	create();
	wordptr temp;
	srand(time(NULL)); //main쪽에서 한번만 실행해야함
	for(int i=0; i<10; i++)
	{
		temp = random_input();
		printf("%s %d %d\n", &temp->str, temp->row, temp->column);
	}
	return 0;
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
		random = rand()%30; //떨어지는 위치
		temp_Arr->column = random;
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
	//printf("%s %d %d\n", &temp_Arr->str, temp_Arr->row, temp_Arr->column);
	return temp_Arr;
}
