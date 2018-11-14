#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#define TFS 0x8000000

int main(int argc, char *argv[]) {
    int fd_dev_zero, fd_tfs;

    fd_tfs=open("./tfs.bin", O_CREAT | O_WRONLY | O_TRUNC, 0666);
    if (fd_tfs==-1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    fd_dev_zero=open("/dev/urandom", O_RDONLY);
    if (fd_dev_zero==-1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    /**External circle**/
    for (int blocksize=512; blocksize<=TFS/2; blocksize*=2) {

        int counter=TFS/blocksize;


        off_t lseek_err=lseek(fd_tfs, 0, SEEK_SET);
        if (lseek_err==-1) {
            perror("lseek");
            exit(EXIT_FAILURE);
        }

        char *buf=malloc(blocksize);

        /**TIME - 1**/
        struct timespec t1 = {0};

        int clock_err=clock_gettime(CLOCK_MONOTONIC, &t1);
        if (clock_err==-1) {
            perror("clock_gettime");
            exit(EXIT_FAILURE);
        }

        /**Internal circle**/
        for (int i=0; i<counter; i++) {
            int rd=read(fd_dev_zero, buf, blocksize);
            if (rd==-1) {
                perror("read");
                exit(EXIT_FAILURE);
            }

            int wr=write(fd_tfs, buf, blocksize);
            if (wr==-1) {
                perror("write");
                exit(EXIT_FAILURE);
            }
        }

        syncfs(fd_tfs);

        /**TIME - 2**/
        struct timespec t2 = {0};
        clock_err=clock_gettime(CLOCK_MONOTONIC, &t2);
        if (clock_err==-1) {
            perror("clock_gettime");
            exit(EXIT_FAILURE);
        }

        free(buf);

        double elapsedTime;

        elapsedTime = (t2.tv_sec - t1.tv_sec);
        elapsedTime += (t2.tv_nsec - t1.tv_nsec) / 1.0E9;

        printf("%d : %f : %f\n", blocksize, elapsedTime, ((double)TFS/1024/1024)/elapsedTime);   /**Megabyte/sec**/
    }
}
