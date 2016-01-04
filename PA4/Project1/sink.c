#include <stdio.h>
#include <windows.h>

#define BUFFER_SIZE 25

int main(VOID)
{
    HANDLE ReadHandle, WriteHandle, hPipe; 
    CHAR buffer[BUFFER_SIZE];
    char pipeName[] = "\\\\.\\pipe\\proj1";
    DWORD read;
    
    //ReadHandle = GetStdHandle(STD_INPUT_HANDLE);
    //WriteHandle= GetStdHandle(STD_OUTPUT_HANDLE);
    hPipe = CreateNamedPipe(
			pipeName,
			PIPE_ACCESS_DUPLEX,       // read/write access 
			PIPE_TYPE_MESSAGE |       // message type pipe 
			PIPE_READMODE_MESSAGE |   // message-read mode 
			PIPE_WAIT,                // blocking mode 
			PIPE_UNLIMITED_INSTANCES, // max. instances  
			BUFFER_SIZE,                  // output buffer size 
			BUFFER_SIZE,                  // input buffer size 
			0,                        // client time-out 
			NULL);
	if (hPipe == INVALID_HANDLE_VALUE) {
		fprintf(stderr, "CreateNamedPipe Failed\n");
		return 1;
	}

	ConnectNamedPipe(hPipe, NULL);

    /* read from the pipe */
    if (ReadFile(hPipe, buffer, BUFFER_SIZE, &read, NULL))
       printf("sink reads from named pipe: >%s< \n",buffer);
    else
       fprintf(stderr, "sink: Error reading from named pipe %d \n",GetLastError());
    
    return 0;
}
