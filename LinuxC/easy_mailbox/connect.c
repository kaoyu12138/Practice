#include "myheader.h"

//为A信箱的三个信号量设置初值
void SetA(){
    semctl(mutexA, 0, SETVAL, argM);
    semctl(fullA, 0, SETVAL, argF);
    semctl(emptyA, 0, SETVAL, argE);
}

void SetB(){
    semctl(mutexB, 0, SETVAL, argM);
    semctl(fullB, 0, SETVAL, argF);
    semctl(emptyB, 0, SETVAL, argE);
}

int main(){
    argM.val = 1;
    argF.val = 0;
    argE.val = bufsize;

    CreateA();
    SetA();
    CreateB();
    SetB();

    return 0;
}