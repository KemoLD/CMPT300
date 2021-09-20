#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <stdbool.h>
#include "UnixLs.h"

static bool initialised = false;
static bool i_Param = false;
static bool l_Param = false;
static bool R_Param = false;

static int dirIndex;
static bool directories = false;

static void printTimeStamp(struct stat stat);
static void printGroup(struct stat stat);
static void printOwner(struct stat stat);
static void printHardLink(struct stat stat);
static void printPermissions(struct stat stat);
static void end();
static bool isDirectory(char* path);
static bool isDirectoryOrSymbolicLinkDirectory(char* path);
static bool isValid(char* path);
static bool isFile(char* path);
static void freeList(void* item);


int main(int argc, char* argv[]){

    setParams(argc, argv);
    end();
    return 0;

   
}

void setParams(int count, char* input[]){

    if(count > 1){

        dirIndex = 0;

        for(int i = 1; i < count; i++){
              
            if(input[i][0] == '-'){

                dirIndex++;
                for(int j = 1; j < strlen(input[i]); j++){

                    if(input[i][j] == 'i'){
                        i_Param = true;
                    } 
                    else if(input[i][j] == 'R'){
                        R_Param = true;
                    }
                    else if(input[i][j] == 'l'){
                        l_Param = true;
                    }  
                    else{
                        printf("Error! Invalid parameters \n");
                        end();   
                    }
                }
            }
        } 

        if(dirIndex+1 == count){
            UnixManager(".");
        }
        else{
            for(int y = dirIndex+1; y < count; y++){
	       if(isDirectory(input[y])){directories = true;}
	       else if(isDirectoryOrSymbolicLinkDirectory(input[y]) & l_Param == false){printf("%s:\n", input[y]);}	
               UnixManager(input[y]);
            }
            
        }


    }
    else{
      UnixManager(".");
    }
}



void UnixManager(char* path){

    if(isValid(path)){

	if(l_Param){
		if(!isDirectory(path)){ 
		    printFile(NULL, path);
		}
		else{
		    printDirectory(path);
		}
	}
	else{
		if(!isDirectoryOrSymbolicLinkDirectory(path)){ 
		    printFile(NULL, path);
		}
		else{
		    printDirectory(path);
		}
	}
    }
    else{
	printf("Error, could not find directory or file \n");
    }	
    
}

void printDirectory(char* path){

    struct dirent* dirent;
    DIR* dir = opendir(path);

    if(dir == NULL){
        printf("Error, could not open directory \n");
        closedir(dir);
        end();
    }

    if(R_Param == true || directories == true){
        printf("%s: \n", path);
    }

    dirent = readdir(dir);
    while(dirent != NULL){
        if(dirent->d_name[0] != '.'){
            char *newpath = malloc(sizeof(path) + sizeof(dirent->d_name) + 1);
            if (path[strlen(path) - 1] == '/'){
                sprintf(newpath, "%s%s", path, dirent->d_name);
            }
            else{
                sprintf(newpath, "%s/%s", path, dirent->d_name);
            }

           
            printFile(dirent->d_name, newpath);
            free(newpath);
        }
	dirent = readdir(dir);
        
    }

    printf("\n");
    closedir(dir);

    if(R_Param == true){

	dir = opendir(path);

    	if(dir == NULL){
        	printf("Error, could not open directory \n");
        	closedir(dir);
        	return;
    	}

        dirent = readdir(dir);
        while(dirent != NULL){
            if(dirent->d_name[0] != '.'){
                char *newpath = malloc(sizeof(path) + sizeof(dirent->d_name) + 1);
                if (path[strlen(path) - 1] == '/'){
                    sprintf(newpath, "%s%s", path, dirent->d_name);
                }
                else{
                    sprintf(newpath, "%s/%s", path, dirent->d_name);
                }
			
		if(isDirectory(newpath)){
                    printDirectory(newpath);
		}
                free(newpath);
            }
	    dirent = readdir(dir);
        }
        closedir(dir); 
    }

    
}

void printFile(char * file, char* path){

    struct stat stat;
    if(lstat(path, &stat) == -1){
        printf("Error! couldnt read file \n");
	end();
    }

    if(i_Param == true){
        printf("%lu  ", stat.st_ino);
    }
    if(l_Param){

        printPermissions(stat);
        printHardLink(stat);
        printOwner(stat);
        printGroup(stat);
        printf("%lu  ",stat.st_size); //size
        printTimeStamp(stat);
    }

    if(file){
         printf("%s", file);
    }
    else{
        printf("%s", path);
    }

    if(S_ISLNK(stat.st_mode) && l_Param){
        char* newPath = malloc(stat.st_size + 1);
        int status = readlink(path, newPath, stat.st_size + 1);
        
        if (status == -1)
        {
            printf("Error, cannot read link \n");
            end();
        }

        newPath[stat.st_size] = '\0';
        printf(" -> %s", newPath);
        free(newPath);        
    }

    printf("\n");

}

void printTimeStamp(struct stat stat){
    struct tm *time;
    char timeArray[50];
    time = localtime(&stat.st_mtime);
    strftime(timeArray, sizeof(timeArray), "%b %e %Y %H:%M", time);
    printf("%s  ", timeArray);

}

void printGroup(struct stat stat){
    struct group *group;
    group = getgrgid(stat.st_gid);
    printf("%s  ", group->gr_name);
}

void printOwner(struct stat stat){
    struct passwd *owner;
    owner = getpwuid(stat.st_uid);
    printf("%s  ", owner->pw_name);
}

void printHardLink(struct stat stat){
    printf("%lu  ", stat.st_nlink);
}

void printPermissions(struct stat stat){

    if (S_ISDIR(stat.st_mode)) {
        printf("d");
    }
    else if (S_ISLNK(stat.st_mode)) {
        printf("l");
    }
    else {
        printf("-");
    }

    printf( (stat.st_mode & S_IRUSR) ? "r" : "-");
    printf( (stat.st_mode & S_IWUSR) ? "w" : "-");
    printf( (stat.st_mode & S_IXUSR) ? "x" : "-");
    printf( (stat.st_mode & S_IRGRP) ? "r" : "-");
    printf( (stat.st_mode & S_IWGRP) ? "w" : "-");
    printf( (stat.st_mode & S_IXGRP) ? "x" : "-");
    printf( (stat.st_mode & S_IROTH) ? "r" : "-");
    printf( (stat.st_mode & S_IWOTH) ? "w" : "-");
    printf( (stat.st_mode & S_IXOTH) ? "x" : "-");

    printf("  ");

}



void end(){
    exit(1);
}

bool isDirectory(char* path){
    struct stat Stat;
    lstat(path, &Stat);
    return S_ISDIR(Stat.st_mode);
}

bool isDirectoryOrSymbolicLinkDirectory(char* path){
    char* newPath = malloc(sizeof(path) + 1);
    struct stat Stat;

    if (path[strlen(path) ] != '/' & path[0] != '.'){
        sprintf(newPath, "%s/", path);
    }
    else{
        newPath = path;
    }
	
    lstat(newPath, &Stat);
    return S_ISDIR(Stat.st_mode);
}

bool isValid(char* path){

    struct stat Stat;
    if (lstat(path, &Stat) != -1){
	return true;
    }
    else{
        return false;
    }
}


