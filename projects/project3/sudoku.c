#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define BUFF_SIZE 162
#define GRID_SIZE 9
#define NUM_THREADS 11

char buff[BUFF_SIZE];
int grid[GRID_SIZE][GRID_SIZE];

int valid = 1;

typedef struct{
  int row;
  int col;
} parameters;

void *check_cols(){
  for(int i = 0; i < GRID_SIZE; i++){
    int cols_check[GRID_SIZE] = {0};  
    int j;
     
    for(j = 0; j < GRID_SIZE; j++){
      cols_check[grid[i][j] - 1]++;
    }
    
    for(j = 0; j < GRID_SIZE; j++){
      if(!cols_check[j]){
	printf("Digit %d is missing from column %d\n", j+1, i); 
	valid = 0;
      }
    }
  }
  pthread_exit(0);
}

void *check_rows(){
  for(int i = 0; i < GRID_SIZE; i++){
    int rows_check[GRID_SIZE] = {0};  
    int j;
     
    for(j = 0; j < GRID_SIZE; j++){
      rows_check[grid[j][i] - 1]++;
    }
    
    for(j = 0; j < GRID_SIZE; j++){
      if(!rows_check[j]){
        printf("Digit %d is missing from row %d\n", j+1, i); 
        valid = 0;
      }
    }
  }
  pthread_exit(0);
}

void *check_grid(void *params){
  parameters *p = (parameters*)params;
  int col = p->col;
  int row = p->row;
  int j;
  
  int grid_check[GRID_SIZE] = {0};

  
  for(int i = col; i < (col + 3); i++){
    for(j = row; j < (row + 3); j++){
      grid_check[grid[i][j] - 1]++;
    }
  }

  for(j = 0; j < GRID_SIZE; j++){
    if(!grid_check[j]){
      printf("Digit %d is missing from rows %d...%d and columns %d...%d\n", j + 1, col + 1, col + 3, row + 1, row + 3);
      valid = 0;
    }
  }
  pthread_exit(0);
}

parameters* parameter(int col, int row){
  parameters* params = (parameters *) malloc(sizeof(parameters));
  params -> col = col;
  params -> row = row;
  return params; 
}

int main(int argc, char *argv[ ]){  
  FILE *fp;
  pthread_attr_t attr;
  pthread_t workers[NUM_THREADS];
  int i;
  
  fp = fopen(argv[1], "r");
  fgets(buff, BUFF_SIZE, fp);
  fclose(fp);

  for(i = 0; i < BUFF_SIZE; i+=2){
    grid[i/(GRID_SIZE * 2)][(i%(GRID_SIZE * 2))/2] = buff[i] - '0';
  }

  pthread_attr_init(&attr);
  pthread_create(&workers[0], &attr, check_cols, NULL);
  pthread_create(&workers[1], &attr, check_rows, NULL);

  int worker_num = 2;
  
  for(i = 0; i < 3; i++){
    for(int j = 0; j < 3; j++){
      pthread_create(&workers[worker_num], &attr, check_grid, parameter(i * 3, j * 3));
      worker_num++;
    }
  }
  
  for(i = 0; i < NUM_THREADS; i++){
    pthread_join(workers[i], NULL);
  }
  
  if(valid){
    printf("This sudoku solution is valid!\n");
  }

  else{
    printf("This sudoku solution is invalid.\n");
  }
  
  return 1;
}


