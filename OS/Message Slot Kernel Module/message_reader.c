#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "message_slot.h"

int main(int argc, char* argv[]) 
{
    int fd = -1;
    ssize_t bytes_read = 0;
    ssize_t bytes_written = 0;
    char buffer[BUFFER_SIZE] = {0};
    unsigned long channel_id = 0;

    // validate arguments
    if (argc != READER_ARGS_COUNT) 
    {
        perror("message_reader(): invalid number of arguments\n");
        return EXIT_FAILURE;
    }

    // open the message slot
    fd = open(argv[READER_ARGS_FILE_PATH], O_RDWR);
    if (-1 == fd) 
    {
        perror("message_reader(): error in opening file\n");
        return EXIT_FAILURE;
    }

    // set the channel id
    channel_id = (unsigned long)atoi(argv[READER_ARGS_CHANNEL_ID]);
    if (-1 == ioctl(fd, MSG_SLOT_CHANNEL, channel_id)) 
    {
        perror("message_reader(): error setting the channel\n");
        return EXIT_FAILURE;
    }

    // read a message from the message slot to the buffer
    bytes_read = read(fd, buffer, BUFFER_SIZE);
    if (bytes_read < 0) 
    {
        perror("message_reader(): error in reading from message slot\n");
        return EXIT_FAILURE;
    }

    // close the device
    (void)close(fd);

    // print the message to stdout
    bytes_written = write(STDOUT_FILENO, buffer, bytes_read);
    if (bytes_written < 0) 
    {
        perror("message_reader(): error in writing to stdout\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
