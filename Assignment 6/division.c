////////////////////////////////////////////////////////////////////////////////
// Main File:  indate.c, sendsig.c, division.c
// This File:  division.c
// Other Files:  sendsig.c, indate.c
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
#include <stdlib.h>
#include <signal.h>
#include <string.h>

int divCount = 0;
// global variable that keeps track of the number of times of
// successful integer division

/* handels a SIGFPE signal, which is sent when the user tries to
 * divide by 0. Then it prints the number of times of integer 
 * divisions completed successfully
 *
 * parameter: N/A
 * return: void
 */
void sigfpe_handler(){
    printf("Error: a division by 0 operation was attempted.\n");
    printf("Total number of operations completed successfully: ");
    printf("%d\n", divCount);
    printf("The program will be terminated.\n");
    exit(0);
}

/* handels a SIGINT signal, which is sent when Control + c
 * is pressed. Then it prints the number of times of integer 
 * divisions completed successfully
 *
 * parameter: N/A
 * return: void
 */
void sigint_handler(){
    printf("\nTotal number of operations completed successfully: ");
    printf("%d\n", divCount);
    printf("The program will be terminated.\n");
    exit(0);
}

/* This function drives the program to take user input and perform
 * integer division with that input. Then it prints the quotient
 * and the remainder of the division. This function registers 
 * handler functions to catch signals including SIGFPE and SIGINT.
 * It also keeps track of the number of times of successful division
 *
 * parameter: 
 * argc, number of command line arguments
 * argv, pointer to the command line argument char array
 * return: 0 on success (no error), non-zero value on error
 */
int main(int argc, char *argv[]) {  
    struct sigaction sigfpeAction;
    struct sigaction sigintAction;
    // structs to specify actions to be taken on designated signals
    memset(&sigfpeAction, 0, sizeof(sigfpeAction));
    memset(&sigintAction, 0, sizeof(sigintAction));
    // zero out the initialized sigaction struct
    
    sigfpeAction.sa_handler = sigfpe_handler;
    sigintAction.sa_handler = sigint_handler;
    // sets the handler functions
    
    int sigfpeActionResult = sigaction(SIGFPE, &sigfpeAction, NULL);
    int sigintActionResult = sigaction(SIGINT, &sigintAction, NULL);
    // registers the action structs with designated signals,
    // save the return value of sigaction() functions
    
    if (sigfpeActionResult == -1 || sigintActionResult == -1){
        printf("Failed to register handler function with signals.\n");
        exit(0);
        // check if sigAction() successfully registered handler functions
    }
    
    while (1){
        printf("Enter first integer: ");
        char firstInput[100];
        char *fgetsResult_1 = fgets(firstInput, 100, stdin);
        // get the user input, save the return value of fgets()
        
        if (fgetsResult_1 == NULL){
            printf("Failed to read first input integer from user\n");
            exit(0);
            // check if fgets() read the input successfully
        }
        
        int firstInt = atoi(firstInput);
        // convert use input characters into integers
        // Note: if the user input is invalid, atoi() returns 0 as desired
        printf("Enter second integer: ");
        char secondInput[100];
        char *fgetsResult_2 = fgets(secondInput, 100, stdin);
        
        if (fgetsResult_2 == NULL){
            printf("Failed to read second input integer from user\n");
            exit(0);
        }
        
        int secondInt = atoi(secondInput);
        int quotient = firstInt / secondInt;
        int remainder = firstInt - (quotient * secondInt);
        // perform the division and record the results
        printf("%d / %d is %d with a remainder of %d\n", 
        firstInt, secondInt, quotient, remainder);
        divCount++;
        // increment the number of times of successful integer division
        // Note: any error signal will be caught prior to this line
    }
}
