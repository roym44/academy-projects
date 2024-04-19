# Introduction
An assignment given as part of the Operating Systems (fall 2024) course - implementing a mini shell.

The goal is to gain experience with process management, pipes, signals, and the relevant system calls. We implement a simple shell program: the function that receives a shell command and performs it. 
shell.c is a skeleton shell program which reads lines from the user, parses them into commands, and invokes the implemented function. 

## The shell skeleton
The skeleton executes an infinite loop. Each iteration does the following:
1. Reads a string containing a shell command from standard input. If Ctrl-D is pressed, the skeleton
exits.
2. Parses the command string into an array of words. A word is a non-empty sequence of nonwhitespace
characters, where whitespace means space, tab (\t), or newline (\n). The end of the
array is designated with a NULL entry.
3. Invokes your function, passing it the parsed command line array. The skeleton detects and ignores
empty lines (i.e., won’t invoke your function on an empty command).

## Shell functionality
The shell supports the following operations:
1. **Executing commands**: The user enters a command, i.e., a program and its arguments, such as
```sleep 10```. The shell executes the command and waits until it completes before accepting another
command.
2. **Executing commands in the background**: The user enters a command followed by &, for
example: ```sleep 10 &```. The shell executes the command but does not wait for its completion
before accepting another command.
3. **Single piping**: The user enters two commands separated by a pipe symbol (|), for example:
```cat foo.txt | grep bar```. The shell executes both commands concurrently, piping the standard
output of the first command to the standard input of the second command. The shell waits until
both commands complete before accepting another command.
4. **Input redirecting**: The user enters one command and input file name separated by the
redirection symbol (<), for example: ```cat < file.txt```. The shell executes the command so that
its standard input is redirected from the input file (instead of the default, which is to the user’s
terminal). The shell waits for the command to complete before accepting another command.
5. **Output redirecting**: The user enters one command and output file name separated by the
redirection symbol (>), for example: ```cat foo > file.txt```. The shell executes the command
so that its standard output is redirected to the output file (instead of the default, which is to
the user’s terminal). If the specified output file does not exist, it is created. If it exists, it is
overwritten.\
The shell waits for the command to complete before accepting another command.
By default, stdout and stderr are printed to your terminal. But we can redirect that output to a
file using the (>) operator. The > file.txt does two things: A) It creates a file named “file” if
it does not exist, and B) it replaces the content of “file” with new contents.
The shell doesn’t need to support built-in commands such as cd and exit. It only support
execution of program binaries as described above.

# Documentation
We implement the following functions in myshell.c: prepare(), process_arglist(), and finalize().

## prepare
```int prepare(void)```\
The skeleton calls this function before the first invocation of ```process_arglist()```. This function
returns 0 on success; any other return value indicates an error.
You can use this function for any initialization and setup that you think are necessary for your
process_arglist(). If you don’t need any initialization, just have this function return immediately;
but you must provide it for the skeleton to compile.


## process_arglist
```int process_arglist(int count, char **arglist)```\
**Input**:\
This function receives an array arglist with count non-NULL words. This array contains
the parsed command line. The last entry in the array, arglist[count], is NULL. (So overall the
array size is count+1.)

**Behavior**:
1. Commands specified in the arglist are executed as a child process using fork() and
execvp(). Notice that if the arglist array contains a shell symbol (&, | < or >),
the arglist should not be passed to execvp() as-is.
2. Executing commands in the background:
    - If the last non-NULL word in arglist is "&" (a single ampersand), we run the child process
    in the background. The parent does not wait for the child process to finish, but instead continue executing commands.
    - We do not pass the & argument to execvp().
    - We assume background processes don’t read input (stdin).
    - We only use a single call to fork.
3. Piping:
    - If arglist contains the word "|" (a single pipe symbol), run two child processes, with the
    output (stdout) of the first process (executing the command that appears before the pipe) piped to the input (stdin) of the second process (executing the command that appears after the pipe).
    - To pipe the child processes input and output, use the pipe() and dup2() system calls.
    - Use the same array for all execvp() calls by referencing items in arglist. There’s no need to allocate a new array and duplicate parts of the original array.
4. Input redirection: If arglist contains the word “<” (a single redirection symbol), we open the specified file (that appears after the redirection symbol) and then run the child process, with the input (stdin) redirected from the input file.
5. Output redirection: If arglist contains the word “>” (a single redirection symbol), we open the specified file (that appears after the redirection symbol) and then run the child process, with the output (stdout) redirected to the output file.
6. Handling of SIGINT:
    - After prepare() finishes, the parent (shell) does not terminate upon SIGINT.
    - Foreground child processes (regular commands or parts of a pipe) terminate upon
    SIGINT.
    - Background child processes do not terminate upon SIGINT.

**Output**:\
The process_arglist() function does not return until every foreground child process it created exits.\
In the original (shell/parent) process, process_arglist() returns 1 if no error occurs. (This makes sure the shell continues processing user commands.) If process_arglist() encounters an error, it prints an error message and return 0.


## finalize
```int finalize(void)```:\
The skeleton calls this function before exiting. This function returns 0 on success; any other return value indicates an error.


# Usage
```
gcc -O3 -D_POSIX_C_SOURCE=200809 -Wall -std=c11 shell.c myshell.c
```