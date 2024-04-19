#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <wait.h>
#include <fcntl.h>

/* MACROS */
#define PROCESS_FAILURE (0)
#define PROCESS_SUCCESS (1)
#define BACKGROUND_SYMBOL ("&")
#define PIPE_SYMBOL ("|")
#define INPUT_REDIRECT_SYMBOL ("<")
#define OUTPUT_REDIRECT_SYMBOL (">")

/* FUNCTIONS */
int prepare(void);
void signal_handling(void (*handler)(int), int flags, int signal);
int process_arglist(int count, char** arglist);
int find_symbol_index(int count, char** arglist, char* symbol);
int execute_foreground(char** arglist); // e.g. (sleep 10)
int execute_background(char** arglist); // e.g. (sleep 10 &)
int execute_pipe(char** arglist, int pipe_index); // e.g. (cat foo.txt | grep bar)
int execute_redirect(char** arglist, int redirect_index); // e.g. (cat < file.txt), (cat foo > file.txt)
int finalize(void);


int prepare(void)
{
    // ignore SIGINT, allow restarting handler
    signal_handling(SIG_IGN, SA_RESTART, SIGINT);
    // ignore SIGCHLD to deal with zombies (note: creates some problems when running directly on mac)
    signal_handling(SIG_IGN, 0, SIGCHLD);
    return EXIT_SUCCESS;
}

// setting a signal handler for the given signal with the given flags
void signal_handling(void (*handler)(int), int flags, int signal)
{
    struct sigaction sa = {};
    sa.sa_handler = handler; 
    sa.sa_flags = flags; 
    if (-1 == sigaction(signal, &sa, NULL)) {  
        perror("sigaction failed");
        exit(EXIT_FAILURE);
    }
}

int process_arglist(int count, char** arglist)
{
    int result = 0;
    int symbol_index = -1;

    // can't contain one of {<, >, |, &}
    if (count < 2) {
        result = execute_foreground(arglist);
        return result;
    }

    // handle background process (&)
    symbol_index = count - 1; // appears last
    if (0 == strcmp(arglist[symbol_index], BACKGROUND_SYMBOL)) { 
        arglist[symbol_index] = NULL; // remove '&' from the arglist
        result = execute_background(arglist);
    }
    else {
        // handle redirection (input <, output >)
        symbol_index = count - 2; // appears before last
        if (0 == strcmp(arglist[symbol_index], INPUT_REDIRECT_SYMBOL) || 
            0 == strcmp(arglist[symbol_index], OUTPUT_REDIRECT_SYMBOL)) {
            // don't remove '<' or '>' from the arglist
            result = execute_redirect(arglist, symbol_index);
        }
        else {
            // handle piping (|)
            symbol_index = find_symbol_index(count, arglist, PIPE_SYMBOL);
            if (-1 != symbol_index) {
                arglist[symbol_index] = NULL; // remove '|' from the arglist
                result = execute_pipe(arglist, symbol_index);
            }
            // handle foreground process (last option)
            else 
                result = execute_foreground(arglist);
        }
    }
    return result;
}

// get the index of th given symbol in the arglist
int find_symbol_index(int count, char** arglist, char* symbol)
{
    int i = 0;
    for (i = 0; i < count; ++i)
        if (0 == strcmp(arglist[i], symbol))
            return i;
    return -1;
}

int execute_foreground(char** arglist)
{
    pid_t pid = 0;

    pid = fork();
    switch (pid) {
        case -1: // fork failed
            perror("fork failed");
            return PROCESS_FAILURE;
        case 0: // child
            signal_handling(SIG_DFL, SA_RESTART, SIGINT); // reset SIGINT to default
            if (-1 == execvp(arglist[0], arglist)) {
                perror("execvp failed");
                exit(EXIT_FAILURE);
            }
        default: // parent - waits for child process to finish
            waitpid(pid, NULL, 0); // we will get SIGCHLD on finish, which we ignore thus the child dies
            break;
    }

    return PROCESS_SUCCESS;
}

int execute_background(char** arglist)
{
    pid_t pid = 0;

    pid = fork();
    switch (pid) {
        case -1: // fork failed
            perror("fork failed");
            return PROCESS_FAILURE;
        case 0: // child
            if (-1 == execvp(arglist[0], arglist)) {
                perror("execvp failed");
                exit(EXIT_FAILURE);
            }
        default: // parent - should not wait for child process to finish
            break;
    }

    return PROCESS_SUCCESS;
}

int execute_pipe(char** arglist, int pipe_index)
{
    int pfds[2] = {};
    pid_t pid1 = 0;
    pid_t pid2 = 0;

    if (-1 == pipe(pfds)) {
        perror("pipe failed");
        return PROCESS_FAILURE;
    }

    pid1 = fork();
    switch (pid1) {
        case -1: // fork failed
            perror("fork failed");
            return PROCESS_FAILURE;
        case 0: // child (writer)
            signal_handling(SIG_DFL, SA_RESTART, SIGINT); // reset SIGINT to default
            close(pfds[0]); // close the read end of the pipe
            // redirect stdout to the write end of the pipe
            if (-1 == dup2(pfds[1], STDOUT_FILENO)){
                perror("dup2 failed");
                exit(EXIT_FAILURE);
            } 
            close(pfds[1]); // close the write end 
            if (-1 == execvp(arglist[0], arglist)) {
                perror("execvp failed");
                exit(EXIT_FAILURE);
            }
        default: // parent
            pid2 = fork();
            switch (pid2) {
                case -1: // fork failed
                    perror("fork failed");
                    return PROCESS_FAILURE;
                case 0: // child (reader)
                    close(pfds[1]); // close the write end 
                    // redirect stdin to the read end 
                    if (-1 == dup2(pfds[0], STDIN_FILENO)){
                        perror("dup2 failed");
                        exit(EXIT_FAILURE);
                    } 
                    close(pfds[0]); // close the read end 
                    if (-1 == execvp(arglist[pipe_index + 1], &arglist[pipe_index + 1])) {
                        perror("execvp failed");
                        exit(EXIT_FAILURE);
                    }
                default: // parent
                    close(pfds[0]); // close the read end
                    close(pfds[1]); // close the write end
                    waitpid(pid1, NULL, 0); // wait for the first child to finish
                    waitpid(pid2, NULL, 0); // wait for the second child to finish
                    break;
            }
            break;
    }

    return PROCESS_SUCCESS;
}

int execute_redirect(char** arglist, int redirect_index)
{
    pid_t pid = 0;
    int fd = 0;

    pid = fork();
    switch (pid) {
        case -1: // fork failed
            perror("fork failed");
            return PROCESS_FAILURE;
        case 0: // child
            signal_handling(SIG_DFL, SA_RESTART, SIGINT); // reset SIGINT to default
            // handle input redirection
            if (0 == strcmp(arglist[redirect_index], INPUT_REDIRECT_SYMBOL)) {
                fd = open(arglist[redirect_index + 1], O_RDONLY);
                if (-1 == fd) {
                    perror("open failed");
                    exit(EXIT_FAILURE);
                }
                if (-1 == dup2(fd, STDIN_FILENO)){
                    perror("dup2 failed");
                    exit(EXIT_FAILURE);
                } 
            }

            // handle output redirection
            else if (0 == strcmp(arglist[redirect_index], OUTPUT_REDIRECT_SYMBOL)) {
                // open a file if it exists (and overwrite it), or else create it, with read/write permissions
                fd = open(arglist[redirect_index + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if (-1 == fd) {
                    perror("open failed");
                    exit(EXIT_FAILURE);
                }
                if (-1 == dup2(fd, STDOUT_FILENO)){
                    perror("dup2 failed");
                    exit(EXIT_FAILURE);
                } 
            }
            close(fd);
            arglist[redirect_index] = NULL; // remove '<' or '>' from the arglist
            if (-1 == execvp(arglist[0], arglist)) {
                perror("execvp failed");
                exit(EXIT_FAILURE);
            }
        default: // parent
            waitpid(pid, NULL, 0); // wait for the child to finish
            break;
    }

    return PROCESS_SUCCESS;
}

int finalize(void)
{
    return EXIT_SUCCESS;
}