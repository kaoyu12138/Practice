#include<sys/types.h>
#include<sys/ipc.h>
#include<unistd.h>
#include<string.h>
#include<stdlib.h>
#include<stdio.h>

#define _USE_GNU
#include<sys/msg.h>

struct msgbuf {
    long mtype;       // 消息类型
    char mtext[1];    // 消息内容
};


int main(int argc, char* argv[]){
    int mode = atoi(argv[1]);
    int msgtype = atoi(argv[2]);
    char *str = argv[3];

    //key_t key = 1234;
    //key_t key = IPC_PRIVATE 只能用于亲缘关系进程
    key_t key = ftok("/tmp/test", 1);
    int id = msgget(key, 0644 | IPC_CREAT);
    printf("%d\n", id);

    while(1){
        if(mode == 0){
            struct msgbuf msg = {0};
            int ret = msgrcv(id, &msg, 1024, msgtype, 0);
            
            if(ret == -1){
                perror("msgrcv");
                break;
            }
            printf("ret:%d,type:%d,mtext:%s\n", ret, msgtype, msg.mtext);
        }else{
            struct msgbuf msg = {0};
            memcpy(msg.mtext, str, strlen(str));
            int ret = msgsnd(id, &msg, strlen(str), 0);

            if(ret == -1){
                perror("msgsnd");
                break;
            }
            sleep(1);
        }
    }
    
    msgctl(id, IPC_RMID, 0);
    return 0;
}