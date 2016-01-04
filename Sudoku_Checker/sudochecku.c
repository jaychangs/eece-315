#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "pthread.h"

#define ROWS 9
#define COLUMNS 9
#define MAX_PARAMETERS 81
#define NUM_THREADS 9

#ifndef NULL
#define NULL ...
#endif

int success_check = 0;

/* structure for passing data to threads */
typedef struct {
  int row;
  int column;
  int value;
} parameters;

void createParameter(parameters*, int, int, int);
void freeParameter(parameters*);
void parseSudoku(FILE*, char**);
void *checkRows(void*);
void *checkColumns(void*);
void *checkSquares(void*);

int main(int argc, char *argv[])  {

  int i, t, threadInt;
  int rowcount, columncount;
  char** values;
  int intValues[81];
  pthread_t threads[NUM_THREADS];
  parameters** paramArray;

  values = (char**)malloc ((MAX_PARAMETERS) * sizeof(char*));
  for(i = 0; i < MAX_PARAMETERS; i++){
    values[i] = (char*) malloc(sizeof(char));
  }

  // Check if filename is passed as argument
  if(argc != 2){
    printf("Error: Specify file to check.");
    return -1;
  }

  //Open the file indicated and error check
  FILE *sudoku = fopen(argv[1], "r");
  if(sudoku == NULL) {
    printf("File not found or cannot be opened.");
    return -1;
  }

  parseSudoku(sudoku, values);
  fclose(sudoku);
  for(i = 0; i < MAX_PARAMETERS; i++){
    intValues[i] = atoi(values[i]);
  }

  // Malloc an array to hold parameters for each square in Sudoku
  paramArray = (parameters**) malloc((MAX_PARAMETERS) * sizeof(parameters));

  for(rowcount = 0; rowcount < ROWS; rowcount++){
    for(columncount = 0; columncount < COLUMNS; columncount++){
      createParameter(paramArray[rowcount*columncount], rowcount, columncount, intValues[rowcount*columncount]);
    }
  }
	
  for(t=0; t<NUM_THREADS; t++){
    printf("Creating thread %i for rows\n", t+1);
    threadInt = pthread_create(&threads[t], NULL, checkRows, (void *)paramArray);
    if(threadInt){
      printf("ERROR %d\n", threadInt);
      exit(-1);
    }
  }

  for(rowcount = 0; rowcount < ROWS; rowcount++){
    for(columncount = 0; columncount < COLUMNS; columncount++){
      freeParameter(paramArray[rowcount*columncount]);
    }
  }
  free(paramArray);
  pthread_exit(NULL);
  return 0;
}

void createParameter(parameters* data, int row, int column, int value){
  data = (parameters *) malloc(sizeof(parameters));
  data->row = row;
  data->column = column;
  data->value = value;
  return;
}

void freeParameter(parameters* data){
  free(data);
  return;
}

void *checkRows(void* rowArgs){
  parameter** elements;
  elements = rowArgs;

  pthread_exit(NULL);
}
void *checkColumns(void* columnArgs){
  pthread_exit(NULL);
}
void *checkSquares(void* squareArgs){
  pthread_exit(NULL);
}

void parseSudoku(FILE* sudoku, char* dirs[]){
  char sudokuPuzzle [300];
  char *theSudoku;
  char temp = ' ';
  int i;

  theSudoku = (char *)malloc(strlen(sudokuPuzzle)+1);

  while(!feof(sudoku)){
    //fseek(sudoku, 0, SEEK_SET);
    fread(sudokuPuzzle, sizeof(char), 1, sudoku);
    if(strcmp(sudokuPuzzle, "\n") != 0) strcat(theSudoku, sudokuPuzzle);
  }

  dirs[0] = strtok(theSudoku, &temp);
  for(i = 1; i < MAX_PARAMETERS; i++){
    dirs[i] = strtok(NULL, &temp);
    if(dirs[i] == NULL){
      break;
    }
  }
  return;
}