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

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("3 args needed\n");
        return 0;
    }

    double pi = 0.0;
    int numProc = atoi(argv[1]);
    int numTerm = atoi(argv[2]);
    long double sum[numProc];
    pid_t pid, childProcs[numProc];
    int stat;
    int pfds[2];
    char buff[30];
    pipe(pfds);

    for (int i = 0; i < numProc; i++) {
        if ((childProcs[i] = fork()) < 0) {
            printf("Error forking child\n");
            exit(1);
        }
        else if (childProcs[i] == 0) {
            double x, sum;
            double y = 1.0 / (double) numTerm;
            int j = (numTerm / numProc) * i + 1;
            int k = (numTerm / numProc) * (i + 1);
            for (j; j <= k; j++) {
                x = y * ((double) j - 0.5);
                sum += 4.0 / (1.0 + x * x);
            }
            sprintf(buff, "%f", sum);
            buff[strlen(buff)] = 0;
            write(pfds[1], buff, strlen(buff) + 1);
            printf("Process %d and values for approx: %5d%5d\t%f\n", i, j, k, sum);
            exit(0);
        }
        else {
            read(pfds[0], buff, sizeof(buff));
            sum[i] = atof(buff);
        }
    }

    double temp = numProc;
    while (temp > 0) {
        pid = wait(&stat);
        temp--;
    }

    for (int i = 0; i < numProc; i++) {
        pi += (1.0 / (double) numTerm) * sum[i];
    }

    printf("Approximation of PI:%f\n", pi);
}
