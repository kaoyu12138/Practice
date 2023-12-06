#include "myheader.h"

void getMessageB(){
    fseek(file, 0, SEEK_SET);
    fread(dataB, 1, sizeof(dataB), file);
    printf("Now message is: \n%s", dataB);
    if(ftruncate(fileno(file), 0) != 0) { // 使用ftruncate函数清空文件数据
        printf("无法清空文件\n");
        exit(-1);
    }
}

void createMessage(){
    printf("Please enter your message!\n");
    char input[1024];
    fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\n")] = 0;
    fprintf(file, "%s\n", input);
}

void sendMessage(){
    getMessageB();

    semWaitEmptyB();
    LockB();

    memcpy(shm_addrB, &dataB, getMessageSizeB());
    printf("Send message success!\n");

    UnlockB();
    semPostFullB();
}

void receiveMessage(){
    semWaitFullA();
    LockA();

    memcpy(&MessageB, shm_addrA, sizeof(MessageB));
    shmctl(shmidA, IPC_RMID, NULL);
    printf("%s", MessageB);
    
    UnlockA(); 
    semPostEmptyA();
}

void clearMessage(){
    FILE *file = fopen("MessageB.txt", "a+");
    if(ftruncate(fileno(file), 0) != 0){
        printf("Clear message error");
        exit(-1);
    }
}

void cancelMessagB(){
    if(shmctl(shmidB, IPC_RMID, NULL) == -1){
        printf("Cancle message error");
        exit(-1);
    }
    semWaitFullB();
}

int main(int argc, char *argv[]){
    if(argc > 3){
        printf("wrong command!");
        return -1;
    }

    CreateA();
    CreateB();
    file = fopen("MessageB.txt", "a+");
    
    if(strcmp(argv[1], "create") == 0) createMessage();
    else if(strcmp(argv[1], "send") == 0) sendMessage();
    else if(strcmp(argv[1], "clear") == 0) clearMessage();
    else if(strcmp(argv[1], "receive") == 0) receiveMessage();
    else if(strcmp(argv[1], "cancel") == 0) cancelMessagB();
    else{
        printf("wrong command!");
        return -1;
    }

    return 0;
}