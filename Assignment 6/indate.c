////////////////////////////////////////////////////////////////////////////////
// Main File:  indate.c, sendsig.c, division.c
// This File:  indate.c
// Other Files:  sendsig.c, division.c
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

#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>

int sigusr1Count = 0;
// global variable that keeps track of the number of times that the 
// program received the SIGUSR1 signal

/* handles a SIGALRM signal, which is sent by the program
 * to itself using alarm(). Then it prints current process's pid
 * and the current time.
 * 
 * parameter: N/A
 * return: void
 */
void sigalrm_handler(){
    pid_t processPid;
    // the current process's pid
    processPid = getpid();
    time_t currentTime;
    currentTime = time(NULL);
    // get the current time and convert it into characters
    if (currentTime == -1){
        printf("Failed to return the current time.\n");
        exit(0);
        // check if the time() gets the current time successfully
    }
    char *stringTime = ctime(&currentTime);
    if (stringTime == NULL){
        printf("Failed to convert the current time into a string.\n");
        exit(0);
        // check if ctime() converts current time successfully
    }
    printf("PID: ");
    printf("%d", processPid);
    printf(" | Current Time: ");
    printf("%s", stringTime);
    alarm(3);
    // sends a SIGALRM signal to be caught by this function itself
    // to keep printing the process's pid and current time every 3 seconds
}

/* handles a SIGUSR1 signal, prints a message when the 
 * signal is caught. It also increments the count of 
 * SIGUSR1 signal received
 *
 * parameter: N/A
 * return: void
 */
void sigusr1_handler(){
    sigusr1Count++;
    printf("SIGUSR1 caught!\n");
    alarm(3);
}

/* handels a SIGINT signal, which is sent when Control + c
 * is pressed. Then it prints the number of times that the
 * program received the SIGUSR1 signal. Then it terminates 
 * the program
 *
 * parameter: N/A
 * return: void
 */
void sigint_handler(){
    printf("\nSIGINT received.\n");
    printf("SIGUSR1 was received ");
    printf("%d", sigusr1Count);
    printf(" times. Exiting now.\n");
    exit(0);
    // if SIGINT was received, print the count and terminate
}

/* main function that drives the sending of signals. A SIGALRM
 * signal is sent to the program itself every 3 seconds. This pattern 
 * will continue endlessly if no signal interrupts it. This function
 * also registers handler functions to catch signals including
 * SIGALRM, SIGUSR1, and SIGINT.
 *
 * parameter: N/A
 * return: 0 on success (no error), non-zero value on error
 */
int main(){
    printf("Pid and time will be printed every 3 seconds.\n");
    printf("Enter ^C to end the program.\n");
    
    struct sigaction sigalrmAction;
    struct sigaction sigusr1Action;
    struct sigaction sigintAction;
    // structs to specify actions to be taken on designated signals
    memset(&sigalrmAction, 0, sizeof(sigalrmAction));
    memset(&sigusr1Action, 0, sizeof(sigusr1Action));
    memset(&sigintAction, 0, sizeof(sigintAction));
    // zero out the initialized sigaction struct
    
    sigalrmAction.sa_handler = sigalrm_handler;
    sigusr1Action.sa_handler = sigusr1_handler;
    sigintAction.sa_handler = sigint_handler;
    // sets the handler functions
    
    int sigalrmActionResult = sigaction(SIGALRM, &sigalrmAction, NULL);
    int sigusr1ActionResult = sigaction(SIGUSR1, &sigusr1Action, NULL);
    int sigintActionResult = sigaction(SIGINT, &sigintAction, NULL);
    // registers the action structs with designated signals,
    // save the return value of sigaction() functions
    
    if (sigalrmActionResult == -1 || sigusr1ActionResult == -1
        || sigintActionResult == -1){
        printf("Failed to register handler function with signals.\n");
        exit(0);
        // check if sigAction() successfully registered handler functions
    }
    
    alarm(3);
    // start the infinite loop of sending a SIGALRM every 3 seconds
    
    while (1){
    }
    // an infinite loop that keeps the program running infinitely until 
    // a SIGINT signal interrupts it
    
    return 0;
}
