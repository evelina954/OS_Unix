#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>


int parse_block_size(int argc, char *argv[]) {
	int opt;
    int blockSize = 4096;
	while ((opt = getopt(argc, argv, ":b:")) != -1) {
		switch (opt) {
		case 'b':
			blockSize = atoi(optarg);
			break;}
	}
    return blockSize;
}


int check_open(char *path, int flags, int mode) {
    int result = open(path, flags, mode);
    if (result == -1) {
		perror(ERROR_FILE_MSG);
		exit(-1);
    }
    return result;
}

int check_read(int fd, char *buf, int count)
{
    int result = read(fd, buf, count);
    if (result == -1) {
        perror("Error 'read' file");
        exit(-1);
    }
    return result;
}

void check_lseek(int fd, int offset, int whence)
{
    int result = lseek(fd, offset, whence);
    if (result == -1) {
        perror("Error 'lseek' file");
        exit(-1);
    }
    return;
}

void check_write(int fd, char *buf, int count)
{
    int result = write(fd, buf, count);
    if (result == -1) {
        perror("Error 'write' file");
        exit(-1);
    }
    return;
}

void close_file(int fd)
{
    int result = close(fd);
    if (result == -1)
        perror("Error 'close' file");
}


int main(int argc, char *argv[]) {
	char *f1;
	char *f2;
    int fd1 = 0;
    int fd2 = 0;
    int blockSize = parse_block_size(argc, argv);
	if (optind < argc) {
		f1 = argv[optind++];
	}
	else {
		printf("Missing the filename\n");
		return -1;
	}
	if (optind < argc) {
		f2 = argv[optind++];
		fd1 = check_open(f1, O_RDONLY, 0);
		fd2 = check_open(f2, O_WRONLY|O_TRUNC|O_CREAT, S_IRWXU);
	}
	else {
		fd2 = check_open(f1, O_WRONLY|O_TRUNC|O_CREAT, S_IRWXU);
	}
	
	int flag;
	int zeroBytes = 0;
	int curRead = 0;
	char buffer[blockSize];

    while ((curRead = check_read(fd1, buffer, blockSize)) != 0) {
        flag = 1;
        for (int i = 0;i < curRead;i++) {
			if (buffer[i] != 0) {
				flag = 0;
				break;}
		}
        if (flag) {
			zeroBytes += curRead;
			continue;
        }
        if (zeroBytes != 0) {
			check_lseek(fd2, zeroBytes, SEEK_CUR);
			zeroBytes = 0;
        }
        check_write(fd2, buffer, curRead);
    }
    if (zeroBytes != 0)
        check_lseek(fd2, zeroBytes, SEEK_CUR);
    if (fd1 != 0)
        close_file(fd1);
    close_file(fd2);
    return 0;

}
