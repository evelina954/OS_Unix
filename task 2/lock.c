#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>

int check_open(char *path, int flags, int mode) {
    int result = open(path, flags, mode);
    if (result == -1) {
        perror("Error 'open' file");
        exit(-1);
    }
    return result;
}

int check_write(int fd, char *buf, int count) {
    int result = write(fd, buf, count);
    if (result == -1) {
        perror("Error 'write' file");
        exit(-1);
    }
    return result;
}

void close_file(int fd) {
    int result = close(fd);
    if (result == -1)
        perror("Error 'close' file");
}

static int locks = 0;
static char* lockFileName = "somefile.lck";
static int pid;
static int pidLength;
static char* pidStr;

int check_unlink(char* lockFile) {
	int result = unlink(lockFile);
	if (result == -1) {
		perror("Error while deleting lock file");
		exit(-1);
	}
	return result;
}

int check_access(char* lockFile, int mode) {
	int result = access(lockFile, mode);
	if (result == -1) {
		perror("Missing lock file");
		free(pidStr);
		exit(-1);
	}
	return result;
}

int check_read(int fd) {
	int result = read(fd, pidStr, pidLength + 1);
	if (result == -1) {
		perror("Error while reading file");
		free(pidStr);
		exit(-1);
	}
	return result;
}

int lock() {
	int fd = -1;
	while (fd == -1)
		fd = open(lockFileName, O_CREAT | O_EXCL | O_RDWR, S_IRWXU);		
	locks++;

	pid = getpid();
	pidLength = snprintf(NULL, 0, "%d", pid);
	pidStr = malloc(pidLength + 1);
	if (pidStr == NULL) {
		perror("Error while allocating buffer with 'malloca'\n");
		return -1;
	}
	snprintf(pidStr, pidLength + 1, "%d", pid);
	check_write(fd, pidStr, pidLength);
	return fd;
}


void sigintHandler(int signum) { 
	int fd = check_open("stats", O_WRONLY | O_APPEND | O_CREAT, S_IRWXU);
	int strLength = snprintf(NULL, 0, "Success locks: %d\n", locks);
	char* statStr = malloc(strLength + 1);
	if (statStr == NULL) {
		perror("Error while allocating buffer with 'malloca'\n");
		exit(-1);
	}
	snprintf(statStr, strLength + 1, "Success locks: %d\n", locks);
	check_write(fd, statStr, strLength);
	exit(0);
}

int main(int argc, char *argv[]) {
	if (signal(SIGINT, sigintHandler) == SIG_ERR) {
		perror("Error with SIGINT");
		return -1;
	}

	while (1) {
		//lock
		int fd = lock();

		int mainFd = check_open("somefile", O_RDWR, 0);		
		sleep(1);
		close_file(mainFd);
		
		//unlock
		check_access(lockFileName, F_OK);
		lseek(fd, 0, SEEK_SET);
		check_read(fd);
		
		int newPid = atoi(pidStr);
		free(pidStr);
		if (newPid != pid) {
			perror("Different pids");
			return -1;
		}
		
		close_file(fd);
		check_unlink(lockFileName);
	}
	return 0;
}
