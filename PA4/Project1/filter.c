#include <stdio.h>
#include <windows.h>
#include <stdlib.h>

#define BUFFER_SIZE 25

int main(VOID)
{
    HANDLE ReadHandle, WriteHandle, hPipe;
    CHAR buffer[BUFFER_SIZE];
    CHAR filtered[BUFFER_SIZE];
    char pipeName[] = "\\\\.\\pipe\\proj1";
    DWORD read;
    DWORD written;
    int i;
    SECURITY_ATTRIBUTES sa = {sizeof(SECURITY_ATTRIBUTES),NULL,TRUE};
    
    ReadHandle = GetStdHandle(STD_INPUT_HANDLE);
    //WriteHandle= GetStdHandle(STD_OUTPUT_HANDLE);
    
    hPipe = CreateFile(
			pipeName, 
			GENERIC_READ | GENERIC_WRITE, 
			FILE_SHARE_READ | FILE_SHARE_WRITE, 
			NULL, 
			OPEN_EXISTING, 
			0, 
			NULL
			);
			
	if (hPipe == INVALID_HANDLE_VALUE) {
		fprintf(stderr, "CreateFile Failed\n");
		return 1;
	}

	if (ReadFile(ReadHandle, buffer, BUFFER_SIZE, &read, NULL))
    	printf("filter reads: >%s<\n",buffer);
	else
    	fprintf(stderr, "filter: Error reading from pipe\n");
	
	for(i=0; i<BUFFER_SIZE; i++)
   	{
      //printf("%c\n",toupper(buffer[i]));
		if(islower(buffer[i]))
			filtered[i] = toupper(buffer[i]);
		else
			filtered[i] = tolower(buffer[i]);
   	}
	printf("Filter: lowercase to uppper case:   %s \n", filtered );
	CloseHandle(ReadHandle);

	ConnectNamedPipe(hPipe, NULL);
	/* writes to the pipe */
	if (WriteFile(hPipe, filtered,BUFFER_SIZE,&written,NULL))
		printf("filter writes to named pipe: >%s< \n",filtered);
	else
		fprintf(stderr, "Error writing to named pipe.\n");
	
	/* close pipe */
	CloseHandle(hPipe);

	return 0;
}
