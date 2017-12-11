#include "headers.h"

int main(){
    int msgQue;
  
    key_t nodeKey;
    nodeKey = ftok(".", 'N');
  
    msgQue = msgget(nodeKey, IPC_CREAT | 0666);
  
    Message msg;
    msg.receiver = 11; /* 11 for printer */
    msg.sender = 12; /* 12 for hacker */
    printf("Hacker started working!\n");
    strcpy(msg.buffer , "Hacking what's up!!\n");
  
    int randomTime;
  
    while (1){
        randomTime = rand() %10 + 1;
        sleep(randomTime);
        printf("Hacking!\n");
        msgsnd(msgQue, &msg, size, 0);
    }
}
