#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

// makros
#define TC_LAST_3DIG 111        // The last three digit of randım id number

// function prototypea
int abundant_check(int num);    // booelan 1 = true (abundant), 0 = false
int prime_check(int num);       // booelan 1 = true (prime), 0 = false
void write_N_num_file(FILE* file, int N); // writes N number random int to the file
int result_sum(FILE* file, int (*function)(int)); // reads from the file applies the PN and AN functions and returns the sum

// main
int main() {
    
    FILE*   file1;              // output file for child 1
    FILE*   file2;              // output file for child 2
    int     N;                  // input value for N
    pid_t   pid_chld1;          // process id for child 1
    pid_t   pid_chld2;          // process id for child 2
    int     fd_ch1_Pwrites[2];  // pipe for child 1 in which parent writes child reads
    int     fd_ch2_Pwrites[2];  // pipe for child 1 in which parent reads child writes
    int     fd_ch1_Pread[2];    // pipe for child 2 in which parent writes child reads
    int     fd_ch2_Pread[2];    // pipe for child 2 in which parent reads child writes
    /* we get the N as input */
    scanf("%d", &N);
    
    /* creating the pipes for communication */
    pipe(fd_ch1_Pwrites);
    pipe(fd_ch2_Pwrites);
    pipe(fd_ch1_Pread);
    pipe(fd_ch2_Pread);

    /* we are creating the files which will hold Primes and Abundants */
    file1 = fopen("File1","w");
    file2 = fopen("File2","w");

    /* we writes N random numbers to the files */
    write_N_num_file(file1, N);
    write_N_num_file(file2, N);
    
    /* we are creating the first child */
    pid_chld1 = fork();
    
    if (pid_chld1 > 0) {
        /* creating the second child here */
        pid_chld2 = fork();

        if (pid_chld2 > 0) {
            /* parent area is here */

            /* closing the read channels that we will not use because parent will write in this channel */
            close(fd_ch1_Pwrites[0]);
            close(fd_ch2_Pwrites[0]);
            /* closing the write channels that we will now use because parent will read in this channel */
            close(fd_ch1_Pread[1]);
            close(fd_ch2_Pread[1]);

            /* Primary number and Abundant number */
            int PN; int AN;
            /* communication part */
            {
                /* sending the N to the childeren  */
                /*  in this part I will use a trick 
                    instead of sending the info like string and changing type
                    I will send int directly after all it is (void*) with the int size */
                write(fd_ch1_Pwrites[1], &N, sizeof(int)); 
                write(fd_ch2_Pwrites[1], &N, sizeof(int));

                /* reading the results from the childeren */
                read(fd_ch1_Pread[0], &PN, sizeof(int));
                read(fd_ch2_Pread[0], &AN, sizeof(int));
                
                /* sending the results to the other child */
                write(fd_ch1_Pwrites[1], &AN, sizeof(int));
                write(fd_ch2_Pwrites[1], &PN, sizeof(int)); 
            }

            /* closing the all pipes */ 
            close(fd_ch1_Pwrites[1]);
            close(fd_ch2_Pwrites[1]);
            close(fd_ch2_Pread[0]);
            close(fd_ch1_Pread[0]);

            /* now we will wait for the children to finish */
            wait(NULL);
            wait(NULL);

            /* printing the results in parent*/
            printf("The number of positive integers in each file: %d\n",N);
            printf("The number of prime numbers in File1: %d\n",PN);
            printf("The number of abundant numbers in File2: %d\n",AN);
        }
        else if (pid_chld2 == 0) {
            /* second child area is here */

            close(fd_ch2_Pread[0]);     /* parent reads with fd[0] then as a child we close fd[0] */
            close(fd_ch2_Pwrites[1]);   /* parent writes with fd[1] then as a child we close fd[1] */

            /* closing all pipes related with child2 here */
            {
                close(fd_ch1_Pwrites[0]);
                close(fd_ch1_Pwrites[1]);
                close(fd_ch1_Pread[0]);
                close(fd_ch1_Pread[1]);
            }

            
            /* Abundant number and Primary number we will get these in the next local */
            int AN_of_this; int PN_of_that;
            /* communication part*/
            {
                file2 = fopen("File2","r");
                /* reading the N from the parent */
                read(fd_ch2_Pwrites[0], &N, sizeof(int)); // the real N is in parents this is child fork so does not matter for N variable
                
                /* writing the result to the parent */
                AN_of_this = result_sum(file2, abundant_check);
                write(fd_ch2_Pread[1], &AN_of_this, sizeof(int));

                /* getting the other childs PN from parent */
                read(fd_ch2_Pwrites[0], &PN_of_that, sizeof(int));
            }

            /* closing the remaining pipes */
            close(fd_ch2_Pread[1]);
            close(fd_ch2_Pwrites[0]);

            /* winner child??? */
            int winner;

            if (AN_of_this > PN_of_that)
            {
                winner = 3;
            }
            else
            {
                winner = 2;
            }

            printf("I am Child process P3: The winner is child process P%d\n", winner);
            
            exit(0);
        }
        
    }
    else if (pid_chld1 == 0){
        /* first child area is here */
        
        close(fd_ch1_Pread[0]);     /* parent reads with fd[0] then as a child we close fd[0] */
        close(fd_ch1_Pwrites[1]);   /* parent writes with fd[1] then as a child we close fd[1] */

        /* closing all pipes related with child2 here */
        {
            close(fd_ch2_Pwrites[0]);
            close(fd_ch2_Pwrites[1]);
            close(fd_ch2_Pread[0]);
            close(fd_ch2_Pread[1]);
        }

        
        /* Abundant number and Primary number we will get these in the next local */
        int PN_of_this; int AN_of_that;
        /* communication part*/
        {
            file1 = fopen("File1","r");
            /* reading the N from the parent */
            read(fd_ch1_Pwrites[0], &N, sizeof(int));

            /* writing result to the parent */
            PN_of_this = result_sum(file1, prime_check);
            write(fd_ch1_Pread[1], &PN_of_this, sizeof(int));

            /* getting the result of the other from the parent */
            read(fd_ch1_Pwrites[0], &AN_of_that, sizeof(int));

        }

        /* closing the remaining pipes */
        close(fd_ch1_Pread[1]);
        close(fd_ch1_Pwrites[0]);

        /* winner child??? */
        int winner;

        if (PN_of_this > AN_of_that)
        {
            winner = 2;
        }
        else 
        {
            winner = 3;
        }

        printf("I am Child process P2: The winner is child process P%d\n", winner);

        exit(0);
    }
    else
        fprintf(stderr," fork could not happend pls try again later.\n");
    
    return 0;
}

// functions
int abundant_check(int num) {
    /* if the summation of the dividers are bigger then that number its abundant*/
    int sum = 0;

    /* we search for the divider and summ them all */
    for (size_t i = 1; i < num; i++) {
        
        /* dividers */
        if (num % i == 0) {
            sum += i ;
        }
        
    }

    /* if summation is bigger return true if not false */
    return (sum > num);
}

int prime_check(int num) {
    /* from discreate math course we know that if N does not have any prime until (N)^(1/2)*/
    /* then it is prime and for this purpose I used a trick from https://youtube.com/shorts/KcF9ma8i33w?si=HesHpmDfrAvTR0Jx */

    if ((num == 2) || (num == 3))
        /* 2 and 3 is prime */
        return 1; 
    else if (num < 2 || (num % 2 == 0) || (num % 3 == 0))
        /* 0 and 1 is not prime and if can be divided 2 or 3 also */
        return 0;
    else
        /* 2k and 3k's are handled in the else if part (+2, +3, +4, these are divided into 2 and 4)  */
        /* now we will search for the other than 6k+1 and 6k+5 */
        for (size_t i = 5; i * i <= num; i += 6) {

            if (num % i == 0 || num % (i + 2) == 0)
                return 0;

        }
    
    return 1;
}

void write_N_num_file(FILE* file, int N) {
    /* we get file pointer and write N number rand integer */
    for (size_t i = 1; i <= N; i++) {
        /* we create number from that is rand in 0 to 100 + the last three digit of my id */
        int num = rand() % (TC_LAST_3DIG + 1);
        
        if (i != N)
            fprintf(file, "%d ", num);  /* space exist */
        else
            fprintf(file, "%d", num);   /* space does notexist */
    }

    fclose(file);
}

int result_sum(FILE* file, int (*function)(int)) {
    /* we will increase the count for the used function (PN or AN) and will return*/
    int count = 0;
    int temp;

    while (fscanf(file,"%d",&temp) != EOF) {
        
        /* both prime and abundant returns 1 if true so when function pointer returns 1 increase the count */
        if(function(temp))
            count++;

    }

    fclose(file);
    
    return count;
}