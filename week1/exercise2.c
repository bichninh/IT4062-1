#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define MAX 100

struct list1{//subject
	char ID[MAX];
	char name[MAX];
	int midFactor;
	char semester[MAX];
	int studentCount;
	struct list1 *next;
};
struct list2{ //student
	char ID[MAX];
	char firstName[MAX];
	char lastName[MAX];
	float mid;
	float last;
	float final;
	char ranking;
	struct list2*next;
};
typedef struct list1 Subject;
typedef struct list2 Student;

void getString(char*str){//get a string
	fgets(str,100,stdin);
	str[strlen(str) - 1] = '\0';
}

Subject* createSub(char ID[], char name[], int midFactor, char semester[], int studentCount){//create a sub element
	Subject*new = (Subject*) malloc(sizeof(Subject));
	strcpy(new->ID,ID);
	strcpy(new->name,name);
	new->midFactor = midFactor;
	strcpy(new->semester,semester);
	new->studentCount = studentCount;
	new->next = NULL;
	return new;
}

void addSub(Subject** list,Subject*new){//add a sub element to list
	Subject*cur = *list;
	if(*list != NULL){
		while(cur->next != NULL){
			cur = cur->next;
		}
		cur->next = new;
	}
	else *list = new;
}

void writeToFileSubject(Subject*subject){//save information of subject to file
	char file_name[MAX];
	FILE*fptr;
	if(subject != NULL){
		strcpy(file_name,subject->ID);
		strcat(file_name,"_");
		strcat(file_name,subject->semester);
		strcat(file_name,".txt");
		if((fptr = fopen(file_name, "w+")) == NULL){
			printf("Can not open file!\n");
			return;
		}
		fprintf(fptr,"SubjectID | %s\n",subject->ID );
		fprintf(fptr,"Subject | %s\n",subject->name );
		fprintf(fptr,"Semester : %s\n",subject->semester);
		fprintf(fptr,"F|%d|%d\n", subject->midFactor, (100-subject->midFactor));
		fprintf(fptr,"StudentCount| %d\n",subject->studentCount);
		fclose(fptr);
	}
}

void addNewScoreBoard(Subject**list){//choose 1 - add a new score board
	char ID[MAX],name[MAX],semester[MAX];
	int midFactor,studentCount;
	printf("Enter a new subject information:\n");
	while(getchar() != '\n');
	printf("Enter subject id:\n");
	getString(ID);
	printf("Enter subject name:\n");
	getString(name);
	printf("Enter semester:\n");
	getString(semester);
	do{
		printf("Enter mid-term factor (0 =<x =<100) :\n");
		scanf("%d",&midFactor);
	}while(midFactor <= 0|| midFactor >= 100);
	printf("Enter student count:\n");
	scanf("%d",&studentCount);
	Subject* subject = createSub(ID,name,midFactor,semester,studentCount);
	addSub(list,subject);
	writeToFileSubject(subject);
}
//==============MAIN==============
int main(int argc, char const *argv[]){
	Subject* list = NULL;
	int choice;
	char c;
	do{
		printf("Learning Management System\n");
		printf("-------------------------------------\n");
		printf("\t1.\tAdd a new score board\n");
		printf("\t2.\tAdd score\n");
		printf("\t3.\tRemove score\n");
		printf("\t4.\tSearch score\n");
		printf("\t5.\tDisplay score board and score report\n");
		printf("Your choice(1-6 to, other to quit):\n");
		scanf("%d",&choice);
		switch(choice){
			case 1:
				do{
					addNewScoreBoard(&list);
					while(getchar() != '\n');
					printf("Press 'y' or 'Y' for continue, other for no:");
					c = getchar();
				}while(c == 'y' || c == 'Y');
				break;
			case 2:
				break;
			case 3:
				break;
			case 4:
				break;
			case 5:
				break;
			default:
				break;
		}
	}while(choice > 0 && choice < 6);
	return 0;
}

