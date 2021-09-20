#include <stdio.h>
#include <stdlib.h>
#include "tcp.h"
#include "list.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <ctype.h>

#define MAX_LEN 1024

static int portnumber;
static char* remoteMachineName;
static char* remotePortNumber;

static char* dispMessage;
static char* sendM;

static int network_socket;
static struct addrinfo sinRemote;
static struct sockaddr_in address;
static struct addrinfo* remoteAddress;

static pthread_t keyboardThread;
static pthread_t screenThread;
static pthread_t inputThread;
static pthread_t outputThread;

static pthread_mutex_t input = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t output = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t inputCond = PTHREAD_COND_INITIALIZER;
static pthread_cond_t outputCond = PTHREAD_COND_INITIALIZER;

static List* listIn;
static List* listOut;

static void createThreads();
static void destroyThreads();
static void end();
static bool emptyString(char message[]);

int main(int argc, char** argv){

	if(argc != 4 ){
		printf("please enter s-talk [port number] [remote machine name] [remote port number]");
		exit(1);
	}
	
	portnumber = atoi(argv[1]);
	remoteMachineName = argv[2];
	remotePortNumber = argv[3];
	
	printf("************  s-talk *************\n");
	printf("connected on port: %d \n", portnumber);
	printf("**********************************\n");
	printf("\n");

	memset(&address, 0, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_port = htons(portnumber);
	address.sin_addr.s_addr = INADDR_ANY;
	
	memset(&sinRemote, 0, sizeof(sinRemote));
	sinRemote.ai_family = AF_INET;
	sinRemote.ai_socktype = SOCK_DGRAM;
	sinRemote.ai_protocol = IPPROTO_UDP;
	if( getaddrinfo(remoteMachineName, remotePortNumber, &sinRemote, &remoteAddress) != 0){
		printf("error getting remote host address info\n");
		exit(1);
	}	
	
	network_socket = socket(PF_INET, SOCK_DGRAM, 0);
	if( network_socket == -1){
		printf("error creating socket \n");
		exit(1);
	}
	
	if ( bind(network_socket, (struct sockaddr *)&address, sizeof(address)) == -1){
		printf("error binding socket \n");
		close(network_socket);
		exit(1);
	}
	
	//Create input and output list ADT's
	listIn = List_create();
	listOut = List_create();
	
	createThreads();
		
	destroyThreads();
	freeaddrinfo(remoteAddress);
	close(network_socket);
	return 0;
}

void createThreads(){

	//create threads
	if( pthread_create(&keyboardThread, NULL, keyboard,NULL) != 0){
		printf("error creating keyboard thread \n");
		exit(1);
	}
	
	if( pthread_create(&screenThread, NULL, display ,NULL) != 0){
		printf("error creating screen thread \n");
		exit(1);
	}
	
	if( pthread_create(&inputThread, NULL, receive,NULL) != 0){
		printf("error creating input thread \n");
		exit(1);
	}
	
	if( pthread_create(&outputThread, NULL,sendMessage ,NULL) != 0){
		printf("error creating output thread \n");
		exit(1);
	}
	
	//join threads
	pthread_join(keyboardThread, NULL);
	pthread_join(screenThread, NULL);
	pthread_join(inputThread, NULL);
	pthread_join(outputThread, NULL);
}

void destroyThreads(){

	pthread_mutex_destroy(&input);
	pthread_mutex_destroy(&output);
	pthread_cond_destroy(&inputCond);
	pthread_cond_destroy(&outputCond);
}

bool emptyString(char message[]){

	if( strcmp(message, "\n") == 0 ){
		return true;
	}
	
	int x = 0;
	
	while(x < strlen(message)){
		if(isspace(message[x]) == 0){
			return false;
		}
		x++;
	}
	
	return true;
} 

void* keyboard(){

	char message[MAX_LEN];

	while(1){
		
		fgets(message, MAX_LEN, stdin);
		
		if( emptyString(message) == false ){
			pthread_mutex_lock(&output);
			
			if(List_count(listOut) == LIST_MAX_NUM_NODES){
				printf("incoming list is full, cannot add message to list\n");
				}
				
			else if(List_append(listOut,message) == -1){
				printf("error appending message to outgoing list\n");
			}
			
			pthread_cond_signal(&outputCond);
			pthread_mutex_unlock(&output);
			
		}
		

	}
	return NULL;
}

void* display(){

	while(1){
			
		pthread_mutex_lock(&input);
		
		if(List_count(listIn) != 0){				
			List_first(listIn);
			dispMessage =  List_remove(listIn);
			pthread_cond_signal(&inputCond);
		}	
		else{
			pthread_cond_wait(&inputCond,&input);
		}
		
		pthread_mutex_unlock(&input);
		
		if(dispMessage != NULL){
		
			printf("%s: %s", remoteMachineName, dispMessage);
				
			if(strcmp(dispMessage,"!\n") == 0){
				printf("--ending program--\n");
				end();
			}
				
			dispMessage = NULL;
		}
		
	}
	
	return NULL;
}


void* receive(){

	unsigned int address_len = sizeof(address);
	char message[MAX_LEN];
	
	while(1){
	
		int bytesRx = recvfrom(network_socket, message, MAX_LEN, 0, (struct sockaddr *) &address,&address_len);
		
		if(bytesRx != -1){
			int terminate = (bytesRx < MAX_LEN) ? bytesRx : MAX_LEN-1 ;
			message[terminate] = 0;
			
			pthread_mutex_lock(&input);
			
			if(List_count(listIn) == LIST_MAX_NUM_NODES){
				printf("incoming list is full, cannot receive messaged\n");
				break;
			}
			
			else if(List_append(listIn, &message) == -1){
				printf("error appending message to incoming list\n");
			}
			
			pthread_cond_signal(&inputCond);
			pthread_mutex_unlock(&input);			
			
		}
		else{
			printf("error receiving message \n");
		}
	}
	return NULL;
	
}

void* sendMessage(){
	
	while(1){
	
		pthread_mutex_lock(&output);
		
		if(List_count(listOut) != 0){
			List_first(listOut);
			sendM = (char*)List_remove(listOut);
		}
		else{
			pthread_cond_wait(&outputCond, &output);
		}
			
		pthread_mutex_unlock(&output);
			
		if(sendM != NULL){	
			int bytesRx = sendto(network_socket, sendM, strlen(sendM),0,remoteAddress->ai_addr, remoteAddress->ai_addrlen);
				
			if(bytesRx == -1){
				printf("error sending message\n");
			}
				
			if(strcmp(sendM, "!\n") == 0){
				printf("--ending program--\n");
				end();
			}

			sendM = NULL;
		}

	}
	
	return NULL;

}

void freeList(void* curr){
    printf("freed List \n");
}

void end(){
	pthread_cancel(screenThread);
	pthread_cancel(keyboardThread);
	pthread_cancel(inputThread);
	pthread_cancel(outputThread);
	List_free(listIn, freeList);
	List_free(listOut, freeList);
}
