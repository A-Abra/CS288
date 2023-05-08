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

struct File {
    char* fPath;
    int fSize;
    int directory;
};
struct List {
    struct File file;
    struct List* next;
    struct List* prev;
};

struct List* findLast(struct List** head);
struct List* pop(struct List** head);
struct List* peek(struct List** head);
struct File checkStat(char* path);
int sizeOfList(struct List** head);
struct List* openq = NULL;
struct List* closed = NULL;

void push(struct List** head, struct File file) {
    struct List* curr = (struct List*) malloc(sizeof(struct List));
    curr->file = file;
    if ((*head) == NULL)
        *head = curr;
    else {
        struct List* last = findLast(head);
        last->next = curr;
        curr->prev = last;
    }
}

struct List* pop(struct List** head) {
    struct List* curr;
    if ((*head) == NULL)
        return NULL;
    if ((*head)->next == NULL) {
        curr = (*head);
        (*head) = NULL;
        return curr;
    }
    curr = (*head);
    (*head) = (*head)->next;
    (*head)->prev = NULL;    
    return curr;
}

struct List* peek(struct List** head) {
    if ((*head) == NULL)
        return NULL;
    return (*head);
}

struct List* findLast(struct List** head) {
    struct List* curr = (*head);
    if (curr == NULL)
        return NULL;
    while (curr->next != NULL)
        curr = curr->next;
    return curr;
}

int sizeOfList(struct List** head) {
    struct List* curr = (*head);
    int length = 0;
    while (curr != NULL) {
        length++;
        curr = curr->next;
    }
    return length;
}

void monitorFile(char* path) {
    DIR* dir;
    struct dirent* dp;
    char* fName;
    struct File root = checkStat(path);
    push(&openq, root);

    while (sizeOfList(&openq) != 0) {
        dir = opendir(peek(&openq)->file.fPath);
        while ((dp = readdir(dir)) != NULL) {
            if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0) {
                fName = malloc(strlen(peek(&openq)->file.fPath) + strlen(dp->d_name) + 2);
                sprintf(fName, "%s/%s", peek(&openq)->file.fPath, dp->d_name);
                struct File curr = checkStat(fName);
                if (curr.directory == 0) 
                    push(&openq, curr);
                else if (curr.directory == 1)
                    push(&closed, curr);
            }
        }
        closedir(dir);
        pop(&openq);
    }
}

struct File checkStat(char* fName) {
    struct File file;
    struct stat statBuf;
    mode_t mode;
    int result = lstat(fName, &statBuf);
    if (result == -1) {
        fprintf(stderr,"Cannot stat %s \n", fName);
        struct File error;
        return error;
    }
    mode = statBuf.st_mode;
    file.fPath = fName;
    file.fSize = statBuf.st_size;
    if (S_ISDIR(mode))
        file.directory = 0;
    else if (S_ISREG(mode))
        file.directory = 1;
    else
        file.directory = -1;
    return file;
}

void sortSize(struct List** head) {
    int size = sizeOfList(head);
    if (size <= 1)
        return;
    struct List* first = (*head);
    for (int i=0; i<size; i++) {
        struct List* second = first->next;
        for (int j=i+1; j<size; j++) {
            if (first->file.fSize > second->file.fSize) {
                struct File temp = first->file;
                first->file = second->file;
                second->file = temp;
            }
            second = second->next;
        }
        first = first->next;
    }
}

void print(struct List** head){
	struct List* curr = (*head);
    	while (curr != NULL) {
	       	printf("%d\t%s\n", curr->file.fSize, curr->file.fPath);
		curr = curr->next;
	}
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Only 2 args\n");
        return 0;
    }
    monitorFile(argv[1]);
    sortSize(&closed);
    print(&closed);
}

