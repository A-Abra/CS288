#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/syscall.h>
#include <dirent.h>
#include <ctype.h>

struct File *filePtr;
int fInd = 0;

struct File {
	char *fPath;
	int fSize;
};

void listFilesRec(char *basePath);

void sortSize(struct File* files){
	for(int i=0; i<fInd; i++){
		for(int j=i+1;j<fInd;j++){
			if(files[i].fSize > files[j].fSize){
				struct File temp = files[i];
				files[i] = files[j];
				files[j] = temp;
			}
		}
	}
}

void monitorFile(char *fName){
	struct stat statBuf;
	mode_t mode;
	int result;

	result = lstat(fName, &statBuf);
	if(result == -1){
		fprintf(stderr,"Cannot stat %s \n", fName);
		return;
	}
	mode = statBuf.st_mode;
	if(S_ISDIR(mode))
		listFilesRec(fName);
	else if(S_ISREG(mode)){
		(filePtr+fInd)->fPath=fName;
		(filePtr+fInd)->fSize=statBuf.st_size;
		fInd++;
	}
}

void listFilesRec(char *basePath){
	char path[1000];
	struct dirent *dp;
	
	DIR *dir = opendir(basePath);
	char* fName;

	if(!dir)
		return;
	
	while ((dp = readdir(dir)) != NULL){
		if(strcmp(dp->d_name,".") != 0 && strcmp(dp->d_name,"..") != 0){
			fName = malloc(strlen(basePath)+strlen(dp->d_name) + 2);
			sprintf(fName, "%s/%s", basePath, dp->d_name);
			monitorFile(fName);
		}
	}
	closedir(dir);
}

int main(int argc, char* argv[]) {
	if (argc != 2) {
		printf("Only 2 args\n");
		return 0;
	}
	struct File fileList[10000];
	filePtr=fileList;

	char* path = argv[1];
	monitorFile(path);

	filePtr=fileList;
	sortSize(filePtr);

	for(int i=0; i<fInd; i++){
		printf("%d\t%s\n", filePtr->fSize, filePtr->fPath);
		filePtr++;
	}

	return 0;
}

