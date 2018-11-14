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

    fd_tfs=open("/home/leonid/Документы/TFS", O_CREAT | O_WRONLY | O_TRUNC, 0666);
    if (fd_tfs==-1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    fd_dev_zero=open("/dev/zero", O_RDONLY);
    if (fd_dev_zero==-1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    /**External circle**/
    for (int blocksize=512; blocksize<=TFS/2; blocksize*=2) {

        int counter=TFS/blocksize;

        sync();

        off_t lseek_err=lseek(fd_tfs, 0, SEEK_SET);
        if (lseek_err==-1) {
            perror("lseek");
            exit(EXIT_FAILURE);
        }

        char *buf=malloc(blocksize);

        /**TIME - 1**/
        struct timespec tp;

        int clock_err=clock_gettime(CLOCK_MONOTONIC, &tp);
        if (clock_err==-1) {
            perror("clock_gettime");
            exit(EXIT_FAILURE);
        }

        double tv_past=tp.tv_sec+(double)tp.tv_nsec/10000000000;

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

        free(buf);

        /**TIME - 2**/
        clock_err=clock_gettime(CLOCK_MONOTONIC, &tp);
        if (clock_err==-1) {
            perror("clock_gettime");
            exit(EXIT_FAILURE);
        }

        printf("%d : %f\n", blocksize, (double)blocksize/(tp.tv_sec+(double)tp.tv_nsec/10000000000-tv_past)/1024/1024);   /**Megabyte/sec**/
    }
}
