#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

#define CHILD_NUM 4
#define FIFO_NUM 16
#define CHUNK_SIZE 1024

volatile sig_atomic_t ready = 0;
volatile sig_atomic_t fifo_flag = 0;

void signal_handler(int signal_number) {
    ready++; // 자식 프로세스가 얼마나 준비를 마쳤는가?
}

void server_signal_handler(int signal_number) {
    ready++;
    fifo_flag++; // fifo에 청크가 얼마나 쌓여있는가?
}

void server_process(int i) {
    // 시그널 설정
    signal(SIGUSR1, server_signal_handler);
    int cnt=0;

    // {i+1}.ionode 오픈
    char ionode_name[10];
    sprintf(ionode_name, "%d.ionode", i+1);
    int ionode_fd = open(ionode_name, O_WRONLY | O_CREAT, 0666);
    if (ionode_fd < 0) {
        perror("open");
        exit(EXIT_FAILURE);
    }
    printf("%d.ionode 오픈완료\n",i);

    // fifo 4개 오픈 (1번 서버일 경우 0,1,2,3.fifo)
    int fifo_fd[4];
    for (int k=i*4; k<=i*4+3; k++){
        char fifo_name[10];
        sprintf(fifo_name, "%d.fifo", k);
        fifo_fd[cnt] = open(fifo_name, O_RDONLY | O_NONBLOCK);
        if (fifo_fd[cnt] < 0) {
            perror("open");
            exit(EXIT_FAILURE);
        }
        cnt++;
        printf("서버: %d.fifo 오픈완료\n",k);
    }
    
    // 부모 프로세스에게 준비가 되었음을 알림
    kill(getppid(), SIGUSR2);

    printf("클라이언트의 시그널을 대기중임\n");

    while (fifo_flag < 16) { // 청크가 쌓이기를 기다림. 왜 16인지는 ...?
        pause();
    }
    
    // fifo_fd의 NONBLOCK 해제
    int flags[4]; 
    for (int j=0; j<4; j++) {
        flags[j]= fcntl(fifo_fd[j], F_GETFL, 0);
        flags[j] &= ~O_NONBLOCK;
        fcntl(fifo_fd[j], F_SETFL, flags);
    }

    // fifo 입력 시작
    int num, stacked=0;
    while (1) { // 계속해서 fifo를 read&write한다.
        for (int j=0; j<4; j++) {
            read(fifo_fd[j], &num, sizeof(num));
            write(ionode_fd, &num, sizeof(num)); stacked++;
        }

        if (stacked >= 256*1024) break; // 256*1024개의 정수가 담겼다면 빠져 나온다.
    }

    // 종료 수순
    printf("%d번 서버 종료.\n",i+1);

    for (int j=0; j<4; j++){
        close(fifo_fd[j]);
    }
    close(ionode_fd);

    // 부모 프로세스에게 종료하였음을 알림
    kill(getppid(), SIGUSR2);

    exit(EXIT_SUCCESS);
}

void client_process(int i, int *server_pids) {
    int cnt=0;
    signal(SIGUSR2, signal_handler);

    // FIFO 4개 쓰기로 열기 (1번 클라이언트일 경우 0,4,8,12.fifo)
    int fifo_fd[4];
    for (int k=i; k<16; k+=4){
        char fifo_name[10];
        sprintf(fifo_name, "%d.fifo", k);
        fifo_fd[cnt] = open(fifo_name, O_WRONLY);
        if (fifo_fd[cnt] < 0) {
            perror("open");
            exit(EXIT_FAILURE);
        }
        cnt++;
        printf("클라이언트: %d.fifo 오픈\n",k);
    }

    // P{i+1}.dat 불러오기
    int num,fd;
    char file_name[10];
    sprintf(file_name, "P%d.dat", i+1);
    fd = open(file_name, O_RDONLY);
    if (fd < 0) {
        perror("open");
        exit(EXIT_FAILURE);
    }
    printf("클라이언트: P%d.dat 오픈\n",i+1);
    
    // 알맞은 서버에 FIFO 전달 시작
    int stacked=0, chunk_cnt=0;
    while ((read(fd, &num, sizeof(num))) > 0) {
        write(fifo_fd[stacked/256 %4], &num, sizeof(num));
        //printf("%d\n",num);
        stacked++; chunk_cnt++;
        
        //if(stacked >= 256*1024) break;

        if (chunk_cnt >= 256) {// SIGUSR1 : 당신에게 청크의 1/4를 보냈습니다.
            kill(server_pids[stacked/256%4], SIGUSR1);
            chunk_cnt=0;
        }

    }

    // 종료 수순
    printf("%d번 클라이언트 종료.\n",i+1);

    close(fd);
    for (int j=0; j<4; j++){
        close(fifo_fd[j]);
    }

    // 부모 프로세스에게 종료하였음을 알림
    kill(getppid(), SIGUSR2);

    exit(EXIT_SUCCESS);
}

int main() {
    int server_pids[CHILD_NUM];
    int client_pids[CHILD_NUM];

    signal(SIGUSR2, signal_handler);

    // FIFO 16개 생성 (0~15.fifo)
    for (int i = 0; i < FIFO_NUM; i++) {
        char fifo_name[10];
        sprintf(fifo_name, "%d.fifo", i);
        mkfifo(fifo_name, 0777);
    }

    // 자식 프로세스 4개 생성 : server의 역할
    for (int i = 0; i < CHILD_NUM; i++) {
        server_pids[i] = fork();

        if (server_pids[i] < 0) {
            perror("fork");
            exit(EXIT_FAILURE);
        } else if (server_pids[i] == 0) {
            server_process(i);
        }
    }

    printf("부모: 자식의 준비가 끝나기 대기중임\n");

    // 자식 프로세스들이 모두 준비될 때까지 대기
    while (ready < CHILD_NUM) {
        pause();
    }
    
    printf("===부모는 자식의 준비가 끝났음을 알았음, 클라이언트 생성 시작\n");

    // 자식 프로세스 4개 생성: client 역할
    for (int i = 0; i < CHILD_NUM; i++) {
        client_pids[i] = fork();

        if (client_pids[i] < 0) {
            perror("fork");
            exit(EXIT_FAILURE);
        } else if (client_pids[i] == 0) {
            client_process(i, server_pids);
        }
    }

    // 자식 프로세스들이 모두 종료될 때 까지 대기
    /*
    while (ready < 12) {
        pause();
    }
    */

    return 0;
}

