#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "unistd.h"
#include "sys/types.h"
#include "sys/wait.h"
#include "sys/stat.h"
#include "fcntl.h"
#include "sys/ipc.h"
#include "sys/shm.h"
#include "math.h"


/*
	Answers calculated from bluebit matrix solver
	For N = 3, output should be {1, 2, -2}
	For N = 4, output should be {1 ,2 ,-1 ,1}
	For N = 5, output should be {1.888, -1.103, -2.258, 1.978, -1.612}
	For N = 6, output should be {1, 2, 3, 4, 5 ,6}
*/
#define N 3
#define w 1.25
#define ITERATIONS 100

int main(int argc, char *argv[]) {

  int i = 0;
  int j = 0;
  int shmid, shmid2, x;
  int pids[N] = {0};
  double temp;
  double *shm;
  int *shm2;
  double A3[3][3] = { { 3, -1, 1 }, { -1, 3, -1 }, { 1, -1, 3 } };
  double b3[3] = { -1, 7, -7};
  double A4[4][4] = { {10, -1,  2, 0},
  					  {-1, 11, -1, 3},
  					  { 2, -1, 10,-1},
  					  { 0,  3, -1, 8}};
  double b4[4] = { 6, 25, -11, 15};
  double A5[5][5] = { {100, -10, -15, -4, 16},
  					  {-10, 105, -13,  4, 14},
  					  {-14, -13,  90, 19,-11},
  					  { 12,   4,  14,110, 15},
  					  { 16,  14, -10,  8, 95}};
  double b5[5] = { 200, -120, -160, 180, -100};
  double A6[6][6] = { {100, 10, 5, 10, 5, 10},
  					  { 5, 200,5,10, 5, 10},
  					  { 10, 1, 200, 5, 10, 1},
  					  { 10, 5, 5, 150,5, 10},
  					  { 10,5,5, 10, 150, 10},
  					  { 10,10, 10, 5, 5, 200}};
  double b6[6] = {260, 545, 688, 720, 885, 1305};
/*
  for(x = 0; x < N; x++){
    for(y = 0; y < N; y++){
      //A[x][y] = (x*N + y)+1;
      printf("%f ", A[x][y]);
    }
    printf("\n");
  }

  for(x = 0; x < N; x++){
    //b[x] = x+1;
    printf("%f ", b[x]);
  }
  printf("\n");
*/

  shmid = shmget(IPC_PRIVATE, N, IPC_CREAT | 0666);
  if(shmid < 0){
    perror("shmget");
    abort();
  }

  shmid2 = shmget(IPC_PRIVATE, N*ITERATIONS, IPC_CREAT | 0666);
  if(shmid < 0){
    perror("shmget");
    abort();
  }

  // Create child processes to execute the commands
  // Taken from StackOverflow
  for(i = 0; i<N; i++){
     if ((pids[i] = fork()) < 0) {
      perror("fork");
      abort();
    } 
    else if (pids[i] == 0) {
      //Shared memory segment that holds vector answer
      shm = shmat(shmid, NULL, 0);
      if(shm == -1){
        perror("shmat");
        abort();
      }
      //Shared memory segment that holds progression flags
      shm2 = shmat(shmid2, NULL, 0);
      if(shm2 == -1){
        perror("shmat");
        abort();
      }
      shm2[0] = 1;

      //Begin SOR algorithm
        if( N == 3){
            for(x = 0; x < ITERATIONS; x++){ 
            while(shm2[x*N + i] == 0);
                temp = 0;
                for(j = 0; j < N; j++){
                    if(j != i)
                    temp = (temp) + A3[i][j]*shm[j];
                }
                shm[i] = (1-w)*shm[i] + w*(b3[i] - temp)/A3[i][i];
                shm2[x*N + i + 1] = 1;
            }
            exit(0);
        }
        else if( N == 4){
            for(x = 0; x < ITERATIONS; x++){ 
            while(shm2[x*N + i] == 0);
                temp = 0;
                for(j = 0; j < N; j++){
                    if(j != i)
                    temp = (temp) + A4[i][j]*shm[j];
                }
                shm[i] = (1-w)*shm[i] + w*(b4[i] - temp)/A4[i][i];
                shm2[x*N + i + 1] = 1;
            }
            exit(0);
        }
        else if( N == 5){
            for(x = 0; x < ITERATIONS; x++){ 
            while(shm2[x*N + i] == 0);
                temp = 0;
                for(j = 0; j < N; j++){
                    if(j != i)
                    temp = (temp) + A5[i][j]*shm[j];
                }
                shm[i] = (1-w)*shm[i] + w*(b5[i] - temp)/A5[i][i];
                shm2[x*N + i + 1] = 1;
            }
            exit(0);
        }
        else if( N == 6){
            for(x = 0; x < ITERATIONS; x++){ 
            while(shm2[x*N + i] == 0);
                temp = 0;
                for(j = 0; j < N; j++){
                    if(j != i)
                    temp = (temp) + A6[i][j]*shm[j];
                }
                shm[i] = (1-w)*shm[i] + w*(b6[i] - temp)/A6[i][i];
                shm2[x*N + i + 1] = 1;
            }
            exit(0);
        }
    }
  }
  shm = shmat(shmid, NULL, 0);
  if(shm == -1){
    perror("shmat");
    abort();
  }

  int status;
  int n = N;
  while (n > 0) {
    wait(&status);
    --n;  // TODO(pts): Remove pid from the pids array.
  }
  printf("Vector is {");
  for(i = 0; i < N; i++){
    printf(" %f", *(shm + i));
  }
  printf("}\n");
  return 0;
}
