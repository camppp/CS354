////////////////////////////////////////////////////////////////////////////////
// Main File:  indate.c, sendsig.c, division.c
// This File:  sendsig.c
// Other Files:  indate.c, division.c
// Semester:         CS 354 Fall 2018
//
// Author:           YUXUAN LIU
// Email:            liu686@wisc.edu
// CS Login:         yuxuan
//
/////////////////////////// OTHER SOURCES OF HELP //////////////////////////////
//                   fully acknowledge and credit all sources of help,
//                   other than Instructors and TAs.
//
// Persons:          N/A
//                   
// Online sources:   N/A                
//                  
//////////////////////////// 80 columns wide ///////////////////////////////////

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>

/* main function that drives the action of sending signals to a 
 * process according to user input. The program can send two types
 * of signal: SIGINT and SIGUSR1
 *
 * parameter: 
 * argc, number of command line arguments
 * argv, pointer to the command line argument char array
 * return: 0 on success (no error), non-zero value on error
 */
int main(int argc, char *argv[]) {
    if (argc != 3){
        printf("Usage: <signal type> <pid>\n");
        exit(0);
    }
    if ((strcmp(argv[1], "-u") != 0) && (strcmp(argv[1], "-i") != 0)){
        printf("Usage: <signal type> <pid>\n");
        exit(0);
    }
    // check for validity of user input
    if (strcmp(argv[1], "-u") == 0){
        int result = kill(atoi(argv[2]), SIGUSR1); 
        // save the return value of kill()
        if (result == -1){
            printf("Failed to send SIGUSR signal to the specified process\n");
            exit(0);
            // check if kill() sent the signal to the process successfully
        }
        // if the first argument is -u, send a SIGUSR1 signal
    } else if (strcmp(argv[1], "-i") == 0){
        int result = kill(atoi(argv[2]), SIGINT);
        if (result == -1){
            printf("Failed to send SIGINT signal to the specified process\n");
            exit(0);
        }
        // if the first argument is -i, send a SIGINT signal
    }
}
