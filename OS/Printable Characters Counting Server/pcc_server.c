#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <signal.h>

/* MACROS */
#define INVALID_RESULT (-1) 
#define BUFFER_SIZE (100000)
#define LISTEN_QUEUE_SIZE (10)
#define PRINTABLE_LOWER_VALUE (32)
#define PRINTABLE_UPPER_VALUE (126)
#define PRINTABLE_COUNT_SIZE (PRINTABLE_UPPER_VALUE - PRINTABLE_LOWER_VALUE + 1)
#define KNOWN_TCP_ERROR() (errno == ETIMEDOUT || errno == ECONNRESET || errno == EPIPE)

/* TYPEDEFS */
typedef enum _SERVER_ARGS
{
	SERVER_ARGS_SELF = 0,
	SERVER_ARGS_SERVER_PORT,	    // the server's port

	SERVER_ARGS_COUNT // must be last
} SERVER_ARGS;

/* FUNCTIONS */
void signal_handling(void (*handler)(int), int flags, int signal); // setting a signal handler for the given signal with the given flags
void sig_terminate(int sig); // handle SIGINT by terminating
void terminate(); // terminate the server and print output
void handle_client(int listen_fd); // handle a connection

/* GLOBALS */
static int pcc_total[PRINTABLE_COUNT_SIZE] = {0}; // how many times each printable char was observed
static int conn_fd = -1; // client connection socket
static int sigint_flag = 0; // flag for termination
static int listen_fd = -1;

int main(int argc, char const *argv[])
{
    int optval = 1;
    struct sockaddr_in server = {};

    // validate arguments
    if (argc != SERVER_ARGS_COUNT) 
    {
        perror("pcc_server(): invalid number of arguments\n");
        return EXIT_FAILURE;
    }

    // set SIGINT handling
    signal_handling(&sig_terminate, SA_RESTART, SIGINT);

    // create a socket
    listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd < 0)
    {
        perror("pcc_server(): could not create socket\n");
        return EXIT_FAILURE;
    }

    // enable rusing the port quickly after server terminates
    if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0)
    {
        perror("pcc_server(): failed to set SO_REUSEADDR\n");
        return EXIT_FAILURE;
    }

    // set up server - bind and listen
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(atoi(argv[SERVER_ARGS_SERVER_PORT]));

    if (bind(listen_fd, (struct sockaddr *)&server, sizeof(server)) != 0) 
    {
        perror("pcc_server(): bind failed\n");
        return EXIT_FAILURE;
    }

    if (listen(listen_fd, LISTEN_QUEUE_SIZE) != 0) 
    {
        perror("pcc_server(): listen failed\n");
        return EXIT_FAILURE;
    }

    while (1)
    {
        if (sigint_flag == 1)
            break;
            
        handle_client(listen_fd);
    }

    terminate();

    return EXIT_SUCCESS;
}

void signal_handling(void (*handler)(int), int flags, int signal)
{
    struct sigaction sa = {};
    sa.sa_handler = handler; 
    sa.sa_flags = flags; 
    if (-1 == sigaction(signal, &sa, NULL)) {  
        perror("signal_handling(): sigaction failed");
        exit(EXIT_FAILURE);
    }
}

void sig_terminate(int sig)
{
    if (-1 == conn_fd)
        terminate();
    sigint_flag = 1;
}

void terminate()
{
    int i = 0;

    for (i = 0; i < PRINTABLE_COUNT_SIZE; i++) 
        printf("char '%c' : %hu times\n", (i + PRINTABLE_LOWER_VALUE), pcc_total[i]);

    exit(EXIT_SUCCESS);
}

void handle_client(int listen_fd)
{
    int i = 0;
    int pcc_client[PRINTABLE_COUNT_SIZE] = {0};
    int pcc_count = 0;
    int size, size_network, count_network = 0;
    char* size_buffer = NULL;
    char* count_buffer = NULL;
    int not_written = 0; // how much we have left to write
    int not_read = 0; // how much we have left to read
    int total_sent = 0; // how much we've written so far
    int total_read = 0; // how much we've read so far
    int nsent = 0; // how much we've written in last write() call
    int nread = 0; // how much we've read in last read() call
    char buffer[BUFFER_SIZE] = {};

    // accept a new connection
    conn_fd = accept(listen_fd, NULL, NULL);
    if (conn_fd < 0) {
        perror("handle_client(): accept failed\n");
        exit(EXIT_FAILURE);
    }

    // (a) read N (size of message)
    not_read = sizeof(size_network);
    size_buffer = (char *)(&size_network);
    while (not_read > 0)
    {
        nread = read(conn_fd, size_buffer + total_read, not_read);
        if (nread == 0 || KNOWN_TCP_ERROR())
        {
            perror("handle_client(): client connection closed, continue\n");
            goto lblCleanup; // we do not exit the server!
        }
        if (nread < 0)
        {
            perror("handle_client(): an error occurred while reading message size\n");
            exit(EXIT_FAILURE);
        }
        total_read += nread;
        not_read -= nread;
    }
    size = ntohl(size_network);

    // (b) read file content
    not_read = size;
    total_read = 0; // not necessarily used here
    while (not_read > 0)
    {
        nread = read(conn_fd, buffer, BUFFER_SIZE);
        if (nread == 0 || KNOWN_TCP_ERROR())
        {
            perror("handle_client(): client connection closed, continue\n");
            goto lblCleanup; // we do not exit the server!
        }
        if (nread < 0)
        {
            perror("handle_client(): an error occurred while reading message\n");
            exit(EXIT_FAILURE);
        }
        // handle printable characters
        for (i = 0; i < nread; i++)
        {
            if ((PRINTABLE_LOWER_VALUE <= buffer[i]) && (buffer[i] <= PRINTABLE_UPPER_VALUE)) 
            {
                pcc_count++;
                pcc_client[(int)(buffer[i] - PRINTABLE_LOWER_VALUE)]++;
            }
        }
        total_read += nread;
        not_read -= nread;
    }

    // (c) send result
    count_network = htonl(pcc_count);
    count_buffer = (char *)(&count_network);
    not_written = sizeof(count_network);
    while (not_written > 0)
    {
        nsent = write(conn_fd, count_buffer + total_sent, not_written);
        if (nsent == 0 || KNOWN_TCP_ERROR())
        {
            perror("handle_client(): client connection closed, continue\n");
            goto lblCleanup; // we do not exit the server!
        }
        if (nsent < 0)
        {
            perror("handle_client(): an error occurred while sending message\n");
            exit(EXIT_FAILURE);
        }
        total_sent += nsent;
        not_written -= nsent;
    }

    // update global counter
    for (i = 0; i < PRINTABLE_COUNT_SIZE; i++)
    {
        pcc_total[i] += pcc_client[i];
    }
    
lblCleanup:
    close(conn_fd); // close connection
    conn_fd = -1; // no client is being processed
}