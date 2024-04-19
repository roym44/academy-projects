#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/* MACROS */
#define INVALID_RESULT (-1) 
#define BUFFER_SIZE (100000)

/* TYPEDEFS */
typedef enum _CLIENT_ARGS
{
	CLIENT_ARGS_SELF = 0,
	CLIENT_ARGS_SERVER_IP,	    // the server's IP address
    CLIENT_ARGS_SERVER_PORT,    // the server's port
	CLIENT_ARGS_FILE,	        // path of the file to send

	CLIENT_ARGS_COUNT // must be last
} CLIENT_ARGS;

/* FUNCTIONS */
// transfers the contents of the file (given an open fd) to the server (given ip and port)
int transfer_to_server(int fd, int size, char* server_ip, char* server_port); 

int main(int argc, char const *argv[])
{
    int fd = -1;
    uint16_t size = 0;
    int result = -1;
    struct stat file_stat = {};

    // validate arguments
    if (argc != CLIENT_ARGS_COUNT) 
    {
        perror("pcc_client(): invalid number of arguments\n");
        return EXIT_FAILURE;
    }

    // open the file
    fd = open(argv[CLIENT_ARGS_FILE], O_RDONLY);
    if (-1 == fd) 
    {
        perror("pcc_client(): error in opening file\n");
        return EXIT_FAILURE;
    }

    // get the file size
    if (stat(argv[CLIENT_ARGS_FILE], &file_stat) < 0)
    {
        perror("pcc_client(): error getting file stat\n");
        return EXIT_FAILURE;
    }
    size = file_stat.st_size;

    // communicate with server
    result = transfer_to_server(fd, size, (char*)argv[CLIENT_ARGS_SERVER_IP], (char*)argv[CLIENT_ARGS_SERVER_PORT]);
    if (INVALID_RESULT == result)
    {
        perror("pcc_client(): transfer to server failed\n");
        return EXIT_FAILURE;
    }

    // print number of printable characters
    printf("# of printable characters: %hu\n", result);
    
    return EXIT_SUCCESS;
}


int transfer_to_server(int fd, int size, char* server_ip, char* server_port)
{
    int sock_fd = -1;
    int printable_count = 0;
    struct sockaddr_in server = {};
    int size_network, printable_network = 0;
    char* size_buffer = NULL;
    char* printable_buffer = NULL;
    int not_written = 0; // how much we have left to write
    int not_read = 0; // how much we have left to read
    int total_sent = 0; // how much we've written so far
    int total_read = 0; // how much we've read so far
    int nsent = 0; // how much we've written in last write() call
    int nread = 0; // how much we've read in last read() call
    char buffer[BUFFER_SIZE] = {};

    // create a socket
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0)
    {
        perror("transfer_to_server(): could not create socket\n");
        return INVALID_RESULT;
    }

    // get server details 
    memset(&server, 0, sizeof(struct sockaddr_in));
    server.sin_family = AF_INET;
    server.sin_port = htons(atoi(server_port));
    if (inet_pton(AF_INET, server_ip, &(server.sin_addr.s_addr)) < 1)
    {
        perror("transfer_to_server(): error in inet_pton\n");
        return INVALID_RESULT;
    }

    // connect to server 
    if (connect(sock_fd, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        perror("transfer_to_server(): failed to connect\n");
        return INVALID_RESULT;
    }

    // (a) sending N
    size_network = htonl(size);
    not_written = sizeof(size_network);
    size_buffer = (char *)&size_network;
    while (not_written > 0) // until nothing left to write
    {
        nsent = write(sock_fd, size_buffer + total_sent, not_written);
        if (nsent < 0)
        {
            perror("transfer_to_server(): an error occurred while writing\n");
            return INVALID_RESULT;
        }
        total_sent += nsent;
        not_written -= nsent;
    }

    // (b) sending the file content
    not_written = size;
    while (not_written > 0)
    {
        // read from the file
        nread = read(fd, buffer, BUFFER_SIZE);
        if (nread < 0)
        {
            perror("transfer_to_server(): an error occurred while reading file\n");
            return INVALID_RESULT;
        }

        // write what we just read
        total_sent = 0;
        while (nread > 0)
        {
            nsent = write(sock_fd, buffer + total_sent, nread);
            if (nsent < 0)
            {
                perror("transfer_to_server(): an error occurred while writing content\n");
                return INVALID_RESULT;
            }
            nread -= nsent;
            total_sent += nsent;
            not_written -= nsent;
        }
    }

    // (c) receiving result from server
    not_read = sizeof(printable_network);
    total_read = 0;
    printable_buffer = (char *)(&printable_network);
    while (not_read > 0)
    {
        nread = read(sock_fd, printable_buffer + total_read, not_read);
        if (nread < 0)
        {
            perror("transfer_to_server(): an error occurred while reading printable count\n");
            return INVALID_RESULT;
        }
        total_read += nread;
        not_read -= nread;
    }
    printable_count = ntohl(printable_network);

    return printable_count;
}