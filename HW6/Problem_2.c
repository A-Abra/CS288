#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <math.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>

int main(int argc, char* argv[]) {
    if (argc != 4) {
        printf("4 args needed\n");
        return 0;
    }
    int numProc = atoi(argv[1]);
    char* mFile = argv[2];
    char* outFile = argv[3];
    
    struct stat fs_ss;
    int fd = open(mFile, O_RDWR);
    if (fd == -1) {
        printf("Error opening file for writing\n");
        exit(1);
    }
    fstat(fd, &fs_ss);
    int FILESIZE = fs_ss.st_size;
    int length = (FILESIZE / sizeof(float));
    float* map = mmap(0, FILESIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    if (numProc > length) {
        printf("Process inc cannot be greater than matrix size\n");
        return 1;
    }
    if (map == MAP_FAILED)
        printf("mmap error\n");

    int msize = (int) sqrt(length);
    float matrix[msize][msize];
    float outMatrix[msize][msize];
    int mInd = 0;
    for (int i = 0; i < msize; i++) {
        for (int j = 0; j < msize; j++) {
            matrix[j][i] = map[mInd];
            mInd++;
        }
    }
    for (int i = 0; i < msize; i++) {
        for (int j = 0; j < msize; j++) {
            if(i == j)
		    outMatrix[i][j] = 1;
	    else
		    outMatrix[i][j] = 0;
	}
    }

    int value;
    int FILESIZE2 = length * sizeof(float);
    float* outMap;
    fd = open(outFile, O_RDWR | O_CREAT | O_TRUNC, (mode_t)0600);
    if (fd == -1) {
        printf("Error opening file for writing\n");
        exit(1);
    }
    value = lseek(fd, FILESIZE2 - 1, SEEK_SET);
    if (value == -1) {
        close(fd);
        printf("Error calling lseek() to 'stretch' the file\n");
        exit(1);
    }
    value = write(fd, "", 1);
    if (value == -1) {
        close(fd);
        printf("Error writing last byte of the file\n");
        exit(1);
    }
    outMap = mmap(0, FILESIZE2, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (outMap == MAP_FAILED) {
        close(fd);
        printf("Error mapping the file\n");
        exit(1);
    }

    pid_t pid, childProcs[numProc];
    int stat;
    int work = length / numProc;
    int workLeft = work;
    int size = sqrt(length);
    if (length % numProc != 0)
        workLeft = work + length - (numProc * work);
    for (int proc = 0; proc < numProc; proc++) {
        if ((childProcs[proc] = fork()) < 0) {
            printf("Error forking child process\n");
            exit(1);
        }
        else if (childProcs[proc] == 0) {
	    int wAmnt;
	    if (proc != numProc-1)
		    wAmnt = work;
	    else
		    wAmnt = workLeft;
            int workRows[wAmnt];
            int workCols[wAmnt];
            int matrixRow = 0;
            int matrixCol = 0;
            int totWAmnt = work * proc;
            for (int i = 0; i < totWAmnt; i++) {
                if (++matrixRow >= size) {
                    matrixRow = 0;
                    matrixCol++;
                }
            }
            for (int i = 0; i < wAmnt; i++) {
                workRows[i] = matrixRow;
                workCols[i] = matrixCol;

                matrixRow++;
                if (matrixRow >= size) {
                    matrixRow = 0;
                    matrixCol++;
                }
            }
            int writeTo = proc * work;
            float mNum = 0;
            int workNum = 0;
            for (int x = 0; x < size; x++) {
                for (int y = 0; y < size; y++) {
                    for (int z = 0; z < size; z++) {
                        mNum += matrix[y][z] * outMatrix[z][x];
                    }
                    if ((y == workRows[workNum]) && (x == workCols[workNum])) {
                        printf("Process %d (with pid:%d) writing to [%d][%d] with fpoint: %f\n", proc, getpid(), y, x, mNum);
                        outMap[writeTo++] = mNum;
                        workNum++;
                    }
                    mNum = 0;
                }
            }
            exit(0);
        }
    }
    while (numProc > 0) {
        pid = wait(&stat);
        numProc--;
    }
    if (pid != 0) {
        if (munmap(outMap, FILESIZE2) == -1) {
            printf("Error un-mapping file\n");
            exit(1);
        }
        close(fd);
    }
}
