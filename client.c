#include "mytest.h"

int client_oriented_io() {
	int c1s1[2], c2s2[2], c3s3[2], c4s4[2];
	int p1p2[2][2], p1p3[2][2], p1p4[2][2], p2p3[2][2], p2p4[2][2], p3p4[2][2];
	int i, j, k, fd, cnt, fd2;

	int *rData = malloc(sizeof(int) * 1024 * 1024);
	int(*rcvData)[256] = malloc(sizeof(int[4][256]));
	int *ioData = malloc(sizeof(int) * 1024);
	int *sendData = malloc(sizeof(int) * 1024);

	char fileName[10];
	char* ioNode[4] = { "c1.ionode", "c2.ionode", "c3.ionode", "c4.ionode" };

	pid_t pid;

	//Create Pipe
	pipe(c1s1);
	pipe(c2s2);
	pipe(c3s3);
	pipe(c4s4);
	for (i = 0; i < 2; i++) {
		pipe(p1p2[i]);
		pipe(p1p3[i]);
		pipe(p1p4[i]);
		pipe(p2p3[i]);
		pipe(p2p4[i]);
		pipe(p3p4[i]);
	}

	struct timeval stime,etime;
    double time_result;

	//Client Process
	for (i = 0; i < 4; i++) {

		switch (pid = fork()) {
		case -1:
			perror("error on fork");

		case 0:
			// Comp.
			gettimeofday(&stime, NULL);

			sprintf(fileName, "P%d.dat", (i + 1));
			fd = open(fileName, O_RDONLY);
			read(fd, rData, sizeof(int) * 1024 * 256);

			gettimeofday(&etime, NULL);

			time_result = (etime.tv_sec - stime.tv_sec) * 1000.0;      // sec to ms
			time_result += ((etime.tv_usec - stime.tv_usec) / 1000.0); // us to ms
			printf("Client_oriented_io Comp. %d TIMES == %.2f ms\n", i, time_result);


			// Comm.
			gettimeofday(&stime, NULL);

			if (i == 0) {
				close(p1p2[0][0]);
				close(p1p2[1][1]);

				close(p1p3[0][0]);
				close(p1p3[1][1]);

				close(p1p4[0][0]);
				close(p1p4[1][1]);


				for (j = 0; j < 1024 * 256; j += 1024) {				
					memcpy(rcvData[0], rData + j, 256 * sizeof(int));
					read(p1p2[1][0], rcvData[1], 256 * sizeof(int));
					read(p1p3[1][0], rcvData[2], 256 * sizeof(int));
					read(p1p4[1][0], rcvData[3], 256 * sizeof(int));

					cnt = 0;
					for (k = 0; k < 256; k++) {
						sendData[cnt++] = rcvData[0][k];
						sendData[cnt++] = rcvData[1][k];
						sendData[cnt++] = rcvData[2][k];
						sendData[cnt++] = rcvData[3][k];
					}
					write(c1s1[1], sendData, 1024 * sizeof(int));

				}

				for (j = 0; j < 1024 * 256; j += 1024) {
					write(p1p2[0][1], rData + (j + 256), 256 * sizeof(int));
				}

				for (j = 0; j < 1024 * 256; j += 1024) {
					write(p1p3[0][1], rData + (j + 512), 256 * sizeof(int));
				}
				for (j = 0; j < 1024 * 256; j += 1024) {
					write(p1p4[0][1], rData + (j + 768), 256 * sizeof(int));
				}
			}
			else if (i == 1) {
				close(p1p2[1][0]);
				close(p1p2[0][1]);

				close(p2p3[0][0]);
				close(p2p3[1][1]);

				close(p2p4[0][0]);
				close(p2p4[1][1]);

				for (j = 0; j < 1024 * 256; j += 1024) {
					write(p1p2[1][1], rData + j, 256 * sizeof(int));
				}


				for (j = 0; j < 1024 * 256; j += 1024) {
					read(p1p2[0][0], rcvData[0], 256 * sizeof(int));
					memcpy(rcvData[1], rData + (j + 256), 256 * sizeof(int));
					read(p2p3[1][0], rcvData[2], 256 * sizeof(int));
					read(p2p4[1][0], rcvData[3], 256 * sizeof(int));

					cnt = 0;
					for (k = 0; k < 256; k++) {
						sendData[cnt++] = rcvData[0][k];
						sendData[cnt++] = rcvData[1][k];
						sendData[cnt++] = rcvData[2][k];
						sendData[cnt++] = rcvData[3][k];
					}

					write(c2s2[1], sendData, 1024 * sizeof(int));


				}
				for (j = 0; j < 1024 * 256; j += 1024) {
					write(p2p3[0][1], rData + (j + 512), 256 * sizeof(int));
				}
				for (j = 0; j < 1024 * 256; j += 1024) {
					write(p2p4[0][1], rData + (j + 768), 256 * sizeof(int));
				}
			}
			else if (i == 2) {
				close(p1p3[1][0]);
				close(p1p3[0][1]);
				close(p2p3[1][0]);
				close(p2p3[0][1]);
				close(p3p4[0][0]);
				close(p3p4[1][1]);

				for (j = 0; j < 1024 * 256; j += 1024) {
					write(p1p3[1][1], rData + j, 256 * sizeof(int));

				}
				for (j = 0; j < 1024 * 256; j += 1024) {
					write(p2p3[1][1], rData + (j + 256), 256 * sizeof(int));
				}
				for (j = 0; j < 1024 * 256; j += 1024) {
					read(p1p3[0][0], rcvData[0], 256 * sizeof(int));
					read(p2p3[0][0], rcvData[1], 256 * sizeof(int));
					memcpy(rcvData[2], rData + (j + 512), 256 * sizeof(int));
					read(p3p4[1][0], rcvData[3], 256 * sizeof(int));

					cnt = 0;
					for (k = 0; k < 256; k++) {
						sendData[cnt++] = rcvData[0][k];
						sendData[cnt++] = rcvData[1][k];
						sendData[cnt++] = rcvData[2][k];
						sendData[cnt++] = rcvData[3][k];
					}

					write(c3s3[1], sendData, 1024 * sizeof(int));

				}
				for (j = 0; j < 1024 * 256; j += 1024) {
					write(p3p4[0][1], rData + (j + 768), 256 * sizeof(int));
				}
			}
			else {
				close(p1p4[1][0]);
				close(p1p4[0][1]);
				close(p2p4[1][0]);
				close(p2p4[0][1]);
				close(p3p4[1][0]);
				close(p3p4[0][1]);

				for (j = 0; j < 1024 * 256; j += 1024) {
					write(p1p4[1][1], rData + j, 256 * sizeof(int));
				}
				for (j = 0; j < 1024 * 256; j += 1024) {
					write(p2p4[1][1], rData + (j + 256), 256 * sizeof(int));
				}
				for (j = 0; j < 1024 * 256; j += 1024) {
					write(p3p4[1][1], rData + (j + 512), 256 * sizeof(int));
				}
				for (j = 0; j < 1024 * 256; j += 1024) {
					read(p1p4[0][0], rcvData[0], 256 * sizeof(int));
					read(p2p4[0][0], rcvData[1], 256 * sizeof(int));
					read(p3p4[0][0], rcvData[2], 256 * sizeof(int));
					memcpy(rcvData[3], rData + (j + 768), 256 * sizeof(int));

					cnt = 0;
					for (k = 0; k < 256; k++) {
						sendData[cnt++] = rcvData[0][k];
						sendData[cnt++] = rcvData[1][k];
						sendData[cnt++] = rcvData[2][k];
						sendData[cnt++] = rcvData[3][k];
					}

					write(c4s4[1], sendData, 1024 * sizeof(int));

				}
			}

			gettimeofday(&etime, NULL);

			time_result = (etime.tv_sec - stime.tv_sec) * 1000.0;      // sec to ms
			time_result += ((etime.tv_usec - stime.tv_usec) / 1000.0); // us to ms
			printf("Client_oriented_io Comm. %d TIMES == %.2f ms\n", i, time_result);

			exit(1);

		default:
			break;
		}
	}


	//Server Process
	for (i = 0; i < 4; i++) {
		switch (pid = fork()) {
		case -1:
			perror("error on fork");
		case 0:
			fd2 = open(ioNode[i], O_RDWR | O_CREAT | O_TRUNC | O_APPEND, 0644);

			// IO.
			gettimeofday(&stime, NULL);

			if (i == 0) {
				close(c1s1[1]);

				for (j = 0; j < 256; j++) {
					read(c1s1[0], ioData, 1024 * sizeof(int));
					write(fd2, ioData, 1024 * sizeof(int));
				}
			}
			if (i == 1) {
				close(c2s2[1]);

				for (j = 0; j < 256; j++) {
					read(c2s2[0], ioData, 1024 * sizeof(int));
					write(fd2, ioData, 1024 * sizeof(int));
				}
			}
			if (i == 2) {
				close(c3s3[1]);

				for (j = 0; j < 256; j++) {
					read(c3s3[0], ioData, 1024 * sizeof(int));
					write(fd2, ioData, 1024 * sizeof(int));
				}
			}
			if (i == 3) {
				close(c4s4[1]);

				for (j = 0; j < 256; j++) {
					read(c4s4[0], ioData, 1024 * sizeof(int));
					write(fd2, ioData, 1024 * sizeof(int));
				}
			}
			gettimeofday(&etime, NULL);

			time_result = (etime.tv_sec - stime.tv_sec) * 1000.0;      // sec to ms
			time_result += ((etime.tv_usec - stime.tv_usec) / 1000.0); // us to ms
			printf("Client_oriented_io IO. %d TIMES == %.2f ms\n", i, time_result);

			exit(1);

		default:
			break;
		}
	}

	waitpid(-1, NULL, 0);
}