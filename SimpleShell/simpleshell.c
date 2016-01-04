#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "unistd.h"
#include "sys/types.h"
#include "sys/wait.h"
#include <sys/stat.h>
#include <fcntl.h>

#define MAX_LINE_LEN 80
#define MAX_ARGS 64
#define MAX_ARG_LEN 16
#define MAX_PATHS 64
#define MAX_PATH_LEN 96
#define WHITESPACE " ,\t\n"

#ifndef NULL
#define NULL ...
#endif

struct command_t {
  char *name;
  int argc;
  char *argv[MAX_ARGS];
};

char *lookupPath(char**, char**);
int parseCommand(char*, struct command_t*);
int parsePath(char* dirs[]);
void printPrompt();
void readCommand(char*);
void change_directory(struct command_t*);
void in_out(struct command_t*);

int main(int argc, char *argv[]) {
  int i;
  int pid, numChildren;
  int status, isParallel;
  size_t nbytes = 100;
  char *user_input = (char *)malloc(nbytes+1);
  char **pathv; 
  struct command_t command;

  pathv = (char**)malloc ((MAX_ARGS) * sizeof(char*));
  for(i = 0; i < MAX_ARGS; i++){
    pathv[i] = (char*) malloc((MAX_ARG_LEN+1)*sizeof(char));
  }

  parsePath(pathv);
  // Get directory paths from PATH
  // Process each command
  numChildren = 0;
  while (1) {
    printPrompt();
    getline(&user_input, &nbytes, stdin);
    parseCommand(user_input, &command);
    command.argv[command.argc] = NULL;

    // Check if last char entered by user is a '&'
    if (command.name[strlen(command.name)-1] == '&') {
      isParallel = 1;
    } else {
      isParallel = 0;
    }

    // Check if the second argument entered is '>'
    if (command.argv[1] != NULL){
      if (command.argv[1][0] == '>'){
        in_out(&command);
      }
    }
    
    // Exceptions to the execv() call that we implemented ourselves
    if (strcmp(command.name, "cd") == 0) {
      change_directory(&command);
     }
     else if (strcmp(command.name, "ls") == 0) {
      system(command.name);
     }
     else if (strcmp(command.name, "pwd") == 0) {
      system(command.name);
     }
     else if (strcmp(command.name, "echo") == 0) {
      system(command.name);
    } else if (strcmp(command.name, "exit") == 0) {
      kill(pid, SIGKILL);
      exit(status);
    } else if (strcmp(command.name, "quit") == 0) {
      kill(1, SIGKILL);
      exit(status);
    } else {
      command.name = lookupPath(command.argv,pathv);
      if (strcmp(command.name, "") == 0){
        printf("Command not found\n");
        continue;
      }
    }
    // Create a child process to execute the command
    if ((pid = fork()) == 0) {
      // Child executing command
      execv(command.name, command.argv);
    }
    // Parent continuing to the next command in the file
    numChildren++;
  
    // Terminate after all the children have terminated
    for (i=0; i<numChildren; i++) {
      if (isParallel != 1) {
        wait(&status);
      }
    }
  }
  return 0;
}

int parseCommand (char *cLine, struct command_t *cmd) {
  int argc;
  char **clPtr;
  // Initialization
  clPtr = &cLine; // cLine is the command line
  argc = 0;
  cmd->argv[argc] = (char *)malloc(MAX_ARG_LEN);

  // Fill argv[]
  while ((cmd->argv[argc] = strsep(clPtr, WHITESPACE)) != NULL) {
    cmd->argv[++argc] = (char *)malloc(MAX_ARG_LEN);
  }
  
  // Set the command name and argc
  cmd->argc = argc -1;
  cmd->name = (char *)malloc(sizeof(cmd->argv[0]));
  strcpy(cmd->name, cmd->argv[0]);
  return 1;
}

void printPrompt() {
  // Building the prompt string to include machine name, current directory...etc.

  char promptString[MAX_LINE_LEN];
  char* userid = getlogin();
  char hostname[32];
  gethostname(hostname, 32);

  getcwd(promptString, sizeof(promptString));
  printf("%s@", userid);
  printf("%s:~%s$ ", hostname, promptString);
}

int parsePath(char *dirs[]) {
  // This function reads the PATH variable for this environment, then builds an
  // array, dirs[], of the directories in PATH
  char *pathEnvVar;
  char *thePath;
  char temp = ':';
  int i;
  for (i=0; i<MAX_ARGS; i++) { 
    dirs[i] = NULL;
  }

  pathEnvVar = (char *)getenv("PATH");
  thePath = (char *)malloc(strlen(pathEnvVar)+1);
  strcpy(thePath, pathEnvVar);

  dirs[0] = strtok(thePath, &temp);
  for(i = 1; i < MAX_ARGS - 1; i++){
    dirs[i] = strtok(NULL, &temp);
    if(dirs[i] == NULL){
      break;
    }
  }
  return 1;
}

char *lookupPath(char **argv, char **dir) {
  // This function searches the directories identified by the dir argument
  // to see if argv[0] (the file name) appears there.
  // Allocate a new string, place the full path name in it, then return the string.
  char *result = (char*) malloc(strlen(*argv) + strlen(*dir) + 1);
  int i;

  // Check to see if filename is already an absolute path name
  if(*argv[0] == '/'){
    strcpy(result, *argv);
    if(access(result, F_OK) == 0){
    	return result;
    }
  }

  char *slash = (char*) malloc(1);
  *slash = '/';

  // Look in PATH Directories, use access() to see if the file is in a directory
  for( i = 0; i < sizeof(dir); i++){
    strcpy(result, dir[i]);
    strcat(result, slash);
    strcat(result, argv[0]);
    if(access(result, F_OK) == 0){
   	  free(slash);
      return result;
    }
  }
  free(slash);
  result = argv[0];
  return result;
}

void change_directory(struct command_t *cmd) {

  char cwd[MAX_PATH_LEN];
  int i;

  // Calling "cd" takes you to the home directory
  if (cmd->argv[1] == NULL) {
    chdir("/home");
    return;
  }

  // Calling "cd ~" takes you to the home directory
  if (strcmp(cmd->argv[1], "~") == 0) {
    chdir("/home");
    return;
  }

  // Calling "cd ." doesn't take you anywhere
  if (strcmp(cmd->argv[1], ".") == 0) {
    return;
  }

  // Calling "cd .." takes you up one directory
  if (strcmp(cmd->argv[1], "..") == 0) {
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
    } else {
      printf("Error locating cwd\n");
    }

    for (i=MAX_PATH_LEN; i>=0; i--) {
      if (cwd[i] != '/') {
        cwd[i] = '\0';
      } else {
        break;
      }
    }
    chdir(cwd);
    return;
  }


  // Calling "cd xxx" takes you there
  // Calling "cd /xxx/xxx/xxx" takes you there (absolute pathing)
  // Calling "cd /../../xxx" takes you there (relative pathing)
  if (chdir(cmd->argv[1]) != 0){
    printf("cd: ");
    for(i = 0; i < MAX_ARGS; i++){
      printf("%c", (cmd->argv[1])[i]);
    }

    printf(": No such file or directory \n");
    return;

  } else {
    return;
  }
}

void in_out(struct command_t *cmd){
  if(cmd->argv[2] == NULL) {
    printf("No output file specified\n");
    return;
  }
  char buff[100];
  FILE *fp = popen(cmd->name,"r");
  FILE *out = fopen(cmd->argv[2], "w");
  while(fgets(buff, 99, fp) != NULL){
    fprintf(out,buff);
  }		    
  fclose(out);
  pclose(fp);
  
}
