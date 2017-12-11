#ifndef HEADER
#define HEADER

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <unistd.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>
#include <sched.h>
#include <sys/wait.h>

typedef struct {
    long receiver; // aka the type
    long sender;
    int sequence;
    char buffer[100];
} Message;

void debug();
void msgInfo(Message m);

const int size = sizeof(Message); /* size of a message */

const char REQUEST[2] = "F";
const char REPLY[2] = "U";
const char PRINT[2] = "L";

const long REQ = (long) "F";
const long REP = (long) "U";
const long PRI = (long) "L";

void debug(){
    printf("WTF\n");
}
void msgInfo(Message m){
    printf("WTF\n");
    printf("To: %d\n", m.receiver);
    printf("From: %d\n", m.sender);
    printf("Message: %s\n", m.buffer);
    printf("End of WTF\n");
}

#endif