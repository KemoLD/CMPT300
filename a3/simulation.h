#include <stdio.h>
#include <stdlib.h>
#include "list.h"
#include <string.h>


enum State{
	running,
	ready,
	blocked,
};

typedef struct PCB PCB;
struct PCB{
	int PID;
	int priority;
	enum State state;
	List* assignedList;
	char* proc_message;
	int replyerPID;
};

typedef struct Semaphore Semaphore;
struct Semaphore{
	int value;
	List* list;
};

void Create(int priority);
void Fork();
void Kill(int PID);
void Exit();
void Quantum();
void Send(int PID, char* msg);
void Receive();
void Reply(int PID, char* msg);
void NewSemaphore(int ID, int value);
void SemaphoreP(int ID);
void SemaphoreV(int ID);
void ProcInfo(int PID);
void TotalInfo();





