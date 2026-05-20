#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

/* function prototypes */
void *reader(void *param);
void *writer(void *param);
int8_t pass_match(int is_pass);

/* struct */
typedef struct {
    /* we have all the needed thread info here */
    int8_t  is_real; // if 0 it is dummy
    int8_t  is_writer; // if it is not 0 it is writer
    int8_t  id;
    int     password; 
    
} THREAD_R_W;


/* global variables */
sem_t           reader_mutex;
pthread_mutex_t rw_mutex;
sem_t           pass_read_mutex; // I could use only the first mutex but it make code faster maybe because only intervence when they are accessing the password.
int             pass_table[10];
int             BUFFER = 0; // for now
int8_t          read_count = 0;

int main() {

    srand(time(NULL));

    /* assigning unique values to pass_table */
    for (size_t i = 0; i < sizeof(pass_table) / sizeof(pass_table[0]); i++) {
        
        pass_table[i] = rand() % 1000000;
        
        for (size_t j = 0; j < i; j++) {
            
            if(pass_table[i] == pass_table[j]) {
                i--;
                break;
            }

        }
        
    }

    sem_init(&reader_mutex, 0, 1);
    pthread_mutex_init(&rw_mutex, NULL);
    sem_init(&pass_read_mutex, 0, 1);

    /* -------------------------------------- */

    /* 10 real and 10 dummy reader writer */
    THREAD_R_W threads[20];
    
    /* when we assign the numbers ourselves */
    int writer_num = 4;
    int reader_num = 5;
    /* when randomly assigned */
    // int writer_num = rand() % 10;
    // reader_num = 10 - writer_num;
    printf("There must be maximum summ of 10 writer/reader!!\n\n");
    printf("Please enter the number of Writer and readers (e.g., 5 4): ");
    
    while (1) {

        scanf("%d %d", &writer_num, &reader_num);
        
        if ((writer_num + reader_num) <= 10 && (writer_num + reader_num) > 0)
            break;
        //

        printf("Please enter in the format as we want (e.g., 5 4): ");
    }
    
    
    /* creating the dummy and real threads in for loops.*/
    for (size_t i = 0; i < 2; i++){
        /* first ofsset is real, the second offset is dummy so i used offset logic for first and second part in the thread */
        int offset = (i == 0)? 0 : (writer_num + reader_num);

        /* assigning readers */
        for (size_t j = 0; j < writer_num; j++) {
            /* id and being writer or not */
            threads[offset + j].id = offset + j;
            threads[offset + j].is_writer = 1;
            
            /* is real or not */
            if (i==0) {
                /* this part if the thread is real */    
                threads[offset + j].is_real = 1;
                threads[offset + j].password = pass_table[j];
            }
            else {
                /* for dummy ones */
                threads[offset + j].is_real = 0;
                threads[offset + j].password = -1 * (rand() % 1000000); // I assume that it will never brought the same password.
            }

        }
        /* assigning readers */
        for (size_t j = writer_num; j < (writer_num + reader_num); j++) {
            /* id and being writer or not */
            threads[offset + j].id = offset + j;
            threads[offset + j].is_writer = 0;
            
            /* is real or not */
            if (i==0) {
                /* this part if the thread is real */    
                threads[offset + j].is_real = 1;
                threads[offset + j].password = pass_table[j];
            }
            else {
                /* for dummy ones */
                threads[offset + j].is_real = 0;
                threads[offset + j].password = rand() % 1000000; // I assume that it will never brought the same password.
            }

        }
        
    }
    
    /* creating and initializing the thread as */

    pthread_t tid[2 * (writer_num + reader_num)]; // 20 thread
    pthread_attr_t attr;

    /* For output purpose */
    printf("\nThread No | Validity     | Role   | Value read/written\n");
    printf("------------------------------------------------------\n");

    /* function pointer for writer or reader */
    void *(*func)(void*);;

    for (size_t i = 0; i < 2 * (writer_num + reader_num); i++) {

        pthread_attr_init(&attr);

        /* i did not want to create to many if else statement so instead use one function pointer */
        func = (threads[i].is_writer) ? writer : reader;
        
        pthread_create(&tid[i], &attr, func, &threads[i]);

    }

    for (size_t i = 0; i < 2 * (writer_num + reader_num); i++)
        pthread_join(tid[i], NULL);
    //

    return 0;
}

/* reader function for reader threads */
void *reader(void *param) {

    THREAD_R_W *thread_info = (THREAD_R_W*) param;
    int8_t is_match;
    int read_BUFFER;
    
    for (size_t i = 0; i < 5; i++) {
        /* reading password */        
        sem_wait(&pass_read_mutex);
            is_match = pass_match(thread_info->password);
        sem_post(&pass_read_mutex);
        
        /* ------ */
        if (is_match) {
            /* for reading purpose */
            sem_wait(&reader_mutex);
            read_count++;


            if (read_count == 1)
                pthread_mutex_lock(&rw_mutex);
            //
            sem_post(&reader_mutex);
            
            /* ------ */
            /* reading performed */
            read_BUFFER = BUFFER;

            printf("Thread No: %d, Real, Reader, Read value: %d\n",thread_info->id, read_BUFFER);   
            /* ------ */

            sem_wait(&reader_mutex);
            read_count--;

            if (read_count == 0) 
                pthread_mutex_unlock(&rw_mutex);
            //

            sem_post(&reader_mutex);
        }
        else 
            printf("Thread No: %d, Dummy, Reader, Access Denied\n", thread_info->id);
            //
        

        sleep(1);
    }
    return NULL;
}

/* writer function for writer threads */
void *writer(void *param) {

    THREAD_R_W *thread_info = (THREAD_R_W*) param;
    int8_t is_match;
    
    for (size_t i = 0; i < 5; i++) {
        /* reading password */
        sem_wait(&pass_read_mutex);
            is_match = pass_match(thread_info->password);
        sem_post(&pass_read_mutex);
        
        /* ----------------------------------- */
        
        if (is_match) {
            /* for writing purpose */
            pthread_mutex_lock(&rw_mutex);
                /* ------ */
                /* writing performed */
                BUFFER = rand() % 10000; // 0-9999 random number
                printf("Thread No: %d, Real, Writer, Written value: %d\n",thread_info->id, BUFFER);
                //
                /* ------ */
            pthread_mutex_unlock(&rw_mutex);           
        }
        else 
            printf("Thread No: %d, Dummy, Writer, Access Denied\n", thread_info->id);


        sleep(1);
    }
    
    return NULL;
}

int8_t pass_match(int is_pass) {

    for (size_t i = 0; i < sizeof(pass_table) / sizeof(pass_table[0]) ; i++) {
        
        if (is_pass == pass_table[i]) 
            return 1;
        //
        
    }
    
    return 0;
}
