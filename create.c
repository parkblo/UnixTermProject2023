#include "mytest.h"

#define BUFSIZE 1024*1024

int create_source_data() {

	/* create per-process, distrinuted input data. The size of each proc is of 1MB (256K of integer).
        create one time, if possible. After creating, comment out to program the remaining functions. */

        printf("**Distribute input data across processes.\n");

        int data[BUFSIZE];
        int fd[4];
        int i, j;

        //compute node 파일 생성 및 에러 검출
        for (i = 0; i < 4; i++) {
                char filename[7];

                sprintf(filename, "P%d.dat", i + 1);
                fd[i] = open(filename, O_RDWR|O_CREAT|O_TRUNC, 0644);

                if (fd[i] == -1) {
                        perror("dat 파일 생성 오류");
                        exit(0);
                }
        }

        // 버퍼에 input data 생성(A[k] = k)
        for (i = 0; i < BUFSIZE; i++) {
                data[i] = i;
        }

        // input data 분산
        for (i = 0; i < 4; ++i) {
                pid_t pid = fork();

                if (pid == -1) {
                        perror("Fork error");
                        exit(0);
                }
                else if (pid == 0) {
                        for (j = 0; j < 256 * 1024; ++j) {
                                write(fd[i], &data[4 * j + i], sizeof(int));
                        }
                }
                else {
                        wait(NULL);
                }
        }

        // 파일 닫기 및 에러 검출
        for (i = 0; i < 4; ++i) {
                if (close(fd[i]) == -1) {
                        perror("dat 파일 닫기 오류");
                        exit(0);
                }
        }
}
