#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>

#define MAX_File 55

void* thread_func(void *arg);
int def_num(int num);


/* normally i initialized all of them in the main but becsause of the scope error  by semaphore i made them gloabl*/
int             semophor_cnt; /* maximum number of threads that can work in parallel */
pthread_t       tid; /* thread identifier */
pthread_attr_t  attr; /* thread attribute i do not know what it does but it is essential */
pthread_t       tids[MAX_File]; /* to keep the thread ids for joining them later */
DIR*            dir; /* to directory traverse */
struct dirent*  entry;
sem_t           sem; /* we need to create semaphore to control access in number */


int main(int argc, char *argv[]) {

    if (argc != 3) {
        fprintf(stderr,"Argumant counts is not exactly the same as required. Please execute with 2 argumants");
        return -1;
    }

    /* we need to change the agrument for semaphore count to integer to be able to use.*/
    semophor_cnt = atoi(argv[2]);
    dir = opendir(argv[1]);
    
    if(dir == NULL) {
        fprintf(stderr,"There is no such directory");
        return -1;
    }

    int cnt = 0;

    /* initialize semaphore with maximum number of threads that should be active by given argument */
    sem_init(&sem, 0, semophor_cnt);

    while ((entry = readdir(dir)) != NULL) {

        if ( (strcmp(entry->d_name, ".") == 0) || (strcmp(entry->d_name, "..") == 0) ) {
            continue;
        }

        /* wait for semaphore if we do not have any available. */
        sem_wait(&sem);

        /* i tried to do with static but unfortunetly it creates racing condition  */
        char* file_name = malloc(( 20 + strlen(entry->d_name) ) * sizeof(char));/* i assume file max name as 19 */
        /* i also need to send the id so i do something like this */ // if out theacer wants us to send tid than change this.
        file_name[0] = ++cnt;
        strcpy(file_name + 1, argv[1]);
        strcat(file_name, "/");
        strcat(file_name, entry->d_name);
        
        /* set thread attributes as default */
        pthread_attr_init(&attr);
        /* create thread with thread_func function */
        pthread_create(&tid, &attr, thread_func, (void*) file_name);
        tids[cnt] = tid;

    }
    
    for (size_t i = 1; i <= cnt; i++) {
        /* wait for thread to complete */
        pthread_join(tids[i], NULL);
    }

    return 0;
}

void* thread_func(void *arg) {
    
    char* ptr_temp = (char*) arg;
    /* i assigned first character as thread number. */
    int id = (((char*) ptr_temp++)[0]); 
    char* file_path = (char*) ptr_temp; /* rest of the string */
    char* file_name = strrchr(file_path, '/') + 1; /* to print we need only file name */

    FILE* file;
    file = fopen(file_path, "r");

    int temp_int;
    int result = 0;

    while(fscanf(file, "%d", &temp_int) != EOF) {
        
        if(def_num(temp_int))
            result++;

    }

    printf("Thread %d has found %d deficient numbers in %s\n", id, result, file_name);
    
    fclose(file);


    free((char*) arg);
    sem_post(&sem);

    return 0;
}

int def_num(int num){
    
    int sum = 0;

    for (size_t i = 1; i < num; i++) {
        
        /* if it is proper divider than add to the sum */
        sum += (num % i == 0)? i : 0;

    }

    return (sum < num) ? 1 : 0;
}