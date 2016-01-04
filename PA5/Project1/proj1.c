#include <stdio.h>
#include <stdlib.h>

#define n 8
#define m 16
#define BASE_ADDR 0x08048000
 
int main(int argc, char *argv[])  {

  int vaddr, paddr, p, d, count;
  int PAGE_TABLE[256];
  int VALID_TABLE[256];
  int COUNT_TABLE[256];
  int TLB[32];
  int TLB_pos = 0;
  int TLBhit = 0;
  int TLBhitcount = 0;
  unsigned char buffer[1000];
  int i = 0;
  int j = 0;
  int translated_count = 0;
  float page_faults = 0;
  float page_fault_rate, TLB_hit_rate;

  FILE *fp;
  FILE *bin;

  // Check if filename is passed as argument
  if(argc != 2) {
   printf("Error: Specify file to check\n");
   return -1;
  }

  // Initialize arrays for use later
  for(i=0; i<256; i++) {
    PAGE_TABLE[i] = i;
    VALID_TABLE[i] = 0;
    COUNT_TABLE[i] = i;
  }

  for(i=0; i<32; i++) {
      TLB[i] = -1;
  }
 
  fp = fopen(argv[1],"r"); 
  bin = fopen("BACKING_STORE.bin", "r");

  // Error checking for opening files
  if(fp == NULL) {
    perror("Error opening addresses.txt\n");
    exit(EXIT_FAILURE);
  }
 
  if(bin == NULL) {
    perror("Error opening addresses.txt\n");
    exit(EXIT_FAILURE);
  }

  fscanf(fp, "%d", &vaddr);

  // Use bit masking and bit shifting to acquire page number and offset
  while(!feof(fp)) {
    p = (vaddr & 0x0000FFFF) >> 8; // Page number
    d = vaddr & 0x000000FF; // Offset
  
<<<<<<< HEAD
  // i did the TLB as 2 columns of 16 rows instead of 2 seperate buffers, but its still 16 entry TLB
    for(count = 0; count < 32; count = count + 2) { //TLB is 16 entry, the odd entry are page numbers, even ones are paddr
=======
    // In case of TLB hit, directly translate to physical address
    for(count = 0; count < 32; count = count + 2) {
>>>>>>> e1f723c61178454a9a1bb16ddf49c472c204c95f
      if(TLB[count] == p) {
        paddr = TLB[count + 1];
        TLBhit = 1;
        TLBhitcount++;
      }
    }
    
    // In case of TLB miss, translate to physical address manually
    if(TLBhit == 0) {
      if(VALID_TABLE[p] == 1) { // if repeating address
        paddr = 256 * COUNT_TABLE[p] + d;
      } else { // non repeating address
        paddr = (PAGE_TABLE[j]) * 256 + d;
        COUNT_TABLE[p] = j; // frame number
        j++;
      }

      // Use FIFO policy to handle TLB
      TLB[TLB_pos] = p;
      TLB[TLB_pos + 1] = paddr;
      TLB_pos = (TLB_pos + 2) % 32;
    }

    TLBhit = 0;
    VALID_TABLE[p] = 1;

    // Locate value associated with physical address in BACKING_STORE.bin
    fseek(bin, vaddr, SEEK_SET);
    fread(buffer, 1, 1, bin);

    printf("Logical Address: %d | Physical Address: %d | Value: %d | PageNum: %d | Offset: %d\n", vaddr, paddr, ((char*)buffer)[0], p, d);

    fscanf(fp, "%d", &vaddr);
    translated_count++;
  }

  for(count = 0; count < 256; count++) {
    if(VALID_TABLE[count] == 1) {
      page_faults++;
    }
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
