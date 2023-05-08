#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>

int nthBit(int num, int n) {
    return (num >> n) & 1;
}

void revArr(float *arr, int n) {
    for (int i = 0, j = n - 1; i < j; i++, j--) {
        float temp = arr[i];
        arr[i] = arr[j];
        arr[j] = temp;
    }
}

void radixsort(float *arr, unsigned int len) {
    unsigned int indOne = 0;
    unsigned int indTwo = 0;
    float *buktOne = (float *) malloc(len * sizeof(float));
    float *buktTwo = (float *) malloc(len * sizeof(float));

    for (int i = 0; i < 32; i++) {
        for (int j = 0; j < len; j++) {
            unsigned int chngnum = * (unsigned int *) &arr[j];
            int bitPos = nthBit(chngnum, i);
            if (bitPos == 0) {
                buktOne[indOne] = arr[j];
                indOne++;
            }
            else {
                buktTwo[indTwo] = arr[j];
                indTwo++;
            }
        }
        
	if(i == 31) {
            revArr(buktTwo, indTwo);
            memcpy(arr, buktTwo, indTwo * sizeof(float));
            memcpy(arr + indTwo, buktOne, indOne * sizeof(float));
        }
        else {
            memcpy(arr, buktOne, indOne * sizeof(float));
            memcpy(arr + indOne,  buktTwo, indTwo * sizeof(float));
        }
        memset(&buktOne[0], 0, sizeof(float) * len);
        memset(&buktTwo[0], 0, sizeof(float) * len);
        indOne = 0;
    	indTwo = 0;
    }
    
    free(buktOne);
    free(buktTwo);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Only 2 args needed\n");
        return 0;
    }

    char* path = argv[1];
    float *map;
    int count;
    int fd = open(path, O_RDWR);
    struct stat fs_ss;
    fstat(fd, &fs_ss);
    int FILESIZE = fs_ss.st_size;
    count = FILESIZE / sizeof(float);

    if (fd == -1) {
        perror("Error opening file for writing"); 
        exit(EXIT_FAILURE);
    }
    map = mmap(0, FILESIZE+1, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (map==MAP_FAILED)
	    printf("mmap error\n");

    radixsort(map, count);

    close(fd);
    return 0;
}
