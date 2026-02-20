/*
 * Name: TechShell Developer
 * Description: A simple shell implementation into C.
 * Date: 2026-02-19
 */

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_INPUT_SIZE 1024
#define MAX_ARGS 64

// Structure to hold parsed command information
struct ShellCommand {
  char *command;
  char **args;
  char *input_redirect;
  char *output_redirect;
};

// Function to display the shell prompt
void displayPrompt() {
  char cwd[1024];
  if (getcwd(cwd, sizeof(cwd)) != NULL) {
    printf("%s$ ", cwd);
  } else {
    printf("Error %d (%s)\n", errno, strerror(errno));
  }
}

// Function to get input from the user
char *getInput() {
  char *input = (char *)malloc(MAX_INPUT_SIZE * sizeof(char));
  if (!input) {
    fprintf(stderr, "Allocation error\n");
    exit(1);
  }

  if (fgets(input, MAX_INPUT_SIZE, stdin) == NULL) {
    free(input);
    return NULL;
  }

  // Remove newline character at the last
  input[strcspn(input, "\n")] = 0;
  return input;
}

// Function to parse the user input into a ShellCommand structure
struct ShellCommand parseInput(char *input) {
  struct ShellCommand cmd;
  cmd.command = NULL;
  // Allocate memory for arguments array
  cmd.args = (char **)malloc(MAX_ARGS * sizeof(char *));
  cmd.input_redirect = NULL;
  cmd.output_redirect = NULL;

  if (!input || strlen(input) == 0) {
    cmd.args[0] = NULL;
    return cmd;
  }

  int arg_idx = 0;
  // Tokenize input by space
  char *token = strtok(input, " ");
  while (token != NULL) {
    if (strcmp(token, "<") == 0) {
      // Input redirection detected
      token = strtok(NULL, " ");
      if (token) {
        cmd.input_redirect = strdup(token);
      }
    } else if (strcmp(token, ">") == 0) {
      // Output redirection detected
      token = strtok(NULL, " ");
      if (token) {
        cmd.output_redirect = strdup(token);
      }
    } else {
      // Regular argument
      if (arg_idx < MAX_ARGS - 1) {
        cmd.args[arg_idx++] = strdup(token);
      }
    }
    token = strtok(NULL, " ");
  }
  cmd.args[arg_idx] = NULL;

  if (arg_idx > 0) {
    cmd.command = cmd.args[0];
  } else {
    cmd.command = NULL;
  }

  return cmd;
}

// Function to execute the parsed command
void executeCommand(struct ShellCommand cmd) {
  if (cmd.command == NULL) {
    return;
  }

  // Handle built-in command: cd
  if (strcmp(cmd.command, "cd") == 0) {
    char *path = cmd.args[1];
    if (path == NULL) {
      path = getenv("HOME");
    }

    if (path && chdir(path) != 0) {
      printf("Error %d (%s)\n", errno, strerror(errno));
    }
    return;
  }

  // Handle built-in command: exit
  if (strcmp(cmd.command, "exit") == 0) {
    exit(0);
  }

  // Handle external commands
  pid_t pid = fork();
  if (pid == -1) {
    printf("Error %d (%s)\n", errno, strerror(errno));
  } else if (pid == 0) {
    // Child process

    // Handle input redirection
    if (cmd.input_redirect) {
      int fd = open(cmd.input_redirect, O_RDONLY);
      if (fd == -1) {
        printf("Error %d (%s)\n", errno, strerror(errno));
        exit(errno);
      }
      if (dup2(fd, STDIN_FILENO) == -1) {
        printf("Error %d (%s)\n", errno, strerror(errno));
        exit(errno);
      }
      close(fd);
    }

    // Handle output redirection
    if (cmd.output_redirect) {
      // Open for writing, create if not exists, truncate if exists. Mode 0644.
      int fd = open(cmd.output_redirect, O_WRONLY | O_CREAT | O_TRUNC, 0644);
      if (fd == -1) {
        printf("Error %d (%s)\n", errno, strerror(errno));
        exit(errno);
      }
      if (dup2(fd, STDOUT_FILENO) == -1) {
        printf("Error %d (%s)\n", errno, strerror(errno));
        exit(errno);
      }
      close(fd);
    }

    // Execute command
    if (execvp(cmd.command, cmd.args) == -1) {
      printf("Error %d (%s)\n", errno, strerror(errno));
      exit(errno);
    }
  } else {
    // Parent process
    int status;
    waitpid(pid, &status, 0);
  }
}

int main() {
  char *input;
  struct ShellCommand command;

  // Change to HOME directory on start
  char *home = getenv("HOME");
  if (home != NULL) {
    if (chdir(home) != 0) {
      printf("Error starting at HOME: %d (%s)\n", errno, strerror(errno));
    }
  }

  for (;;) {
    displayPrompt();
    input = getInput();

    // Handle EOF (Ctrl+D)
    if (input == NULL) {
      printf("\n");
      break;
    }

    command = parseInput(input);
    executeCommand(command);

    // Cleanup
    free(input);
    if (command.args) {
      for (int i = 0; command.args[i] != NULL; i++) {
        free(command.args[i]);
      }
      free(command.args);
    }
    if (command.input_redirect)
      free(command.input_redirect);
    if (command.output_redirect)
      free(command.output_redirect);
  }

  return 0;
}
