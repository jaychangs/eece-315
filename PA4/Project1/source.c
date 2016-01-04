#include <stdio.h>
#include <windows.h>
#include <stdlib.h>

#define BUFFER_SIZE 25

int main(VOID)
{
	HANDLE ReadHandle, WriteHandle;
	DWORD written;
	char buffer[BUFFER_SIZE] =  "eece315 project 1";

	ReadHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	WriteHandle= GetStdHandle(STD_INPUT_HANDLE);

	/* writes to the pipe */
	if (WriteFile(WriteHandle, buffer,BUFFER_SIZE,&written,NULL))
		printf("source writes to pipe: >%s< \n",buffer);
	else
		fprintf(stderr, "Error writing to pipe.\n");

	return 0;
}
