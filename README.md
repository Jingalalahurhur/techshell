# TechShell

A simple shell implementation in C, with basic Bash functionality.

## 1. Group Members
*   **Anup Paudel** 

## 2. Responsibilities
As the only member of this project, I was responsible for all aspects of the development:
*   **Design**: Planning the architecture including the `ShellCommand` structure and function breakdown.
*   **Implementation**: Writing the C code for parsing, execution (`fork`, `execvp`), I/O redirection (`dup2`), and built-in commands (`cd`, `exit`).
*   **Documentation**: Writing the README, code comments, and detailed project description.

## 3. Project Description
TechShell is a custom command line interface that allows users to interact with the operating system. It operates in a continuous loop:
1.  **Displays a prompt** showing the current working directory.
2.  **Reads user input** and parses it into commands and arguments.
3.  **Executes commands**:
    *   **Built-ins**: Commands like `cd` and `exit` are handled internally by the shell.
    *   **External Programs**: Commands like `ls`, `grep`, or `python` are executed by creating new processes using `fork()` and `execvp()`.
    *   **Redirection**: Supports input (`<`) and output (`>`) redirection to read from or write to files.
4.  **Handles Errors**: Displays structured error messages (e.g., `Error 2 (No such file or directory)`) using `errno`.

## 4. Instructions

### Compilation
To compile the program, run the following command in your terminal:
```bash
gcc -o techshell techshell.c
```

### Running the Shell
To start the shell, run:
```bash
./techshell
```

### Usage
Once inside TechShell:
*   **Run commands**: `ls -l`, `pwd`, `date`
*   **Change directory**: `cd /path/to/dir` or `cd` (to go home)
*   **Redirect Output**: `ls > output.txt`
*   **Redirect Input**: `wc -l < input.txt`
*   **Exit**: Type `exit` to close the shell.

## 5. Implementation Status
All required features have been fully implemented and verified:

