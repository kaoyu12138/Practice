#include "myheader.h"

//用来从暂存消息的txt文件中取出消息
void getMessageA(){
    fseek(file, 0, SEEK_SET);
    fread(dataA, 1, sizeof(dataA), file);
    printf("Now message is: \n%s", dataA);
    if(ftruncate(fileno(file), 0) != 0) { // 使用ftruncate函数清空文件数据
        printf("无法清空文件\n");
        return 1;
    }
}

//用于创建消息，并暂时存入txt文件中
void createMessage(){
    printf("Please enter your message!\n");
    char input[1024];
    fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\n")] = 0;
    fprintf(file, "%s\n", input);
}

//用于取出txt文件中的消息，并放到共享内存区A中，即发送消息
void sendMessage(){
    getMessageA();

    semWaitEmptyA();
    LockA();

    memcpy(shm_addrA, &dataA, getMessageSizeA());
    printf("Send message success!\n");

    UnlockA();
    semPostFullA();
}

//用于从共享内存区B中获取数据，即接收消息
void receiveMessage(){
    semWaitFullB();
    LockB();

    memcpy(&MessageA, shm_addrB, sizeof(MessageA));
    shmctl(shmidB, IPC_RMID, NULL);
    printf("%s", MessageA);
    
    UnlockB(); 
    semPostEmptyB();
}

//用于清空txt里暂存的消息
void clearMessage(){
    FILE *file = fopen("MessageA.txt", "a+");
    if(ftruncate(fileno(file), 0) != 0){
        printf("Cancle message error");
        return -1;
    }
}

//用于清空共享内存区A中的数据，即撤销消息
void cancleMessagA(){
    if(shmctl(shmidA, IPC_RMID, NULL) == -1){
        printf("Cancle message error");
        return -1;
    }
}

int main(int argc, char *argv[]){
    if(argc > 3){
        printf("wrong command!");
        return -1;
    }

    CreateA();
    CreateB();
    file = fopen("MessageA.txt", "a+");
    
    if(strcmp(argv[1], "create") == 0) createMessage();
    else if(strcmp(argv[1], "send") == 0) sendMessage();
    else if(strcmp(argv[1], "clear") == 0) clearMessage();
    else if(strcmp(argv[1], "receive") == 0) receiveMessage();
    else if(strcmp(argv[1], "cancle") == 0) cancleMessagA();
    else{
        printf("wrong command!");
        return -1;
    }

    return 0;
}