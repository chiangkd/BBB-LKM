#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <error.h>
#include <stdio.h>
#include <stdlib.h>

char buffer[2048];

#define TRY_READ 10

#define DEVICE_NAME0 "/dev/pseudo0"
#define DEVICE_NAME1 "/dev/pseudo1"
#define DEVICE_NAME2 "/dev/pseudo2"
#define DEVICE_NAME3 "/dev/pseudo3"

int main (int argc, char *argv[])
{
    int fd;

    // This variable holds remaining data bytes to be read
    int remain = TRY_READ;

    // Hold count of total data bytes read so far
    int total_read = 0;

    int n = 0, ret = 0;

    if (argc != 2) {
        printf("Incorrect usage\n");
        printf("Correct usage: <file> <readcount>\n");
        return -1;
    }

    remain = atoi(argv[1]);

	printf("read requested = %d\n", remain);
	fd = open(DEVICE_NAME3, O_RDONLY);

	if(fd < 0){
		// perror decodes user space errno variable and prints cause of failure
		perror("open");
		return fd;
	}
	printf("open was successful\n");

#if 1
	/*activate this for lseek testing */
	ret = lseek(fd, 10, SEEK_SET);
	if(ret < 0){
		perror("lseek");
		close(fd);
		return ret;
	}
#endif
	// Attempt reading twice
	
	while(n != 2 && remain) {
		// read data from 'fd'
		ret = read(fd,&buffer[total_read], remain);

		if(!ret){
			// There is nothing to read
			printf("end of file \n");
			break;
		} else if (ret <= remain) {
			printf("read %d bytes of data \n",ret );
			// 'ret' contains count of data bytes successfully read , so add it to 'total_read'
		        total_read += ret;
			// We read some data, so decrement 'remain'
			remain -= ret;
		} else if (ret < 0) {
			printf("something went wrong\n");
			break;
		} else
			break;

		n++;
	}

	printf("total_read = %d\n",total_read);

	//dump buffer
	for(int i=0 ; i < total_read ; i++)
		printf("%c", buffer[i]);

	close(fd);
	
	return 0;

}