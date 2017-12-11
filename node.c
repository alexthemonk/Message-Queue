#include "headers.h"


int main(int argc, char* argv[]){
    /* lets start with 4 nodes */
    int n = 4;
    
    int i;
    int* request; /* request for critical session, 1 for requesting, 0 for not */
    int msgQue;
    int* requestNum; /* nodes sequence number */
    int* highestRequestNum; /* highest request number seen */
    int* outstandingReply; /* number of unresolved replies */
    
    int shmID;
    
    int shmSize = sizeof(int) * (4 + n);
    
    if (argc < 2){
        printf("Error: not enough argument for node\n");
        exit(1);
    }
    
    const long me = (long) atoi(argv[1]); /* node number */
    
    /* semaphores */
    sem_t sem;
    // sem_t waitsem;
    
    /* initializing */
    if(sem_init(&sem, 1, 1) == -1){
        printf("Error initializing semaphore. \n");
        exit(1);
    }
    
    /* if(sem_init(&waitsem, 1, 0) == -1){
        printf("Error initializing semaphore. \n");
        exit(1);
    } */
    
    key_t nodeKey = ftok(".", 'N');
    key_t replyKey = ftok(".", 'B');
    
    msgQue = msgget(nodeKey, IPC_CREAT | 0660);
    // replyQue = msgget(replyKey, IPC_CREAT | 0660);
    
    shmID = shmget(IPC_PRIVATE, shmSize, IPC_CREAT | 0660);
    
    request = shmat(shmID, NULL, 0); /* request for critical session, 1 for requesting, 0 for not */
    requestNum = request + 1; /* nodes sequence number */
    highestRequestNum = request + 2; /* highest request number seen */
    outstandingReply = request + 3; /* number of unresolved replies */
    
    (*requestNum) = 0;
    (*request) = 0;
    (*highestRequestNum) = 0;
    (*outstandingReply) = n-1;
    
    int deferID;
    int* defer[n];
    
    for(i = 0; i < n; i++){
        /* 0 for not deferring, 1 for defer */
        defer[i] = request + 4 + i;
    }
    
    if(fork() == 0){
        /* child for receiving replies */
        Message receiveM;
        printf("\n---Reply Receiving Process started---\n");
        while(1){
            /* check sender */
            msgrcv(msgQue, &receiveM, size, 20+me, 0);
            printf("\nReply received from %d\n", receiveM.sender);
            
            (*outstandingReply) --;
            // sem_post(&waitsem);
        }
        exit(0);
    }
    if(fork() == 0){
        /* child for receiving requests */
        printf("\n---Request Receiving Process started---\n");
        Message receiveM;
        Message sendM;
        while(1){
            msgrcv(msgQue, &receiveM, size, me, 0);
            printf("\nRequest received from %d\n", receiveM.sender);
            if(receiveM.sequence > (*highestRequestNum)){
                (*highestRequestNum) = receiveM.sequence;
            }
            sem_wait(&sem);
            int deferIt = *request && ((receiveM.sequence > (*requestNum)) || ((receiveM.sequence == (*requestNum)) && (receiveM.sender > me)));
            // printf("%d\n", deferIt);
            sem_post(&sem);
            if(deferIt){
                (*(defer[receiveM.sender-1])) = 1;
                printf("\nBro #%d you ain't going anywhere\n", receiveM.sender);
            }
            else{
                printf("\nReply sent to %d, off you go\n", receiveM.sender);
                sendM.sender = me;
                sendM.receiver = (long) (receiveM.sender+20);
                sendM.sequence = (*requestNum);
                msgsnd(msgQue, &sendM, size, 0);
            }
        }
        exit(0);
    }
    
    /* parent for multual exclusion */
    
    while(1){
        /* wait for hitting enter to enter the requesting mode */
        char command[10];
        fgets(command , 9, stdin);
        if(strcmp(command, "quit\n")==0) break;
        
        sem_wait(&sem);
        (*request) = 1; /* yes I'm requesting */
        (*requestNum) = (*highestRequestNum) + 1;
        sem_post(&sem);
        (*outstandingReply) = n-1;
        /* send request to everyone */
        printf("\nSending new requests\n");
        Message sendM;
        Message printM;
        for(i = 0; i < n; i++){
            if(i != (int) (me-1)){
                // printf("%d should not be the same as %d\n", i+1, me);
                /* start sending requests */
                printf("\nSending request to %d\n", (i+1));
                sendM.sender = me;
                sendM.receiver = (long) (i+1);
                sendM.sequence = (*requestNum);
                msgsnd(msgQue, &sendM, size, 0);
            }
        }
        /* wait for every to reply*/
        while((*outstandingReply) > 0) {
            // sleep(1);
            // printf("%d\n", *outstandingReply);
            // sem_wait(&waitsem);
        }
        printf("\nCritical Session Entered\n");
        
        /* critical session here */
        char messageBuffer[100];
        sprintf (messageBuffer, "############## START OUTPUT FOR NODE %d ##############\n", me);
        printM.sender = me;
        printM.receiver = 11;
        strcpy(printM.buffer, messageBuffer);
        msgsnd(msgQue, &printM, size, 0);
        int randomTimes = (rand() % 10) + 1;
        i = 0;
        while (randomTimes){
            sprintf (messageBuffer, "Printing line %d...\n", i);
            strcpy(printM.buffer, messageBuffer);
            msgsnd(msgQue, &printM, size, 0);
            sleep(1);
            randomTimes--;
            i++;
        }
        
        
        sprintf (messageBuffer, "--------------  END OUTPUT FOR NODE %d  --------------\n", me);
        strcpy(printM.buffer, messageBuffer);
        msgsnd(msgQue, &printM, size, 0);
        
        printf("\nCritical Session Finished\n");
        Message replyM;
        *request = 0; /* I finished, not requesting anymore */
        for(i = 0; i < n; i++){
            if(*(defer[i])){
                printf("\nReply sent to %d, off you go\n", (long)(i+1));
                (*(defer[i])) = 0;
                replyM.sender = me;
                replyM.receiver = (long) (i+1+20);
                replyM.sequence = (*requestNum);
                msgsnd(msgQue, &replyM, size, 0);
            }
        }
    }
    
    printf("\nExiting...\n");
    kill(getpid()+1, SIGKILL);
    kill(getpid()+2, SIGKILL);
    
    return 0;
}