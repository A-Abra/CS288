#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <pthread.h>

struct WorkerThreads {
    int threadNum;
    int beg;
    int size;
};

float sum = 0;
float* map;

void* findSum(void* worker) {
    struct WorkerThreads thisWorker = *(struct WorkerThreads *) worker;
    for (int i = thisWorker.beg; i < thisWorker.beg+thisWorker.size; i++) {
        float num = map[i];
        printf("Thread %d Adding value to sum: %f\n", thisWorker.threadNum,num);
        sum += num;
    }
    pthread_exit(NULL);
}

void createThread(struct WorkerThreads* worker, int numThreads, int length){
    int work = length/numThreads;
    int workLeft = work;

    if (length % numThreads != 0)
        workLeft = work + length - (numThreads * work);
    for (int i = 0; i < numThreads; i++) {
	worker[i].threadNum = i;
	if(i != numThreads-1)
		worker[i].size = work;
	else
		worker[i].size = workLeft;
        worker[i].beg = i * work;
    }
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("3 args needed\n");
        return 0;
    }

    int numThreads = atoi(argv[1]);
    char* file = argv[2];
    struct stat fs_ss;
    int fd = open(file, O_RDWR);
    fstat(fd, &fs_ss);
    int FILESIZE = fs_ss.st_size;
    int length = (FILESIZE / sizeof(float));

    if (fd == -1){
	printf("Error opening file for writing");
	exit(EXIT_FAILURE);
    }

    map = mmap(0, FILESIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    struct WorkerThreads* workers = (struct WorkerThreads*) malloc(sizeof(struct WorkerThreads) * numThreads);
    createThread(workers, numThreads, length);

    for (int i = 0; i < numThreads; i++){
        struct WorkerThreads thisWorker = workers[i];
    }

    printf("%d threads created\n", numThreads);
    pthread_t threads[numThreads];
    int rc;
    void *retval;
    for (int i = 0; i < numThreads; i++) {
        rc = pthread_create(threads + i, NULL, findSum, &workers[i]);

        if (rc != 0) {
            printf("error from pthread_create");
            return 1;
        }
    }
    for (int i = 0; i < numThreads; i++)
        pthread_join(threads[i], &retval);

    printf("Sum: %f\n", sum);
}
