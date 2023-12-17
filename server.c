#include "mytest.h"

#define QuarterSize (1024 * 256)

void makeFifo() { //fifo naming : s1c1.fifo -> server)
	int i, j;
	char fifoname[10];

	for (i=0; i<4; i++) {
		for (j=0; j<4; j++) {
			sprintf(fifoname, "c%ds%d.fifo", i+1, j+1);
			mkfifo(fifoname, 0644);
		}
	}
}

void client(int clientID) {
	//1. 4개의 서버와의 피포파일을 쓰기모드로 연다.
	char fifoname[10];
	int fifo_fd[4];
	int i;

    for (i=0; i<4; i++) {
        sprintf(fifoname, "c%ds%d.fifo", clientID+1, i+1);
        fifo_fd[i] = open(fifoname, O_WRONLY);
    }

    //2. P%d.dat 에서 클라이언트 번호에 맞는 데이터를 읽어들여 data배열에 저장한다.
	int client_fd;
	int data[QuarterSize];
	char datname[7];
	
	sprintf(datname, "P%d.dat", clientID+1);
	client_fd = open(datname, O_RDONLY);
	
	read(client_fd, &data, sizeof(int)*QuarterSize);

	//3. 데이터를 256씩 순차적으로 1, 2, 3, 4번 서버에 보낸다.
	for (i = 0; i < QuarterSize / 256; i++) {
		write(fifo_fd[i%4], &data[i * 256], sizeof(int)*256);
	}

	//4. 모든 피포를 닫는다.
	for (i = 0; i < 4; i++) {
		close(fifo_fd[i]);
	}
}

void server(int serverID){
	pid_t pidS;
	int chunk[1024];
	int ionode_fd, fifo_fd[4];
	int i, j, k;
	
	pidS = fork();
	if (pidS == 0) {
		client(serverID);
		exit(0);
	}

	//1. serveerID에 따라 ionode파일을 만들어 쓰기모드로 연다.
	char ionodename[10];

	sprintf(ionodename, "s%d.ionode", serverID+1);
	ionode_fd = open(ionodename, O_CREAT | O_WRONLY, 0644);

	//2. serverID에 따라 해당하는 피포파일 4개를 읽기모드로 연다.
	for (i=0; i<4; i++){
		char fifoname[10];
		
		sprintf(fifoname, "c%ds%d.fifo", i+1, serverID+1);
		fifo_fd[i] = open(fifoname, O_RDONLY);
	}

    for(i=0; i<256; i++){
        //3-1. 각 피포파일에서 순차적으로 Int 1개씩 256번을 담아 청크배열을 만든다.
        for (j=0; j<256; j++) {
            for(k = 0; k < 4; k++) {
                int n = read(fifo_fd[k], &chunk[j*4+k], sizeof(int));
            }
        }
        
        //3-2. 청크배열을 ionode 파일에 저장한다.
        write(ionode_fd, &chunk, sizeof(int)*1024);
        
        //3-3. 위 과정을 반복한다.
    }

	//4. 모든 피포를 닫는다.
	for (i=0; i<4; i++) {
		close(fifo_fd[i]);
	}

	//5. ionode파일을 닫는다.
	close(ionode_fd);
}

int server_oriented_io() {
	int i=0;
	pid_t pidM;

	makeFifo();
	
	for (i = 0; i < 4; i++) {
		pidM = fork();	
		if (pidM == 0) {
			//server 받아서 ionode에 저장
			server(i);
			exit(0);
		}
	}
}
