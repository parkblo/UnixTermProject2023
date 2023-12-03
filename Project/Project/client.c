#include "mytest.h"
#define BUFSIZE 1024*1024

void sig_handler(int signo) {
	;
}
void child(int p[2], int i, pid_t pid[4]);
void parent(int p[4][2]);
int main() {
	struct sigaction act;
	char buf1[BUFSIZE], buf2[BUFSIZE], buf3[BUFSIZE], buf4[BUFSIZE];
	int p[4][2], fd[4][2], cpid[4], i, j, p2[2], p3[2], p4[2];
	pid_t my_pid, pid, r;

	memset(cpid, 0, sizeof(cpid));
	act.sa_handler = sig_handler;
	sigaction(SIGUSR1, &act, NULL);

	for (i = 0; i < 4; i++) {
		pipe(p[i]);
		switch (pid = fork()) {
		case -1:
			perror("error on fork");
		case 0:

		default:

		}
	}

	parent(p);
}


void parent(int p[4][2]) {
	char buf[BUFSIZE];
	int io_1, io_2, io_3, io_4, i, j;
	fd_set set;

	io_1 = open("io_node_1", O_RDWR | O_CREAT | O_TRUNC, 0644);
	io_2 = open("io_node_2", O_RDWR | O_CREAT | O_TRUNC, 0644);
	io_3 = open("io_node_3", O_RDWR | O_CREAT | O_TRUNC, 0644);
	io_4 = open("io_node_4", O_RDWR | O_CREAT | O_TRUNC, 0644);

	FD_ZERO(&set);
	for (i = 0; i < 4; i++) {
		close(p[i][1]);
		FD_SET(p[i][0], &set);
	}

	while (select(p[3][0] + 1, &set, NULL, NULL, NULL) > 0) {
		for (i = 0; i < 4; i++) {
			if (FD_ISSET(p[i][0], &set)) {
				if (read(p[i][0], &buf, BUFSIZE) > 0) {
					if (i == 0) write(io_1, &buf, BUFSIZE);
					else if (i == 1) write(io_2, &buf, BUFSIZE);
					else if (i == 2) write(io_3, &buf, BUFSIZE);
					else write(io_4, &buf, BUFSIZE);
				}
			}
		}
		if (waitpid(-1, NULL, WNOHANG) == -1) return;
	}

}