/**************************************************************
 * Name:: Eduardo Munoz
 * GitHub-Name:: Smvckerz
 * Project:: Simple Shell with Pipes
 *
 * File:: Main.c
 *
 * Description:: file contains the main function for a simple shell program
 *
 **************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

//global constants
const int lineLimit = 107;
const int argCap = 7;

void closePipe(int *fd) {
    close(fd[0]);
    close(fd[1]);
}

int main(int argc, char * argv[]) 
{
  //checks to see if the entered a prompt
  char *prompt;

  printf("===========================================\n");
  printf(" Welcome to Eduardo's Simple Shell 🐚\n");
  printf("===========================================\n");
  printf("Available Commands:\n");
  printf("  • Any valid system command (e.g. ls, pwd, cat, echo)\n");
  printf("  • Commands with arguments (e.g. ls -l, echo Hello World)\n");
  printf("  • Piped commands (e.g. ls -l | grep txt)\n");
  printf("  • Type 'exit' to quit the shell\n");
  printf("Notes:\n");
  printf("  - Max 6 arguments per command (7 including command name)\n");
  printf("  - Pipes are supported between exactly 2 commands\n");
  printf("===========================================\n\n");

  //if the user entered a prompt, use it, otherwise use the default prompt
  if (argc > 1)
  {
    prompt = argv[1];
  } 
  else 
  {
    prompt = "> ";
  }

  //input buffer to store the user input
  char inputLine[lineLimit + 1];
  //while loop is infinite to keep the shell running until the user exits
  while (1) 
  {
    if (strcmp(inputLine, "help") == 0)
    {
      printf("...\n"); // reprint the welcome message
      continue;
    }
    //prompt the user for input
    printf("%s", prompt);
    //get the user input
    char *storeInput = fgets(inputLine, lineLimit + 1, stdin);

    //if fgets fails, it returns NULL, so we check for that
    if (storeInput == NULL) 
    {
      fprintf(stderr, "Reached the end of the file exiting.\n");
      exit(0); //exit the shell if fgets fails
    }

    //removes the newline character from the input
    if (strstr(storeInput, "\n") != NULL) 
    {
      *strstr(storeInput, "\n") = '\0'; //replaces the newline with a null ending character
    }

    //if the string contains exit, the program will exit
    if (strcmp(inputLine, "exit") == 0 || strcmp(inputLine, "Exit") == 0) 
    {
      exit(0);
    }

    //if the string contains a pipe, it will handle the pipe command
    char *pipeCheck = strchr(inputLine, '|');
    //if pipeCheck is not NULL, it means there is a pipe in the input
    if (pipeCheck) 
    {
      *pipeCheck = '\0'; // Terminate the string at the pipe character
      char *leftSide = inputLine; // Left side of the pipe
      char *rightSide = pipeCheck + 1; //right side of the pipe

      //keeps track of the number of pipes
      int pipeCounter;

      //stores the left and right side arguments
      //using arrays to store the arguments for the left and right side of the pipe
      char * argvleft[6];
      char * argvright[6];

      //creating count variables, for for loops
      int count, count1;

      //tokenizing the left side
      argvleft[0] = strtok(leftSide, " \t");

      //iterating through the left side arguments
      //and storing them in the argvleft array
      for (count = 1; argvleft[count - 1]; count++) 
      {
        char * t = strtok(NULL, " \t"); // tokenize the left side
        if (!t) 
        {
          //break when we reach the end of the left side arguments
          break;
        }
        argvleft[count] = t; //stores the token in the left side arguments
      }
      argvleft[count] = NULL; //null terminate the left side arguments

      argvright[0] = strtok(rightSide, " \t"); //tokenize the right side

      //same process as the above for loop, but for right side
      for (count1 = 1; argvright[count1 - 1]; count1++) 
      {
        //tokenizing the right side arguments
        char *t = strtok(NULL, " \t"); // tokenize the right side
        //if t is NULL, it means we reached the end of the right side arguments
        if (!t) 
        {
          break; // break when we reach the end of the right side arguments
        }
        argvright[count1] = t; //store the token in the right side arguments
      }
      argvright[count1] = NULL; //null terminate the right side arguments
      
      //holds the file descriptors for the pipe
      int fd[2];

      //creating a pipe to connect the two processes
      if (pipe(fd) < 0) 
      {
        fprintf(stderr, "Fork failed.\n");
        continue;
      }

      //creating two child processes to execute the left and right side commands
      pid_t pid1 = fork();

      //if fork fails, it returns a negative value
      if (pid1 < 0) //first child pid
      {
        fprintf(stderr, "Fork failed.\n");
        close(fd[0]); //close the read end of the pipe
        close(fd[1]); //close the write end of the pipe
        continue;
      }

      //if fork is successful, the child process will execute the left side command
      if (pid1 == 0) //Child process
      {
        dup2(fd[1], 1); //redirect stdout to the write end of the pipe
        close(fd[0]); //close the read end of the pipe in parent
        close(fd[1]); //close the write end of the pipe in parent
        execvp(argvleft[0], argvleft); //execute the left side command
        printf("execvp failed");
        exit(1);
      }

      if (strlen(leftSide) == 0 || strlen(rightSide) == 0)
      {
        fprintf(stderr, "Error: Incomplete piped command.\n");
        continue;
      }

      //if fork is successful, the parent process will execute the right side command
      pid_t pid2 = fork();
      if (pid2 < 0) //second child
      {
        fprintf(stderr, "Fork failed.\n");
        close(fd[0]); //close the read end of the pipe
        close(fd[1]); //close the write end of the pipe
        continue;
      }

      //if fork is successful, the child process will execute the right side command
      if (pid2 == 0) // child
      {
        dup2(fd[0], STDIN_FILENO); //redirect stdin to the read end of the pipe
        close(fd[0]); //close the read end of the pipe in child
        close(fd[1]); //close the write end of the pipe in child
        execvp(argvright[0], argvright); //execute the right side command
        printf("execvp failed");
        exit(1);
      }

      //if fork is successful, the parent process will wait for both child processes to finish
      if (pid2 > 0) // parent
      {
        close(fd[0]); //close the read end of the pipe in parent
        close(fd[1]); //close the write end of the pipe in parent
        int status1, status2;
        waitpid(pid1, &status1, 0); //wait for the first child to finish
        printf("Child %d exited with %d \n", pid1, WEXITSTATUS(status1));
        waitpid(pid2, &status2, 0); //wait for the second child to finish
        printf("Child %d exited with %d \n", pid2, WEXITSTATUS(status2));
        continue; //continue to the next iteration of the loop
      }
      continue;
    }

    //if there is no pipe, we will execute the command normally
    //creating an array to store the arguments
    char *args[argCap];
    int argCount = 0;

    //tokenizing the input line using strtok
    // \t takes care of both spaces and tabs
    char *token = strtok(inputLine, " \t");

    int tooMany = 0;

    //tokenizing the input line and storing the arguments in the args array
    while (token != NULL) 
    {
      //if the user enters more than 6 arguments, print an error message and continue to the next iteration
      if (argCount >= argCap - 1) 
      {
        //consume the rest of the input
        //does this work? 
        while ((token = strtok(NULL, " \t")) != NULL)
        {

        } 
        fprintf(stderr, "Too many arguments.\n");
        tooMany = 1;
        break;
      }

      args[argCount] = token;
      argCount++;

      token = strtok(NULL, " \t");
    }
    //null terminate the args array
    args[argCount] = NULL;

    //if the user entered too many arguments, print an error message and continue to the next iteration
    if (tooMany == 1) 
    {
      continue;
    }

    //if the user did not enter any arguments, print an error message and continue to the next iteration
    if (argCount == 0) 
    {
      printf("Empty line, please try again.\n");
      continue;
    }

    //creating a child process to execute the command
    pid_t pid = fork();

    //if fork fails, print an error message and continue to the next iteration
    if (pid < 0) //fork fails
    {
      fprintf(stderr, "Fork failed.\n");
      continue;
    }

    //if fork is successful, the child process will execute the command
    if (pid == 0) //child
    {
      execvp(args[0], args);
      fprintf(stderr, "Command not found: %s\n", args[0]);
      exit(1);
    }

    //if fork is successful, the parent process will wait for the child process to finish
    if (pid > 0) //parent
    {
      int waiting;
      waitpid(pid, &waiting, 0); //wait for the child process to finish

      //test function to check if the child process terminated normally
      if (WIFEXITED(waiting)) 
      {
        //if the child process terminated normally, get the exit status
        int exitStatus = WEXITSTATUS(waiting);
        printf("Child %d exited with %d \n", pid, exitStatus);
      } 
      else 
      {
        fprintf(stderr, "Command did not terminate normally.\n");
      }
    }
  }
}
