#ifndef MYHEADER_H
#define MYHEADER_H
#include<stdio.h>
#include<errno.h>
#include<unistd.h>
#include<sys/shm.h>
#include<sys/socket.h>
#include<string.h>
#include<sys/ipc.h>
#include<sys/types.h>
#include<sys/sem.h>

#define bufsize 4096
union semun {
    int val;                // value for SETVAL
    struct semid_ds *buf;   // buffer for IPC_STAT, IPC_SET
    unsigned short *array;  // array for GETALL, SETALL
    struct seminfo *__buf;  // buffer for IPC_INFO
};

key_t keyA, keyB;           //生成两片共享内存区的key值
int shmidA, shmidB;         //两片共享内存区的标识
void* shm_addrA, *shm_addrB;//两片共享内存区的首地址
char dataA[bufsize], dataB[bufsize];    //用作接收进程创建的，但还未发送的消息
char MessageA[bufsize], MessageB[bufsize];//用于接收共享内存区里的数据

//6个信号量，用于并发访问控制，实现基于生产者消费者的信箱
int mutexA, mutexB, fullA, fullB, emptyA, emptyB;
key_t keymutexA, keymutexB, keyfullA, keyfullB, keyemptyA, keyemptyB;
union semun argM, argF, argE;
struct sembuf sb;
FILE *file;

//创建内存共享区A作为进程A的信箱，并创建三个信号量
void CreateA(){
    //创建一片大小为4096的共享内存区A，作为信箱A，key值为ftok("/home",0x111)
    keyA = ftok("/home", 0x100);
    shmidA = shmget(keyA, bufsize, IPC_CREAT | 0600);
    if(shmidA == -1){
        printf("shmget error");
    }
    shm_addrA = shmat(shmidA, NULL, 0);

    //创建三个信号量集，分别用作互斥信号量、资源信号量full、资源信号量empty
    keymutexA = ftok("/home", 0x110);
    keyfullA = ftok("/home", 0x120);
    keyemptyA = ftok("/home", 0x130);
    
    mutexA = semget(keymutexA, 1, IPC_CREAT | 0666);
    fullA = semget(keyfullA, 1, IPC_CREAT | 0666);
    emptyA = semget(keyemptyA, 1, IPC_CREAT | 0666);
}

void CreateB(){
    keyB = ftok("/home", 0x777);
    shmidB = shmget(keyB, bufsize, IPC_CREAT | 0600);
    if(shmidB == -1){
        printf("shmget error");
    }
    shm_addrB = shmat(shmidB, NULL, 0);

    keymutexB = ftok("/home", 0x210);
    keyfullB = ftok("/home", 0x220);
    keyemptyB = ftok("/home", 0x230);

    mutexB = semget(keymutexA, 1, IPC_CREAT | 0666);
    fullB = semget(keyfullA, 1, IPC_CREAT | 0666);
    emptyB = semget(keyemptyA, 1, IPC_CREAT | 0666);
}

//获取进程A暂存信息的大小
int getMessageSizeA(){
    return sizeof(dataA);
}

int getMessageSizeB(){
    return sizeof(dataB);
}

//获取信箱A内数据量的大小
int getSpaceSizeA(){
    int datasize = 0;
    char* dataPtr = (char*)shm_addrA;
    while(dataPtr[datasize] != '\0'){
        datasize ++;
    }
    return datasize;
}

int getSpaceSizeB(){
    int datasize = 0;
    char* dataPtr = (char*)shm_addrB;
    while(dataPtr[datasize] != '\0'){
        datasize ++;
    }
    return datasize;
}

void LockA(){
    sb.sem_num = 0;
    sb.sem_op = -1;
    sb.sem_flg = 0;
    semop(keymutexA, &sb, 1);
}

void UnlockA(){
    sb.sem_num = 0;
    sb.sem_op = 1;
    sb.sem_flg = 0;
    semop(keymutexA, &sb, 1);
}

void LockB(){
    sb.sem_num = 0;
    sb.sem_op = -1;
    sb.sem_flg = 0;
    semop(keymutexB, &sb, 1);
}

void UnlockB(){
    sb.sem_num = 0;
    sb.sem_op = 1;
    sb.sem_flg = 0;
    semop(keymutexB, &sb, 1);
}

void semWaitEmptyA(){
    sb.sem_num = 0;
    sb.sem_op = getMessageSizeA()*-1;
    sb.sem_flg = IPC_NOWAIT;
    if(semop(keyemptyA, &sb, 1)==-1){
        if(errno == EAGAIN){
            printf("The space is full, send error, Please retry later!");
        }
    }
}

void semPostEmptyA(){
    sb.sem_num = 0;
    sb.sem_op = getSpaceSizeA();
    sb.sem_flg = 0;
    semop(keyemptyB, &sb, 1);
}

void semWaitEmptyB(){
    sb.sem_num = 0;
    sb.sem_op = getMessageSizeB()*-1;
    sb.sem_flg = IPC_NOWAIT;
    if(semop(keyemptyB, &sb, 1)==-1){
        if(errno == EAGAIN){
            printf("The space is full, send error, Please retry later!");
        }
    }
}

void semPostEmptyB(){
    sb.sem_num = 0;
    sb.sem_op = getSpaceSizeB();
    sb.sem_flg = 0;
    semop(keyemptyB, &sb, 1);
}


void semWaitFullA(){
    sb.sem_num = 0;
    sb.sem_op = -1;
    sb.sem_flg = IPC_NOWAIT;
    if(semop(keyfullA, &sb, 1)==-1){
        if(errno == EAGAIN){
            printf("The space is empty, receive error, Please retry later!");
        }
    }
}

void semPostFullA(){
    sb.sem_num = 0;
    sb.sem_op = 1;
    sb.sem_flg = 0;
    semop(keyfullA, &sb, 1);
}

void semWaitFullB(){
    sb.sem_num = 0;
    sb.sem_op = -1;
    sb.sem_flg = IPC_NOWAIT;
    if(semop(keyfullB, &sb, 1)==-1){
        if(errno == EAGAIN){
            printf("The space is empty, receive error, Please retry later!");
        }
    }
}

void semPostFullB(){
    sb.sem_num = 0;
    sb.sem_op = 1;
    sb.sem_flg = 0;
    semop(keyfullB, &sb, 1);
}

#endif
