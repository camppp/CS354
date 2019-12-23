////////////////////////////////////////////////////////////////////////////////
// Main File:        traverse_spiral.c
// This File:        traverse_spiral.c
// Other Files:      N/A
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
//
// Online sources:   N/A
//                   
//                   
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *COMMA = ","; 

// Structure representing Matrix
// size: Dimension of the square (size*size)
// array: 2D array of integers
typedef struct _Square 
{  
    int size;
    int **array;
} Square;

/* TODO:
 * Retrieves from the first line of the input file,
 * the size of the square matrix.
 * 
 * fp: file pointer for input file
 * size: pointer to size of the square matrix
 */
void get_dimensions(FILE *fp, int *size) 
{   
	char *line = NULL;  // varaible to store getLine()'s result
	size_t len = 12;   // buffer size is specified
	if (getline(&line, &len, fp) == -1) 
	{ 
		printf("Error in reading the file\n");
		exit(1); 
	}
	// check if getLine() is successful
	int num = atoi(line);
	*size = num;
}	

/* PARTIALLY COMPLETED:
 * Traverses a given layer from the square matrix
 * 
 * array: Heap allocated 2D square matrix
 * rows: pointer to size of square
 * layer: layer number to be traversed
 * op: pointer to the output file
 */
void traverse_layer(int **array, int size, int layer, FILE *op) 
{       
	int row_num, col_num; // the row and column number at each layer
	int i; // iterator vairable

    if(size % 2 == 1 && layer == (size + 1) / 2 - 1)
	{
        fprintf(op, "%d\n", *(*(array + layer) + layer));
        return;
    }
	//corner case: size is odd & the layer is last so only one entry to print
	
	row_num = layer;
	for(i = layer; i < size - layer; i++) 
	{
		fprintf(op, "%d ", *(*(array + row_num) + i)); 
	}	
	//Traverse upper row from left to right with appropriate bounds
	
	col_num = size - layer - 1;
	for(i = layer + 1; i < size - layer - 1; i++) 
	{
		fprintf(op, "%d ", *(*(array + i) + col_num));
    }
	//Traverse right column from top to bottom with appropriate bounds
	
	row_num = size - layer - 1;
	for(i = size - layer - 1; i >= layer; i--) 
	{
		fprintf(op, "%d ", *(*(array + row_num) + i));
	}
	//Traverse lower row from right to left with appropriate bounds
	
	col_num = layer;
	for(i = size - layer - 2; i >= layer + 1; i--) 
	{
		fprintf(op, "%d ", *(*(array + i) + col_num));
	}
	//Traverse left column from bottom to top with appropriate bounds
}


/* COMPLETED:
 * Traverses the square matrix spirally
 * 
 * square: pointer to a structure that contains 2D square matrix
 * op: pointer to the output file
 */
void traverse_spirally(Square *square, FILE *op)
{       
	int size = square->size; 
	int num_layers = 0;   
	num_layers = size/2; 
	if(size%2 == 1) // when the number of layers is odd
	{
		num_layers++;
	}
	int i;
	for(i = 0; i < num_layers; i++)	
	{
		traverse_layer(square->array, size, i, op);
	}
}

/* PARTIALLY COMPLETED:
 * This program reads a square matrix from the input file
 * and outputs its spiral order traversal to the output file
 *
 * argc: CLA count
 * argv: CLA value
 */
int main(int argc, char *argv[])
{                  
	if (argc != 3) 
	{                                        
		printf("Usage: ./traverse_spiral <input_filename> <output_filename>");
		exit(1);
	}
	//Check if number of arguments is correct
	
	FILE *fp1 = fopen(*(argv + 1), "r"); 
	if (fp1 == NULL) 
	{
		printf("Cannot open file for reading\n");
		exit(1);
	} 
	//Open the file and check if it opened successfully
	
	int sizeOfMatrix = 0;
	get_dimensions(fp1, &sizeOfMatrix);
	//Call the function get_dimensions to retrieve size of the square matrix
	
	int **matrix = malloc(sizeof(int*)*sizeOfMatrix);
	if(matrix == NULL)
	{
		printf("Memory allocation failed");
		exit(1);
	}
	// Check if malloc is successful
	
	for (int i = 0; i < sizeOfMatrix; i++) 
	{
		*(matrix + i) = malloc(sizeof(int)*sizeOfMatrix);
		if(*(matrix + i) == NULL)
		{
			printf("Memory allocation failed");
			exit(1);
		}
		// Check if malloc is successful
	}
	//Dynamically allocate a 2D array as per the retrieved dimensions
	
	char *line = NULL; // varaible to store getLine()'s result
	size_t len = 12; // buffer size is specified
	char *token = NULL;
	for (int i = 0; i < sizeOfMatrix; i++) 
	{
		if (getline(&line, &len, fp1) == -1) 
		{
			printf("Error while reading the file\n");
			exit(1);	
		}
		// check if getLine() is successful
		token = strtok(line, COMMA);
		for (int j = 0; j < sizeOfMatrix; j++) 
		{
			*(*(matrix + i) + j) = atoi(token);
			token = strtok(NULL, COMMA);	
		}
	}
	//Read the file line by line by using the function getline as used in get_dimensions
	//Tokenize each line wrt comma to store the values in the square matrix

	Square square = {.size = sizeOfMatrix, .array = matrix};
	//Create a structure and initialize its size and array fields appropriately

	FILE *fp2 = fopen(*(argv + 2), "w");
	if (fp2 == NULL) 
	{
		printf("Cannot open file for writing\n");
		exit(1);
	}
	//Open the output file and check if it opened successfully

	traverse_spirally(&square, fp2);
	//Call the function traverse_spirally

	for(int i = 0; i < sizeOfMatrix; i++) 
	{
		free(*(matrix + i));
	}
	free(matrix);
	matrix = NULL;
	//Free the dynamically allocated memory for the matrix
	
	if (fclose(fp1) != 0) 
	{
		printf("Error while closing the input file\n");
		exit(1);	
	}
	//Close the input file and check if it closed successfully
	
	if (fclose(fp2) != 0) 
	{
		printf("Error while closing the output file\n");
		exit(1);	
	}
	//Close the output file and check if it closed successfully

	return 0; 
}     
