#include <stdio.h>
#include <string.h>
#define MAX 100

// get a HAZCHEM code and print the information about it

void getString(char*str){//get a string from keyboard
	fgets(str,100,stdin);
	str[strlen(str)- 1] = '\0';
}

int checkCode(char*str){ //check and return 0 if the code is valid, else return 1
	if( strlen(str) <= 3) return 0;
	else if(str[0] > '1'  && str[0] < '4') return 0;
		 else if ( str[1] == 'P' || str[1] == 'R' || str[1] == 'S' || str[1] == 'T' 
			|| str[1] == 'W' || str[1] == 'X' || str[1] == 'Y' || str[1] == 'Z')
			  return 0;
			  else if ( str[2] == '\0'  || str[2] == 'E'  ) return 0;
			  	   else return 1;	
}

void display(char*str){// display the output
	char temp[MAX];//yes,no
	char str1[MAX];//material
	char str2[MAX];//reactivity
	char str3[MAX];//protection
	char str4[MAX];//containment
	char str5[MAX];//evacuation
	char v[MAX];
	strcpy(v,"can be violently reactive");
	char full[MAX];
	strcpy(full,"full protective clothing must be worn");
	char BA[MAX];
	strcpy(BA,"breathing apparatus");
	char BA1[MAX];
	strcpy(BA1,"breathing apparatus, protective gloves for fire only");
	char dilute[MAX];
	strcpy(dilute,"the dangerous goods may be washed down to a drain with a large quantity of water");
	char contain[MAX];
	strcpy(contain,"a need to avoid spillages from entering drains or water courses");
	if(checkCode(str) == 1){
		printf("Error code!\n");
		return;
	}
	else{
		if( str[1] == 'S' || str[1] == 'T'|| str[1] == 'Y' || str[1] == 'Z' ){
			printf("Is the %c reserve colored ? \n", str[1]);
			getString(temp);
		}
	switch(str[0]){
		case '1':
			strcpy(str1,"jets");
			break;
		case '2':
			strcpy(str1,"fog");
			break;
		case '3':
			strcpy(str1,"foam");
			break;
		case '4':
			strcpy(str1,"dry agent");
			break;
	}		
	switch(str[1]){
		case 'P':
			strcpy(str2,v);
			strcpy(str3,full);
			strcpy(str4,dilute);
			break;
		case 'R':
			strcpy(str3,full);
			strcpy(str4,dilute);
			break;
		case 'S':
			strcpy(str2,v);
			if(strcmp(temp,"yes") == 0) strcpy(str3,BA1);
			else strcpy(str3,BA);
			strcpy(str4,dilute);
			break;
		case 'T':
			if(strcmp(temp,"yes") == 0) strcpy(str3,BA1);
			else strcpy(str3,BA);
			strcpy(str4,dilute);
			break;
		case 'W':
			strcpy(str2,v);
			strcpy(str3,full);
			strcpy(str4,contain);
			break;
		case 'Y':
			strcpy(str2,v);
			if(strcmp(temp,"yes") == 0) strcpy(str3,BA1);
			else strcpy(str3,BA);
			strcpy(str4,contain);
			break;
		case 'Z':
			strcpy(str2,v);
			if(strcmp(temp,"yes") == 0) strcpy(str3,BA1);
			else strcpy(str3,BA);
			strcpy(str4,contain);
			break;
	}
	if(str[2] == 'E') strcpy(str5,"consider evacuation");
	}
	printf("***Emergency action advice***\n");
	printf("Material: %s\n",str1);
	printf("Reactivity: %s\n",str2);
	printf("Protection: %s\n",str3);
	printf("Containment: %s\n",str4);
	printf("Evacuation: %s\n",str5);
	printf("*****************************\n");

}
//===============MAIN===============
int main(int argc, char const *argv[]){
	char str[MAX];
	printf("Enter HAZCHEM code: ");
	getString(str);
	display(str);
	return 0;
}