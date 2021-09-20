#include "simulation.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

List* readyHigh;
List* readyNorm;
List* readyLow;
List* waitSend;
List* waitReceive;
Semaphore semaphoreList[5];

PCB* currProcess; 
PCB* init;
int usedPIDs = 0;

void Initialise();
int processCount();
void startNewProcess();
void Print();
PCB* searchAllByPID(int PID);
PCB* searchByReplyerPID(int PID, List* list);
bool equalReplyerPID(void* curr, void* pComparisonArg);
PCB* searchByPID(int PID, List* list);
bool equalPID(void* curr, void* pComparisonArg);
int getPID();
void End();
void Menu();
void printMenu();
int addToReadyQ(PCB* process);
char* getStateName(PCB* process);
char* getListName(PCB* process);

void Initialise(){

	init = (PCB*)malloc(sizeof(PCB));
	init->PID = getPID();
	init->priority = 0;
	init->state = running;
	init->assignedList = NULL;
	init->proc_message = NULL;
	currProcess = init;

	readyHigh = List_create();
    	readyNorm = List_create();
    	readyLow = List_create();
    	waitSend = List_create();
    	waitReceive = List_create();
    	
    	for(int i = 0; i < 5; i++){
    		semaphoreList[i].list = NULL;
    	}
    	
}

void Create(int priority){

	printf("Attempting to create new process of priority %d... \n", priority);
	if(priority < 0 || priority > 2){
		printf("FAILURE! the priority must be between 0 and 2 \n");
		return;
	}
	

	PCB* process = (PCB*)malloc(sizeof(PCB));
	process->PID = getPID();
	process->priority = priority;
	process->state = ready;
	process->proc_message = NULL;	
	int result = addToReadyQ(process);
	
	
	if(result == 0){
		printf("SUCCESS! \n");
		printf("new process of PID %d was created\n", process->PID);
		if(currProcess == init){
			startNewProcess();
			printf("the process with PID %d is now currently running \n", currProcess->PID);
		}

	}
	else{
		printf("FAILURE creating a new process\n");
		return;
	}
	
}

void Fork(){

	if(currProcess != init){

		PCB* process = (PCB*)malloc(sizeof(PCB));
		process->PID = getPID();
		process->priority = currProcess->priority;
		process->state = ready;
		process->proc_message = NULL;	

		int result = addToReadyQ(process);
		
		
		if(result == 0){
			printf("SUCCESS \n");
			printf("new forked process of PID %d created\n", process->PID);
			return;
		}
		else{
			printf("FAILURE \n");
			return;
		}

	}

	printf("cannot fork init process \n");
	return;

}

void Kill(int PID){

	printf("Attempting to KILL process with PID: %d \n", PID);
	if(PID == init->PID && currProcess != init){
		printf("The PID provided corresponds to the INIT process, it cannot be killed while there are other processes available \n");
		return;
	}
	else if(PID == init->PID && processCount() == 0){
		printf("Ending program... \n");
		End();
	}

	PCB* process = searchAllByPID(PID);
	if(currProcess->PID == PID){
		Exit();
		return;
	}
	else if(process == NULL){
		printf("process of PID: %d was not found \n", PID);
		return;
	}

	List_first(process->assignedList);
	if(List_search(process->assignedList, equalPID, &PID)){

		List_remove(process->assignedList);
		free(process);
		printf("SUCCESS the process with PID: %d was killed\n", PID);
	}
	else{
		printf("FAILURE killing process\n");
	}

}

int processCount(){
	int count = 0;
	count += List_count(readyHigh);
	count += List_count(readyLow);
	count += List_count(readyNorm);
	count += List_count(waitReceive);
	count += List_count(waitReceive);

	if(semaphoreList[0].list){
		count += List_count(semaphoreList[0].list);
	}
	if(semaphoreList[1].list){
		count += List_count(semaphoreList[1].list);
	}
	if(semaphoreList[2].list){
		count += List_count(semaphoreList[2].list);
	}
	if(semaphoreList[3].list){
		count += List_count(semaphoreList[3].list);
	}
	if(semaphoreList[4].list){
		count += List_count(semaphoreList[4].list);
	}
	

	return count; 
}

void Exit(){
	
	if (currProcess != init){

		PCB* oldProcess;
		printf("Exiting currently running process...\n");
		oldProcess = currProcess;
		startNewProcess();
		printf("Exited process with PID: %d, and switched to process with PID: %d \n", oldProcess->PID, currProcess->PID);
		free(oldProcess);
		return;

	}
	else if(currProcess == init && processCount() > 0){
		printf("FAILURE! the INIT process cannot be killed unless it is the only available one \n");
		return;
	}
	else{
		printf("Ending program.. \n");
		End();
	}

}

void Quantum(){

	if(currProcess != init){
	
		if(processCount() == 0){
			printf("FAILURE! there are no other ready processes\n");
			return;
		}
		
		PCB* oldProcess = currProcess;
		startNewProcess();
		int result = addToReadyQ(oldProcess);
		
		
		if(result == 0){
			printf("SUCCESS \n");
			printf("Quatum process with PID: %d, and switched to process with PID: %d", oldProcess->PID, currProcess->PID);
			return;
		}
		else{
			printf("FAILURE Quantum process\n");
			return;
		}

			
	}
	else{
		printf("INIT process curently running, cannot Quantum \n");
		return;
	}
}

void Send(int PID, char* msg){

	int result;
	PCB* process = searchAllByPID(PID);

	if(strlen(msg) > 40 ){
		printf("FAILURE! message cannot exceed 40 characters \n");
		return;
	}
	
	while(PID != currProcess->PID){
	
		if(process == NULL){
			printf("FAILURE! process of PID: %d was not found \n", PID);
			return;
		}
	
		if(process->assignedList == waitReceive){

			List_first(process->assignedList);
			List_search(process->assignedList, equalPID, process);
			List_remove(process->assignedList);
			
			result = addToReadyQ(process);
			
			if(result == 0){
				printf("SUCCESS \n");
				printf("message was succesfully sent to process with PID: %d, it is now unblocked and placed in a ready queue\n", process->PID);
				printf("MESSAGE: %s \n", msg);
				return;
			}
			else{
				printf("FAILURE unblocking process \n");
				return;
			}
		}
		else{

			if(currProcess == init){
				init->proc_message = msg;
				init->replyerPID = PID;
				printf("SUCESS! however, the current process corresponds to the INIT process, you cannot block it \n");
				return;
			}

			PCB* oldProcess = currProcess;
			oldProcess->state = blocked;
			oldProcess->proc_message = msg;
			oldProcess->replyerPID = PID;
			oldProcess->assignedList = waitSend;
			result = List_prepend(waitSend, oldProcess);
			startNewProcess();

			if(result == 0){
				printf("SUCCESS! \n");
				printf("message was succesfully sent to process with PID: %d, the process with PID: %d is now blocked until it receives a reply \n", process->PID ,oldProcess->PID);
				printf("the process with PID: %d is now currently running \n", currProcess->PID);
				return;
			}
			else{
				printf("FAILURE sending message \n");
				return;
			}

		}
	}
	printf("FAILURE! A process cannot send a message to itself \n");
}

void Receive(){
	PCB* process = currProcess;
	PCB* sender = NULL;

	sender = searchByReplyerPID(process->PID, waitSend);

	if(sender == NULL){
		if(init->replyerPID == process->PID){
			printf("SUCCESS! \n");
			printf("Received message from (INIT)process with PID: %d, saying %s \n", init->PID, init->proc_message);
			return;
		}
		else if(currProcess != init){

			process->state = blocked;
			process->assignedList = waitReceive;
			int result = List_prepend(waitReceive, process);
			startNewProcess();

			if(result == 0){
				printf("SUCCESS \n");
				printf("This process of PID: %d is blocked and awaiting a message \n", process->PID);
				printf("the new running process is of PID: %d \n", currProcess->PID);
				return;
			}
			else{
				printf("FAILURE! blocking receiver \n");
				return;
			}		

		}
		else{
			printf("FAILURE! the current running process corresponds to INIT process, so it cannot be blocked while awaiting a message \n");
			return;
		}
	}
	else{

		printf("SUCCESS \n");
		printf("Received message from process with PID: %d, saying %s \n", sender->PID, sender->proc_message);
		
	}

}

void Reply(int PID, char* msg){

	PCB* process = currProcess;
	PCB* sender = NULL;
	int result;

	if(strlen(msg) > 40 ){
		printf("FAILURE! message cannot exceed 40 characters \n");
		return;
	}

	List_first(waitSend);
	sender = List_search(waitSend, equalPID, &PID);
	
	if(init->PID == PID && init->replyerPID == process->PID){
		printf("SUCCESS! reply to (init)process with PID: %d saying: %s \n", init->PID, msg);
		init->proc_message = NULL;
		sender->replyerPID = -1; 
		return;
	}
	else if(sender == NULL){		
		printf("FAILURE! the target process is not awaiting a response \n");
		return;
	
	}
	else{

		List_remove(waitSend);
		sender->proc_message = NULL;
		sender->replyerPID = -1; 


		result = addToReadyQ(sender);

		if(result == 0){
			printf("SUCCESS \n");
			printf("Replied to process with PID:%d, saying %s \n", sender->PID, msg);
			return;
		}
		else{
			printf("FAILURE sending a reply \n");
			return;
		}
	}

}

void NewSemaphore(int ID, int value){

	printf("Creating a new Semaphore...\n");
	if(ID < 0 || ID > 4){
		printf("FAILURE! the ID can only take values from 0 to 4 \n");
		return;
	}

	if(semaphoreList[ID].list != NULL){
		printf("FAILURE! a semaphore of this ID already exists \n");
		return;
	}

	semaphoreList[ID].value = value;
	semaphoreList[ID].list = List_create();

	if (semaphoreList[ID].list != NULL ){
		printf("SUCCESS! the semaphore of ID: %d has been created and given a value of %d \n", ID, value);
	}
	else{
		printf("FAILURE creating semaphore\n");
	}	

}

void SemaphoreP(int ID){

	if(ID < 0 || ID > 4){
		printf("FAILURE! the ID can only take values from 0 to 4 \n");
		return;
	}

	if( semaphoreList[ID].list == NULL){
		printf("this sempahore ID has not been created yet. Create one first \n");
		return;
	}

	
	if(semaphoreList[ID].value > 0){
		semaphoreList[ID].value--;
		printf("SUCCESS! the semaphore ID %d is positive, \n", ID);
		printf("so it has been changed from %d to %d \n", semaphoreList[ID].value+1, semaphoreList[ID].value  );
	}
	else{
		if(currProcess != init){
			semaphoreList[ID].value--;
			PCB* process = currProcess;
			process->state = blocked;
			process->assignedList = semaphoreList[ID].list;
			int result = List_prepend(semaphoreList[ID].list, process);
			startNewProcess();

			if (result == 0){
				printf("SUCCESS! this semaphore ID: %d is negative, \n", ID);
				printf("semaphore P executed, the current process ID: %d has now been placed in a blocked queue \n", process->PID);
				printf("It's new value is %d \n", semaphoreList[ID].value);
				printf("The new running process is of PID: %d \n", currProcess->PID);
			}
			else{
				printf("FAILURE executing the semaphore P operation \n");
			}
		}
		else{
			printf("FAILURE! the init process cannot be blocked \n" );
		}

	}
}

void SemaphoreV(int ID){

	if(ID < 0 || ID > 4){
		printf("FAILURE! the ID can only take values from 0 to 4 \n");
		return;
	}

	if( semaphoreList[ID].list == NULL){
		printf("this sempahore ID has not been created yet. Create one first \n");
		return;
	}

	int result;

	if(semaphoreList[ID].value > 0){

		
		PCB* process = NULL;
		process = List_last(semaphoreList[ID].list);
		List_trim(semaphoreList[ID].list);
		if(process){

			result = addToReadyQ(process);
			semaphoreList[ID].value++;

			if(result == 0){
				printf("SUCCESS! semaphore V executed, the process with PID %d has now unblocked and placed in a ready queue\n", process->PID);
				printf("the new semaphore value for ID %d is %d \n", ID, semaphoreList[ID].value);
				return;
			}
			else{
				printf("FAILURE doing semaphoreV operation \n");
				return;
			}

		}
		else{
			printf("FAILURE! no processes are blocked by this semaphore \n");
		}
	}
	else{
		semaphoreList[ID].value++;
		printf("FAILURE! the value of this semaphore is less than 0 \n");
		printf("the semaphore value for ID %d is %d \n", ID, semaphoreList[ID].value);
	}
}

char* getListName(PCB* process){
	
	char* list;
	if(process->assignedList == readyHigh){
		list = "ready Queue high priority";
	}
	else if(process->assignedList == readyNorm){
		list = "ready Queue normal priority";
	}
	else if(process->assignedList == readyLow){
		list = "ready Queue low priority";
	}
	else if(process->assignedList == waitReceive){
		list = "blocked receiver queue ";
	}
	else if(process->assignedList == waitSend){
		list = "blocked semaphore queue";
	}
	if(semaphoreList[0].list != NULL){
		if(process->assignedList == semaphoreList[0].list){
			list = "blocked semaphore list of ID: 0";
		}
	}
	if(semaphoreList[1].list != NULL){
		if(process->assignedList == semaphoreList[1].list){
			list = "blocked semaphore list of ID: 1";
		}
	}
	if(semaphoreList[2].list != NULL){
		if(process->assignedList == semaphoreList[2].list){
			list = "blocked semaphore list of ID: 2";
		}
	}
	if(semaphoreList[3].list != NULL){
		if(process->assignedList == semaphoreList[3].list){
			list = "blocked semaphore list of ID: 3";
		}
	}
	if(semaphoreList[4].list != NULL){
		if(process->assignedList == semaphoreList[4].list){
			list = "blocked semaphore list of ID: 4";
		}
	}
	
	return list;

}

char* getStateName(PCB* process){

	char* state;
	if(process->state == running){
		state = "running";
	}
	else if(process->state == ready){
		state = "ready";
	}
	else if(process->state == blocked){
		state = "blocked";
	}
}

void ProcInfo(int PID){

	PCB* process = searchAllByPID(PID);
	if(process){

		printf("\n");
		printf("Process PID: %d \n", process->PID );
		printf("state: %s \n", getStateName(process) );
		printf("priority: %d \n", process->priority);
		printf("assigned List: %s \n", getListName(process) );
		if(process->proc_message != NULL){
			printf("contains message: %s \n", process->proc_message);
		}
	}
	else if(PID == currProcess->PID){
	

		printf("Process PID: %d \n", currProcess->PID );
		printf("This is the current process \n");
		printf("priority: %d \n", currProcess->priority);
		printf("state: %s \n", getStateName(currProcess) );
		if(init->proc_message != NULL){
			printf("contains message: %s \n", init->proc_message);
		}
	}
	else if(PID == init->PID){
		return;
	}
	else{
		printf("The process with this PID was not found \n");
	}

}

void print(List *pList){

	if(List_count(pList) > 0){

		printf("size: %d \n", pList->size); 
		PCB*  node = List_first(pList);

		while (node != NULL){
			printf("%d ", node->PID);
			node = List_next(pList);
		}
		printf(" \n");
	}
	else{
		printf("This list is empty \n");
	}
}

void TotalInfo(){

	printf("Current process is of ID: %d \n", currProcess->PID);
	printf("Total number of processes in all queues: %d \n", processCount());
	printf("                 Printing contents of all queues: \n");
	printf("--Ready Queue of High Priority: \n");
	print(readyHigh);
	printf("--Ready Queue of Normal Priority: \n");
	print(readyNorm);
	printf("--Ready Queue of Low Priority: \n");
	print(readyLow);
	printf("--Blocked sender queue: \n");
	print(waitSend);
	printf("--Blocked receiver queue: \n");
	print(waitReceive);
	if(semaphoreList[0].list){
		printf("--Blocked semaphore queue of ID 0 and value %d\n", semaphoreList[0].value);
		print(semaphoreList[0].list);
	}
	if(semaphoreList[1].list){
		printf("--Blocked semaphore queue of ID 1 and value %d\n", semaphoreList[1].value);
		print(semaphoreList[1].list);
	}
	if(semaphoreList[2].list){
		printf("Blocked semaphore queue of ID 2 and value %d\n", semaphoreList[2].value);
		print(semaphoreList[2].list);
	}
	if(semaphoreList[3].list){
		printf("--Blocked semaphore queue of ID 3 and value %d\n", semaphoreList[3].value);
		print(semaphoreList[3].list);
	}
	if(semaphoreList[4].list){
		printf("--Blocked semaphore queue of ID 4 and value %d\n", semaphoreList[4].value);
		print(semaphoreList[4].list);
	}

}

int addToReadyQ(PCB* process){
	
	int result;
	if (process->priority == 0){
		result = List_prepend(readyHigh, process);
		process->assignedList = readyHigh;
	}
	else if(process->priority == 1){
		result = List_prepend(readyNorm, process);
		process->assignedList = readyNorm;
	}
	else if(process->priority == 2){
		result = List_prepend(readyLow, process);
		process->assignedList = readyLow;
	}
	
	process->state = ready;
	return result;
}


void startNewProcess(){
	
	PCB* newProcess = NULL;

	if(List_count(readyHigh) > 0){
		newProcess = List_last(readyHigh);
		List_trim(readyHigh);
	}
	else if(List_count(readyNorm) > 0){
		newProcess = List_last(readyNorm);
		List_trim(readyNorm);
	}
	else if(List_count(readyLow) > 0){	
		newProcess = List_last(readyLow);
		List_trim(readyLow);
	}

	else{
		printf("No ready processes available, switching to INIT process\n");
		newProcess = init;
	}
	
	newProcess->state = running;
	newProcess->assignedList = NULL;
	currProcess = newProcess;

}

PCB* searchAllByPID(int PID){

	if(PID == init->PID){
		printf("This PID corresponds to the INIT process \n");
		return NULL;
	}

	PCB* process = NULL;

	if(process == NULL){
		process = searchByPID(PID, readyHigh);
	}
	if(process == NULL){	
		process = searchByPID(PID, readyNorm);
	}
	if(process == NULL){
		process = searchByPID(PID, readyLow);
	}
	if(process == NULL){
		process = searchByPID(PID, waitSend);
	}
	if(process == NULL){
		process = searchByPID(PID, waitReceive);
	}
	if(process == NULL){
		if(semaphoreList[0].list){
			process = searchByPID(PID,semaphoreList[0].list);
		}
	}
	if(process == NULL){
		if(semaphoreList[1].list){
			process = searchByPID(PID,semaphoreList[1].list);
		}
	}
	if(process == NULL){
		if(semaphoreList[2].list){
			process = searchByPID(PID,semaphoreList[2].list);
		}
	}
	if(process == NULL){
		if(semaphoreList[3].list){
			process = searchByPID(PID,semaphoreList[3].list);
		}
	}
	if(process == NULL){
		if(semaphoreList[4].list){
			process = searchByPID(PID,semaphoreList[4].list);
		}
	}
	

	return process;
}

PCB* searchByReplyerPID(int PID, List* list){
	List_first(list);
	return List_search(list, equalReplyerPID, &PID);
}

bool equalReplyerPID(void* curr, void* pComparisonArg){

	PCB* process = (PCB*)curr;
    if ( process->replyerPID == *(int *)pComparisonArg){
        return true;
    }else{
        return false;
    }
}

PCB* searchByPID(int PID, List* list){

	List_first(list);
	return List_search(list, equalPID, &PID);

}

bool equalPID(void* curr, void* pComparisonArg){

    PCB* process = (PCB*)curr;
    if ( process->PID == *(int *)pComparisonArg){
        return true;
    }else{
        return false;
    }
}

int getPID(){

	usedPIDs++;
	return usedPIDs;

}

void freeList(void* curr){
	if(curr){
		free(curr);
	}
}

void End(){

	free(init);
	List_free(readyLow, freeList);
	List_free(readyNorm, freeList);
	List_free(waitReceive, freeList);
	List_free(waitReceive, freeList);
	if(semaphoreList[0].list){
		List_free(semaphoreList[0].list, freeList);
	}
	if(semaphoreList[1].list){
		List_free(semaphoreList[1].list, freeList);
	}
	if(semaphoreList[2].list){
		List_free(semaphoreList[2].list, freeList);
	}
	if(semaphoreList[3].list){
		List_free(semaphoreList[3].list, freeList);
	}
	if(semaphoreList[4].list){
		List_free(semaphoreList[4].list, freeList);
	}
	exit(1);

}

void Menu(){

	Initialise();
	char input[1];
	char param1[2];
	char param2[40];
	char param3[2];
	printMenu();
	 
	while(true){

		printf("\n");
		printf("Enter: ");
		scanf(" %s", input);

		if(input){
			
			if(strcmp("C",input) == 0 || strcmp("c",input) == 0){

				printf("Priority(0 = high, 1 = norm, 2 = low): ");
				scanf("%s", param1);
				
				if(param1){
					if(isdigit(param1[0])){
						Create(atoi(param1));
					}
					else{
						printf("Error! Please enter an integer \n");
					}
				}
			}
			else if(strcmp("F",input) == 0|| strcmp("f",input) == 0){
				Fork();
			}
			else if(strcmp("K",input) == 0|| strcmp("k",input) == 0){

				printf("PID: ");
				scanf("%s", param1);
				
				if(param1){
					if(isdigit(param1[0])){
						Kill(atoi(param1));
					}
					else{
						printf("Error! Please enter an integer \n");
					}
				}
			}
			else if(strcmp("E",input) == 0|| strcmp("e",input) == 0){
				Exit();
			}
			else if(strcmp("Q",input) == 0 || strcmp("q",input) == 0) {
				Quantum();
			}
			else if(strcmp("S",input) == 0 || strcmp("s",input) == 0){

				printf("PID: ");
				scanf("%s", param1); 
				if(param1){
					if(isdigit(param1[0])){
						printf("message: ");
						scanf("%s", param2);
						if(param2){
								Send(atoi(param1), param2);
						}
					}
					else{
						printf("Error! Please enter an integer \n");
					}
				}
					
			}
			else if(strcmp("R",input) == 0 || strcmp("r",input) == 0){
				Receive();
			}
			else if(strcmp("Y",input) == 0 || strcmp("y",input) == 0){

				printf("PID: ");
				scanf("%s", param1);
				
				if(param1){ 
					if(isdigit(param1[0])){
						printf("message: ");
						scanf("%s", param2);
						if(param2){
							Reply(atoi(param1), param2);
						}
					}
					else{
						printf("Error! Please enter an integer \n");	
					}
				}
				
			}
			else if(strcmp("N",input) == 0 || strcmp("n",input) == 0){

				printf("ID: ");
				scanf("%s", param1); 
				if(param1){
					if(isdigit(param1[0])){
						printf("initial value: ");
						scanf("%s", param3);
						if(param3){
							if(isdigit(param3[0])){
								NewSemaphore(atoi(param1), atoi(param3));
							}
							else{
								printf("Error! Please enter an integer \n");	
							}
						}
					}
					else{
						printf("Error! Please enter an integer \n");	
					}
				}
				
			}
			else if(strcmp("P",input) == 0 || strcmp("p",input) == 0){

				printf("ID: ");
				scanf("%s", param1); 

				if(param1){
					if(isdigit(param1[0])){
						SemaphoreP(atoi(param1));
					}
					else{
						printf("Error! Please enter an integer \n");	
					}
				}
					
			}
			else if(strcmp("V",input) == 0 || strcmp("v",input) == 0){

				printf("ID: ");
				scanf("%s", param1); 
				if(param1){
					if(isdigit(param1[0])){
						SemaphoreV(atoi(param1));
					}
					else{
						printf("Error! Please enter an integer \n");	
					}
				}
			}
			else if(strcmp("I",input) == 0 || strcmp("i",input) == 0){

				printf("PID: ");
				scanf("%s", param1); 
				if(param1){
					if(isdigit(param1[0])){
						ProcInfo(atoi(param1));
					}
					else{
						printf("Error! Please enter an integer \n");	
					}
				}
			}
			else if(strcmp("T",input) == 0 || strcmp("t",input) == 0){
				TotalInfo();
			}
			else{
				printf("Please enter a valid character \n");
			}
		}
	}	

}

void printMenu(){

	printf("\n");
	printf("COMMANDS: \n");
	printf("CREATE: C   -parameters: int priority (0 = high, 1 = norm, 2 = low) \n");
	printf("FORK: F  -parameters: NONE \n");
	printf("KILL: K  -parameters: int PID \n");
	printf("EXIT: E  -parameters: NONE  \n");
	printf("QUANTUM: Q  -parameters: NONE \n");
	printf("SEND: S  -parameters: int PID, char* msg \n");
	printf("RECEIVE: R  -parameters: NONE  \n");
	printf("REPLY: Y  -parameters: int PID \n");
	printf("NEW SEMAPHORE: N  -parameters: int semaphore, int value  \n");
	printf("SEMAPHORE P: P  -parameters: int semaphore \n");
	printf("SEMAPHORE V: V  -parameters: int semaphore \n");
	printf("PROCINFO: I  -parameters: int PID \n");
	printf("TOTALINFO: T -parameters: NONE \n");
	printf("\n");
}

int main(){

	Menu();
	return 0;
}
