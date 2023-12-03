#include "mytest.h"

#define IO_CHUNK_SIZE 1024
#define CHUNKS_NUM 256

/*
전체 정수의 개수: 1024*1024
한 IO NODE에 저장할 정수의 개수: 256*1024
한 청크에 들어갈 정수의 개수: 1024
한 IO NODE의 청크의 총 개수: 256
*/

int main()
{
    pid_t pid;
    int fd, pipe_fd, cn_fd;
    char *fifo[4] = {"/fifo1", "/fifo2", "/fifo3", "/fifo4"};
    char *computerNode[4] = {"P1.dat", "P2.dat", "P3.dat", "P4.dat"};
    char *ionode[4] = {"ionode1", "ionode2", "ionode3", "ionode4"};
    int io_chunk[4][CHUNKS_NUM][IO_CHUNK_SIZE]; /* i번째 io node의 j번째 io chunk의 k번째 정수 */
    int buf[1][CHUNKS_NUM][IO_CHUNK_SIZE];

    /* FIFO 만들기 */
    for(int i=0; i<4; i++)
    {
        if (mkfifo(fifo[i], 0666) == -1) {
            perror("mkfifo");
            exit(1);
        }
    }

    for(int i=0; i<4; i++)
    {
        pid = fork();

        if(pid < 0) /* Error */
        {
            perror("fork");
            exit(1);
        }
        else if(pid == 0) /* Child: Computer Node를 읽어 IO chunk를 만들어나감 */
        {
            for(int j=0; j<CHUNKS_NUM; j++)
            {
                for(int k=0, m=0; k<IO_CHUNK_SIZE; k++, m++)
                {
                    if (m >= 4) m = 0; /* computerNode[3]까지 돌고 다시 [0]으로 가기위함 */

                    if ((cn_fd = open(computerNode[m], O_RDONLY)) == -1){
                        perror("computer node open");
                        exit(1);
                    }
                    lseek(cn_fd, ((j*4+i)*IO_CHUNK_SIZE + k)*sizeof(int), SEEK_SET);
                    read(cn_fd, &io_chunk[i][j][k], sizeof(int));
                    close(cn_fd);
                }
            }

            if ((pipe_fd = open(fifo[i], O_WRONLY)) == -1) {
                perror("pipe open");
                exit(1);
            }
            write(pipe_fd, io_chunk[i], sizeof(io_chunk[i]));

            close(pipe_fd);
            exit(0);
        }
        else /* Parent: FIFO로 받은 데이터를 IO node에 작성 */
        {
            pipe_fd = open(fifo[i], O_RDONLY);
            read(pipe_fd, buf, sizeof(buf));
            
            if((fd = open(ionode[i], O_WRONLY | O_CREAT, 0644)) == -1)
            {
                perror("io node open");
                exit(1);
            }
            write(fd, buf, sizeof(buf));

            close(fd);
            close(pipe_fd);
        }
    }

    return 0;
}
