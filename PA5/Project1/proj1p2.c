/*
  Modified version of project 1, using 128 page frames instead of 256 and includes a page replacement policy
 */

#include <stdio.h>
#include <stdlib.h>

#define n 8
#define m 16
#define BASE_ADDR 0x08048000
 
int main(int argc, char *argv[])  {

  int vaddr, paddr, p, d, count, temp;
  int PAGE_NUM_TABLE[128] = {0};
  int FRAME_NUM_TABLE[128] = {0};
  int TLB[32];
  int TLB_pos = 0;
  int TLBhit = 0;
  int TLBhitcount = 0;
  int position = 0;
  int NOT_in_table = 1;
  unsigned char buffer[1000];
  int i = 0;
  float page_faults = 0;
  float page_fault_rate, TLB_hit_rate;
  int translated_count = 0;

  FILE *fp;
  FILE *bin;

  // Check if filename is passed as argument
  if(argc != 2) {
   printf("Error: Specify file to check.\n");
   return -1;
  }

  for(i=0; i<32; i++) {
      TLB[i] = -1;
  }
 
  fp = fopen(argv[1],"r"); 
  bin = fopen("BACKING_STORE.bin", "r");

  if(fp == NULL) {
    perror("Error opening addresses.txt\n");
    exit(EXIT_FAILURE);
  }
 
  if(bin == NULL) {
    perror("Error opening addresses.txt\n");
    exit(EXIT_FAILURE);
  }

  fscanf(fp, "%d", &vaddr);

  while(!feof(fp)) {
    p = (vaddr & 0x0000FFFF) >> 8; //p is address
    d = vaddr & 0x000000FF;
  
// i did the TLB as 2 columns of 16 rows instead of 2 seperate buffers, but its still 16 entry TLB
    for(count = 0; count < 32; count = count + 2) { //TLB is 16 entry, the odd entry are page numbers, even ones are paddr
      if(TLB[count] == p) {
        paddr = TLB[count + 1];
        TLBhit = 1;
        TLBhitcount++;
      }
    }
    
    if(TLBhit == 0) {
      for(count = 0; count < 128; count++) {
        if(PAGE_NUM_TABLE[count] == p) {
          paddr = FRAME_NUM_TABLE[count] + d;
          NOT_in_table = 0;
          temp++;
        }
      }

      // Modified to use 128 page frames
      if(NOT_in_table == 1) {
        paddr = position * 256;
        PAGE_NUM_TABLE[position] = p;
        FRAME_NUM_TABLE[position] = paddr;
        paddr = paddr + d;
        position = (position + 1) % 128;
        page_faults++;
      }
      
      TLB[TLB_pos] = p;
      TLB[TLB_pos + 1] = paddr;
      TLB_pos = (TLB_pos + 2) % 32; 
    }

    TLBhit = 0;
    NOT_in_table = 1;
    fseek(bin, vaddr, SEEK_SET);
    fread(buffer, 1, 1, bin);

    printf("vaddr: %d | paddr: %d | value: %d | pnum: %d | offset: %d\n", vaddr, paddr, ((char*)buffer)[0], p, d);

    fscanf(fp, "%d", &vaddr);
    translated_count++;
  }

  page_fault_rate = page_faults/1000.00;
  TLB_hit_rate = TLBhitcount/1000.00;

  printf("Number of Translated Addresses = %d\n", translated_count);

  printf("Page Faults = %d\n", (int)page_faults);
  printf("Page Fault Rate = %.3f\n", (double)page_fault_rate);

  printf("TLB Hits = %d\n", TLBhitcount);
  printf("TLB Hit Rate = %.3f\n", TLB_hit_rate);

  fclose(fp);
  fclose(bin);
  return 0;
}
