#include "myheader.h"

//断开共享内存区的连接，并释放，同时释放三个信号量
void disconnectA(){
    shmdt(shm_addrA);
    int sh = shmctl(shmidA, IPC_RMID, NULL);

    semctl(mutexA, 0, IPC_RMID);
    semctl(fullA, 0, IPC_RMID);
    semctl(emptyA, 0, IPC_RMID);
}

void disconnectB(){
    shmdt(shm_addrB);
    int sh = shmctl(shmidB, IPC_RMID, NULL);

    semctl(mutexB, 0, IPC_RMID);
    semctl(fullB, 0, IPC_RMID);
    semctl(emptyB, 0, IPC_RMID);
}

int main(){
    CreateA();
    CreateB();
    disconnectA();
    disconnectB();

    return 0;
}