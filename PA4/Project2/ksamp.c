#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stddef.h>
#include <unistd.h>

void find_kernel_version(void);
void find_processor_type(void);
void find_time_since_last_boot(void);
void find_times(void);
void find_diskstats(void);
void find_contextswitch(void);
void find_boottime(void);
void find_numproc(void);
void find_total_memory();
void find_current_memory();
void find_load(char* delaytimec, char* totaltimec);

int main (int argc, char *argv[]) {

  // Version 1 (no arguments)
  find_processor_type();
  find_kernel_version();
  find_time_since_last_boot();
  
  // Version 2/3 (-s)
  if (argc > 1) {
  // Version 2 with -s
    if (strcmp(argv[1], "-s") == 0) {
      find_times();
      find_diskstats();
      find_contextswitch();
      find_boottime();
      find_numproc();
    }
  // Version 3 with -1
    if (strcmp(argv[1], "-1") == 0) {
      find_times();
      find_diskstats();
      find_contextswitch();
      find_boottime();
      find_numproc();
      find_total_memory();
      find_current_memory();
      find_load(argv[2], argv[3]);
    }
  }

  return 0;
}

// Isolates the total memory field under /proc/meminfo and prints.
void find_total_memory() {
  FILE *fp;
  char buffer[200];
  char* token;

  fp = fopen("/proc/meminfo", "r");
  fread(buffer, sizeof(buffer), 1, fp);
  token = strtok(buffer, "\n");
  printf("%s\n", token);
  fclose(fp);
}

// Isolates the available memory field under /proc/meminfo and prints.
void find_current_memory() {
  FILE *fp;
  char buffer[200];
  char* token;


  fp = fopen("/proc/meminfo", "r");
  fread(buffer, sizeof(buffer), 1, fp);
  token = strtok(buffer, "\n");
  token = strtok(NULL, "\n");
  printf("%s\n", token);
  fclose(fp);
}

// Takes in 2 arguments from the user and continuously prints the first
// field under /proc/loadavg with a given interval
void find_load(char* delaytimec, char* totaltimec) {
  FILE *fp;
  int delay = atoi(delaytimec);
  int totaltime = atoi(totaltimec);
  int interval = totaltime/delay;
  float avg;

  int count;
  for(count = 0; count < interval; count++) {
    fp = fopen("/proc/loadavg", "r");
    sleep(delay);
    fscanf(fp, "%f", &avg);
    printf("Average Load Time Sample %d: %f\n", count+1, avg);
    fclose(fp);
  }
}

// Prints the entirety of /proc/version.
void find_kernel_version(void) {
  FILE *fp;
  char buffer[100];
  fp = fopen("/proc/version", "r");
  fread(buffer, 100, 1, fp);
  printf("Kernel Version: %s\n", buffer);
  fclose(fp);
}


// Prints the 10th token in /proc/cpuinfo, deliminated by either a new line character
// or a colon.
void find_processor_type(void) {
  FILE *fp;
  char buffer[200];
  char* token;
  char delim[5] = ":\n";
  int i;

  fp = fopen("/proc/cpuinfo", "r");
  fread(buffer, sizeof(buffer), 1, fp);
  token = strtok(buffer,delim);
  for (i=0; i<9; i++) {
    token = strtok(NULL, delim);
  }
  printf("Processor Type:%s\n", token);
  fclose(fp);
}


// Prints the first element located in /proc/uptime.
void find_time_since_last_boot(void) {
  FILE *fp;
  char time[10];

  fp = fopen("/proc/uptime", "r");
  fscanf(fp, "%s", time);
  printf("Time since last boot: %s seconds\n", time);
  fclose(fp);
}

// Prints the second, fourth and fifth element located in /proc/stat.
void find_times(void) {
  FILE *fp;
  char cpu[10];
  char user_time[10];
  char nice_time[10];
  char sys_time[10];
  char idle_time[10];

  fp = fopen("/proc/stat", "r");
  fscanf(fp, "%s %s %s %s %s", cpu, user_time, nice_time, sys_time, idle_time);
  printf("Time spent in user mode: %s jiffies\n", user_time);
  printf("Time spent in system mode: %s jiffies\n", sys_time);
  printf("Time spent idle: %s jiffies\n", idle_time);
  fclose(fp);
}

// Isolates the sda line under /proc/diskstats, and isolates each field, and chooses
// the number of reads/writes to print.
void find_diskstats(void) {
  FILE *fp;
  char buffer[5000000];
  char* token;
  char* parse;
  char delim[5] = "\n";
  char searchfor[18] = "   8       0 sda ";
  char copy[18];
  int i, j;
  
  fp = fopen("/proc/diskstats", "r");
  fread(buffer, sizeof(buffer), 1, fp);
  token = strtok(buffer, delim);
  while(token != NULL) {
    for(i=0; i<17; i++) {
      copy[i] = token[i];
    }
    if(strcmp(copy,searchfor) == 0) {
      for(j=0; j<23; j++) {
        parse = strsep(&token, " ");
        if(j == 12) {
          printf("Number of reads performed: %s\n", parse);
        } else if (j == 16) {
          printf("Number of writes performed: %s\n", parse);
        }
      }
    }
    token = strtok(NULL, delim);
  }

  fclose(fp);
}

// Isolates and prints the context switch field under /proc/stat.
void find_contextswitch(void) {
  FILE *fp;
  char buffer[50000];
  char* token;
  char* parse;
  char searchfor[5] = "ctxt";
  char copy[5];
  char delim[5] = "\n";
  int i, j;

  fp = fopen("/proc/stat", "r");
  fread(buffer, sizeof(buffer), 1, fp);
  token = strtok(buffer, delim);
  while(token != NULL) {
    for(i=0; i<4; i++) {
      copy[i] = token[i];
    }
    if(strcmp(copy,searchfor) == 0) {
      for(j=0; j<2; j++) {
        parse = strsep(&token, " ");
        if(j == 1) {
          printf("Number of context switches: %s\n", parse);
        }
      }
    }
    token = strtok(NULL, delim);
  }

  fclose(fp);
}

// Finds the current time and time since last boot, subtracts the latter
// from the former to find the time booted.
void find_boottime(void) {
  FILE *fp;
  time_t rawtime;
  struct tm * timeinfo;
  int current_time, time_since_boot, boot_time;

  time(&rawtime);
  timeinfo = localtime(&rawtime);
  current_time = timeinfo->tm_hour * 3600 + timeinfo->tm_min * 60 + timeinfo->tm_sec;

  fp = fopen("/proc/uptime", "r");
  fscanf(fp, "%d", &time_since_boot);
  fclose(fp);

  boot_time = current_time - time_since_boot;
  timeinfo->tm_hour = boot_time / 3600;
  timeinfo->tm_min = (boot_time - (timeinfo->tm_hour * 3600)) / 60;
  timeinfo->tm_sec = boot_time - (timeinfo->tm_hour * 3600) - (timeinfo->tm_min * 60);

  printf("Time last booted: %s", asctime(timeinfo));
}

// Isolates and prints the processes field in /proc/stat.
void find_numproc(void) {
  FILE *fp;
  char buffer[50000];
  char* token;
  char* parse;
  char searchfor[10] = "processes";
  char copy[10];
  char delim[5] = "\n";
  int i, j;

  fp = fopen("/proc/stat", "r");
  fread(buffer, sizeof(buffer), 1, fp);
  token = strtok(buffer, delim);
  while(token != NULL) {
    for(i=0; i<9; i++) {
      copy[i] = token[i];
    }
    if(strcmp(copy,searchfor) == 0) {
      for(j=0; j<2; j++) {
        parse = strsep(&token, " ");
        if(j == 1) {
          printf("Number of processes: %s\n", parse);
        }
      }
    }
    token = strtok(NULL, delim);
  }

  fclose(fp);
}
