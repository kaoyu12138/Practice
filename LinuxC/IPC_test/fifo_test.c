#include<sys/types.h>
#include<sys/stat.h>
#include<string.h>
#include<stdio.h>
#include<errno.h>
#include<fcntl.h>
#include<unistd.h>
#define FIFO_PATH "/tmp/test"

int fifo_read(){
    int ret = mkfifo(FIFO_PATH, 0644);
    if((ret == -1) && (errno != EEXIST)){
        perror("mkfifo");
        return -1;
    }
    
    int fd = open(FIFO_PATH, O_RDONLY);
    if (fd == -1){
        perror("open");
        return -1;
    }

    while(1){
        char buf[1024] = {0};
        read(fd, buf, 1024);
        printf("buf:%s\n", buf);
    }
    return 0;
}

int fifo_write(char * buf){
        int ret = mkfifo(FIFO_PATH, 0644);
    if((ret == -1) && (errno != EEXIST)){
        perror("mkfifo");
        return -1;
    }
    
    int fd = open(FIFO_PATH, O_WRONLY);
    if (fd == -1){
        perror("open");
        return -1;
    }
    
    while(1){
        write(fd, buf, strlen(buf));
        sleep(1);
    }
}

int main(int argc, char* argv[]){
    int mode = atoi(argv[1]);
    char *buf = argv[2];
    if(mode == 0){
        fifo_read();
    }else{
        fifo_write(buf);
    }
    return 0;
}