#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

int main (int argc, char *argv[]) {

  //initizations and declarations
  FILE *fp;
  char line[60];
  char *running;
  char delims[] = ",";
  int arrival_t, priority, processor_t, Mbytes, num_printers, num_scanners, num_modems, num_CDs;
  int i = 1;

  fp = fopen(argv[1], "r");

  if (fp == NULL) {
    exit(EXIT_FAILURE);
  }

  //parse the files
  while (!feof(fp)) {
  
    fgets(line, sizeof(line), fp);
    running = strdupa (line);
    arrival_t = atoi(strsep(&running, delims));
    priority = atoi(strsep(&running, delims));
    processor_t = atoi(strsep(&running, delims));
    Mbytes = atoi(strsep(&running, delims));
    num_printers = atoi(strsep(&running, delims));
    num_scanners = atoi(strsep(&running, delims));
    num_modems = atoi(strsep(&running, delims));
    num_CDs = atoi(strsep(&running, delims));
    printf("Process %d: %s", i, line);
    printf("Arrival: %d\nPriority: %d\nProccessor: %d\nMbytes: %d\nNumPrinters: %d\nNumScanners: %d\nNumModems: %d\nNumCDs: %d\n", arrival_t, priority, processor_t, Mbytes, num_printers, num_scanners, num_modems, num_CDs);
    i++;
  }
  
  return 0;
}
