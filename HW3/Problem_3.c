#include<stdio.h>
#include<stdlib.h>
#include<string.h>
void sort(char * env[], const int size){
	const char * delim = "=";
	for(int i=1;i<size;i++){
		int j=i-1;
		char * temp = env[i];
		
		while(j>=0){
			char * curr=calloc(strlen(env[j+1])+1,sizeof(char));
			char * prev=calloc(strlen(env[j])+1,sizeof(char));
			strcpy(curr,env[j+1]);
			strcpy(prev,env[j]);
			int cmpr = strcmp(strtok(curr,delim),strtok(prev,delim));
			if(cmpr>=0)break;
			else{
				env[j+1]=env[j];
				env[j]=temp;
				j--;
			}
			free(curr);
			free(prev);
		}
	}
}

int main(int argc, char * argv[], char * envp[]){
	int index = 0;
	while(envp[index])
		index++;

	sort(envp,index);
	
	for(int i=0;i<index;i++)
		printf("%s\n", envp[i]);

	exit(0);
}
