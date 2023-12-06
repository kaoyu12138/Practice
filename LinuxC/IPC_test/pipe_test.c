#include <unistd.h>
#include <stdio.h>
#include <string.h>

int pipe_test(){
    int fd[2];
    int ret = pipe(fd);
    if(ret == -1){
        perror("pipe");
        return -1;
    }
    
    ret = fork();
    if(ret == 0){
        close(fd[0]);
        #define TEST_STRING "12138"
        while(1){
            write(fd[1], TEST_STRING, strlen(TEST_STRING));
            sleep(1);
        }
    }else if(ret > 0){
        close(fd[1]);
        while(1){
            char buf[1024] = {0};
            read(fd[0], buf, 1024);
            printf("buf:%s\n", buf);
        }
    }else{

    }
    return 0;
}

int main(int argc, char* argv[]){
    int ret = pipe_test();
    return 0;
}