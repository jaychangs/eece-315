#include <windows.h>
#include <stdio.h>
//code from pg.175
DWORD fibarray[256]; /* data is shared by the thread(s) */

/* the thread runs in this separate function */
DWORD WINAPI Fibonacci(LPVOID Param)
{
	
	DWORD n = *(DWORD*)Param;
	DWORD i, next;
	DWORD t1 = 0;
	DWORD t2 = 1;
	
	fibarray[0] = 0;
	fibarray[1] = 1;
	for (i = 2; i <= n; i++){
		next = t1 + t2;
		t1 = t2;
		t2 = next;
		fibarray[i] = next;
	}
	return 0;
}


int main(int argc, char *argv[])
{
	DWORD ThreadId;
	HANDLE ThreadHandle;
	int Param;
	
	if (argc != 2) {
		fprintf(stderr,"An integer parameter is required\n");
		return -1;
	}
	Param = atoi(argv[1]);
	if (Param < 0) {
		fprintf(stderr,"An integer >= 0 is required\n");
		return -1;
	}

	/* create the thread */
	ThreadHandle = CreateThread(
		NULL, /* default security attributes */
		0, /* default stack size */
		Fibonacci, /* thread function */
		&Param, /* parameter to thread function */
		0, /* default creation flags */
		&ThreadId); /* returns the thread identifier */

	if (ThreadHandle != NULL) {
		/* now wait for the thread to finish */
		WaitForSingleObject(ThreadHandle,INFINITE);

		/* close the thread handle */
		CloseHandle(ThreadHandle);
		
		int i;
		for(i = 0; i < Param ; i++) {
        		printf("%d ", fibarray[i]);
    		}
    		printf("\n");

	}
}
