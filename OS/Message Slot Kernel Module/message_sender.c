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
    ssize_t bytes_written = 0;
    unsigned long channel_id = 0;

    // validate arguments
    if (argc != SENDER_ARGS_COUNT)
    {
        perror("message_sender(): invalid number of arguments\n");
        return EXIT_FAILURE;
    }

    // open the message slot
    fd = open(argv[SENDER_ARGS_FILE_PATH], O_RDWR);
    if (-1 == fd) 
    {
        perror("message_reader(): error in opening file\n");
        return EXIT_FAILURE;
    }

    // set the channel id
    channel_id = (unsigned long)atoi(argv[SENDER_ARGS_CHANNEL_ID]);
    if (-1 == ioctl(fd, MSG_SLOT_CHANNEL, channel_id)) 
    {
        perror("message_reader(): error setting the channel\n");
        return EXIT_FAILURE;
    }
    
    // write the message to the message slot
    bytes_written = write(fd, argv[SENDER_ARGS_MESSAGE], strlen(argv[SENDER_ARGS_MESSAGE]));
    if (strlen(argv[SENDER_ARGS_MESSAGE]) != bytes_written) 
    {
        perror("message_sender(): error in writing to message slot\n");
        return EXIT_FAILURE;
    }

    // close the device
    (void)close(fd);

    return EXIT_SUCCESS;
}
