/**
 * Starting point for project 7 ("Game of Life")
 * Melinda Grad
 * Comp 280 Project 7
 * This program simulates the Game of Life 
 * given world parameters spcified by a test file 
 */
#define _BSD_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <string.h>
#include <getopt.h>

// Function Prototypes
int *initBoard(char *config_file, int *rows, int *cols, int *time_step, int *lives);
void printBoard(int *board, int rows, int cols, int last_flag);
void gameFunc(int *board, int rows, int cols);
int getIndex(int i, int j, int rows, int cols);
static void timeval_subtract (struct timeval *result,
                        struct timeval *end, struct timeval *start);

int main(int argc, char *argv[]) {

	int opt = 0;
	int rows, cols, time_step, lives = 0;
	int verbose_flag=0, file_flag = 0;	
	char *config_file; //configuration file	

	while((opt = getopt(argc, argv,"vc:"))!= -1){
		switch(opt){
			case 'v':
				verbose_flag = 1;
				break;
			case 'c':
				file_flag = 1;
				config_file = optarg;
				break;
			case '?':
				printf("ERROR: incorrect arguments!\n");
				printf("Please enter:-c <filename> -v (for verbose mode)\n");
				exit(1);
		}
	}

	if(!file_flag){
		printf("Please enter -c \n");
		exit(1);
	}

	// Initialize the board
	int *board = initBoard(config_file, &rows, &cols, &time_step, &lives);
	// Start your timer
	struct timeval tv1, tv2, result;
	gettimeofday(&tv1,NULL);

	for(int i =0; i < time_step; i++){
		// To indicate last iteration
		int last_flag = 0;
		gameFunc(board, rows, cols);

		if(verbose_flag){
			if(i==time_step-1)
				last_flag = 1;
			printf("Timestep: %d\n", i);
			printBoard(board, rows, cols, last_flag);
		}
	
	}
	
	gettimeofday(&tv2, NULL);
	timeval_subtract (&result, &tv2, &tv1);
	printf("Total time for %d iterations of a %dx%d world is %ld.%06ld secs\n", 
		time_step, rows, cols,result.tv_sec, result.tv_usec);	
	free(board);
	return 0;
}

/* Function to execute game play
 * @param board game board to modify
 * @param rows number of rows
 * @param cols number of columns
 */
void gameFunc(int *board, int rows, int cols){

	int *temp = malloc(sizeof(int)*rows*cols);
	if(temp == NULL){
		printf("Error initializing array");
		exit(1);
	}

	for(int i = 0; i< rows*cols; i++){
		temp[i] = board[i];
	}

	for(int i = 0; i< rows; i++){
		for(int j = 0; j < cols; j++){
		
			int count;
			//get neighbors
			int n1 = getIndex(i-1, j-1, rows, cols);
			int n2 = getIndex(i-1,j, rows, cols);
			int n3 = getIndex(i-1, j+1, rows, cols);
			int n4 = getIndex(i, j+1, rows, cols);
			int n5 = getIndex(i+1, j+1, rows, cols);
			int n6 = getIndex(i+1, j, rows, cols);
			int n7 = getIndex(i+1, j-1, rows, cols);
			int n8 = getIndex(i, j-1, rows, cols);
			//add elements of all neighbors
			count = temp[n1] + temp[n2] + temp[n3] + temp[n4] + temp[n5] + 
				temp[n6] + temp[n7]+ temp[n8];	
			
			// Live cell with 0 or 1 neighbors dies
			if(count <=1)
				board[i*cols +j] = 0;
			// Live cell with 4 or more neighbors dies
			if(count >= 4)
				board[i*cols +j] = 0;
			// Dead cell with 3 lives neighbors becomes alive
			if(count == 3)
				board[i*cols +j] = 1;
		
		}		
	}

	free(temp);
}



/* Helper function to get the index of the neighbors in the torridal array
 * @param i row index
 * @param j column index
 * @param rows number of rows
 * @param cols number of columns
 * @return index in the array
 */
int getIndex(int i, int j, int rows, int cols){
	// Wrap around rows
	if(i >= rows){
		i = 0;
	}
	if(i < 0){
		i = rows -1;
	}
	// Wrap around cols
	if(j >= cols){
		j = 0;
	}
	if(j < 0){
		j = cols -1;
	}

	return (i * cols + j);
}

/*Function to initialize the game board
 *@param config_file file to be opened
 *@param rows number of rows
 *@param cols number of colums
 *@param time_step number of iterations
 *@param lives number of initial lives
 *@return board initialized game board array 
 */
int *initBoard(char *config_file, int *rows, int *cols, int *time_step, int * lives){
	// Read in the file
	FILE *input = fopen(config_file, "r");
	if(input == NULL){
		fprintf(stderr, "Error could not open file %s for reading\n", config_file);
		exit(1);
	}
	// Read in game specs
	int ret=0;
	ret = fscanf(input,"%d",rows);
	if(ret !=1)
		printf("Error reading in line");

	ret = fscanf(input,"%d",cols);
	if(ret !=1){
		printf("Error reading in line");
		exit(1);
	}
	ret = fscanf(input,"%d",time_step);
	if(ret !=1){
		printf("Error reading in line");
		exit(1);
	}
	ret = fscanf(input,"%d", lives);
	if(ret !=1){
		printf("Error reading in line");
		exit(1);
	}
	// Allocate memory and initilize the board
	int *the_board = calloc(*rows**cols, sizeof(int));
	if(the_board == NULL){
		printf("Error initializing array");
		exit(1);
	}
	for(int i = 0; i< *lives; i++){
		int new_x, new_y = 0;
		fscanf(input, "%d %d", &new_x, &new_y);
		if(ret !=1){
			printf("Error reading in line");
			exit(1);
		}
		the_board[(new_x**cols) + new_y] = 1;
	}
	fclose(input); 
	return the_board;
	
}
/*Function to print the game board
 *@param board gameboard to be printed
 *@param rows number of rows
 *@param cols number of colums
 *@param last_flag to indicate final game baord state
 */
void printBoard(int *board, int rows, int cols, int last_flag){

	for(int i = 0; i< rows*cols; i++){
		printf(" %d ", board[i]);
		if((i+1) % cols == 0){
			printf("\n");
		}
	}
	printf("\n");
	usleep(200000);
	//don't clear if last board
	if(!last_flag)
		system("clear");

}

static void timeval_subtract (struct timeval *result,
                        struct timeval *end, struct timeval *start)
{	//Perform the carry for later subtraction by updating start
	if (end->tv_usec < start->tv_usec) {
		int nsec = (start->tv_usec - end->tv_usec) / 1000000 + 1;
	 	start->tv_usec -= 1000000 * nsec;
	 	start->tv_sec += nsec;
	}
	if (end->tv_usec - start->tv_usec > 1000000) {
		int nsec = (end->tv_usec - start->tv_usec) / 1000000;
		start->tv_usec += 1000000 * nsec;
		start->tv_sec -= nsec;
	}
	// Compute the time remaining to wait.tv_usec is certainly positive.
	result->tv_sec = end->tv_sec - start->tv_sec;
	result->tv_usec = end->tv_usec - start->tv_usec;
}
