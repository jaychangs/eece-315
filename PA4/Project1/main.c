#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#define BUFFER_SIZE 25

int main(VOID)
{
	HANDLE ReadHandle, WriteHandle;
	HANDLE hPipe;
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	char buffer[BUFFER_SIZE];
	char pipeName[] = "\\\\.\\pipe\\proj1";

	/* set up security attributes allowing pipes to be inherited */
	SECURITY_ATTRIBUTES sa = {sizeof(SECURITY_ATTRIBUTES),NULL,TRUE};

	/* allocate memory */
	ZeroMemory(&pi, sizeof(pi));

	/* create the pipe */
	if (!CreatePipe(&ReadHandle, &WriteHandle, &sa, 0)) {
		fprintf(stderr, "Create Pipe Failed\n");
		return 1;
	}

	 /* establish the START_INFO structure for the child process */
	GetStartupInfo(&si);
	si.hStdError = GetStdHandle(STD_ERROR_HANDLE);
	si.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);

	/* redirect the standard input to the write end of the pipe */
	si.hStdInput = WriteHandle;
	si.dwFlags = STARTF_USESTDHANDLES;

	if (CreateProcess(NULL, 
                     "source.exe",
                     NULL,
                     NULL,
                     TRUE, /* inherit handles */
                     0,
                     NULL,
                     NULL,
                     &si,
                     &pi))
	{
	 printf("source process created\n");
	}

	else{
    		fprintf(stderr, "source Process Creation Failed\n");
    		return -1;
	}

	if (CreateProcess(NULL, 
                     "sink.exe",
                     NULL,
                     NULL,
                     TRUE, /* inherit handles */
                     0,
                     NULL,
                     NULL,
                     &si,
                     &pi))
	{
	 printf("sink process created\n");
	}

	else{
    		fprintf(stderr, "sink Process Creation Failed\n");
    		return -1;
	}

	/* redirect the standard input to the read end of the pipe */
	si.hStdInput = ReadHandle;
	si.dwFlags = STARTF_USESTDHANDLES;

	if (CreateProcess(NULL, 
                     "filter.exe",
                     NULL,
                     NULL,
                     TRUE, // inherit handles 
                     0,
                     NULL,
                     NULL,
                     &si,
                     &pi))
	{
	 printf("filter process created\n");
	}

	else{
     		fprintf(stderr, "filter Process Creation Failed\n");
    		return -1;
	}
	
	/* wait for the child to exit */
	WaitForSingleObject(pi.hProcess, INFINITE);

	/*close all handles*/
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	return 0;
}
