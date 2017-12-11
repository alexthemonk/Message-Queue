#include "headers.h"

int main (){
    char buffer[100];
    int msgQue, n;
    Message msg;
    key_t nodeKey;
  
    nodeKey = ftok(".", 'N');
    
    msgQue = msgget(nodeKey, IPC_CREAT | 0666);
    
    printf("Printing!\n");
  
    while(1){
        msgrcv(msgQue, &msg, size, 11, 0);
        printf("%s", msg.buffer);
    }
  
    return 0;
}
