#ifndef MESSAGE_SLOT
#define MESSAGE_SLOT

#include <linux/ioctl.h>	// ioctl

/* MACROS */
#define MAJOR_NUM (235) 										// the major device number (no dynamic registration)
#define MSG_SLOT_CHANNEL (_IOW(MAJOR_NUM, 0, unsigned long)) 	// set the message of the device driver
#define DEVICE_RANGE_NAME ("message_slot")
#define DEVICE_FILE_NAME ("message_slot_dev")
#define BUFFER_SIZE (128)
#define MAX_MINORS (256) // maximum number of message slots
#define INVALID_CHANNEL_ID (0)

#define	FAILURE (1) // for general errors
#define	SUCCESS (0)

// Allocates a zero-ed buffer of n elements from pointer p on the heap 
#define HEAPALLOCZ(p) kzalloc(sizeof(*p), GFP_KERNEL)

/* TYPEDEFS */
typedef struct _CHANNEL // a single message channel
{
    unsigned long id; 			// the channel id
    char message[BUFFER_SIZE]; 	// the message
	int message_length;			// the message length
	struct _CHANNEL* next;		// the next channel in the list
} CHANNEL;
typedef CHANNEL* PCHANNEL; // each message slot is a linked list of channels

typedef enum _SENDER_ARGS
{
	SENDER_ARGS_SELF = 0,
	SENDER_ARGS_FILE_PATH,	// message slot file path
    SENDER_ARGS_CHANNEL_ID, // targed message channel id
	SENDER_ARGS_MESSAGE,	// the message to pass

	SENDER_ARGS_COUNT // must be last
} SENDER_ARGS;

typedef enum _READER_ARGS
{
	READER_ARGS_SELF = 0,	
	READER_ARGS_FILE_PATH,	// message slot file path
    READER_ARGS_CHANNEL_ID, /// targed message channel id

	READER_ARGS_COUNT // must be last
} READER_ARGS;

/* MESSAGE SLOT ADT */
int create_channel(PCHANNEL* pchannel, unsigned long id); // creates a new empty channel
int add_channel(PCHANNEL* phead, PCHANNEL channel); // adds given channel to the head of the list
PCHANNEL find_channel(PCHANNEL head, unsigned long id); // returns a pointer to the channel with the given id, returns NULL if not found
void free_channel(PCHANNEL channel); // frees a single channel
void free_slot(PCHANNEL head); // frees the memory for a slot (channel list starting at head )

#endif // MESSAGE_SLOT