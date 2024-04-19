#undef __KERNEL__
#define __KERNEL__
#undef MODULE
#define MODULE

#include <linux/kernel.h>  	// kernel
#include <linux/module.h>   // module
#include <linux/fs.h>       // register_chrdev
#include <linux/uaccess.h>  // get_user, put_user
#include <linux/string.h>   // memset
#include <linux/slab.h>		// kzalloc

#include "message_slot.h"

MODULE_LICENSE("GPL"); // get rid of taint message by declaring code as GPL

/* MESSAGE SLOT ADT */
static PCHANNEL minors[MAX_MINORS] = { NULL }; // array of message slots
int create_channel(PCHANNEL* pchannel, unsigned long id)
{
    int status = -1;
    PCHANNEL channel = NULL;
    
    channel = (PCHANNEL)HEAPALLOCZ(channel);
    if (channel == NULL)
    {
        printk(KERN_ALERT "create_channel(): HEAPALLOCZ failed\n");
        status = -ENOMEM;
        goto lblCleanup;
    }

    channel->id = id; // set the channel id
    channel->message_length = 0;

    // Transfer ownership
    *pchannel = channel;
    channel = NULL;

    status = SUCCESS;

lblCleanup:
    (void)free_channel(channel);
    return status;
}

int add_channel(PCHANNEL* head, PCHANNEL channel)    
{
    if (NULL == channel)
    {
        printk("add_channel(): invalid arguments\n");
        return -EINVAL;
    }

    // first channel to be added
    if (NULL == *head)
    {
        *head = channel;
    }
    else // add to the head of the list
    {
        channel->next = *head;
        *head = channel;
    }

    return SUCCESS;
}

PCHANNEL find_channel(PCHANNEL head, unsigned long id)
{
    PCHANNEL channel = head;

    while (NULL != channel)
    {
        if (id == channel->id)
            return channel;
        channel = channel->next;
    }

    return NULL;
}

void free_channel(PCHANNEL channel)
{
    if (channel != NULL)
    {
        (void)kfree(channel);
    }
}

void free_slot(PCHANNEL head)
{
    if (head != NULL)
    {
        (void)free_slot(head->next);
        (void)free_channel(head);
    }
}

// just for debugging
void print_slot(PCHANNEL head, int iminor)
{
    PCHANNEL channel = head;
    if (NULL == channel)
    {
        printk("[slot %d]: empty slot", iminor);
        return;
    }
    printk("[slot %d]: ", iminor);
    while (NULL != channel)
    {
        printk("(channel: id=%ld, msg=%s)->", channel->id, channel->message);
        channel = channel->next;
    }
    printk("END OF SLOT\n");
}

/* DEVICE FUNCTIONS */
static int device_open(struct inode* inode, struct file* file)
{
    // we already hold a list of channels for each minor, adding channels in device_ioctl
    printk("Called device_open(%p,%p)\n", inode, file);
    return SUCCESS;
}

static int device_release(struct inode* inode, struct file* file)
{
    // no memory to free or cleanup needed
    printk("Called device_release(%p,%p)\n", inode, file);
    return SUCCESS;
}

static ssize_t device_read(struct file* file, char __user* buffer, size_t length, loff_t* offset)
{
    int i = 0;
    unsigned int minor = -1;
    unsigned long channel_id = 0;
    PCHANNEL channel = NULL;

    printk("Called device_read(%p,%p,%ld)\n", file, buffer, length);

    minor = iminor(file_inode(file)); // get the minor number
    channel_id = (unsigned long)file->private_data; // get the channel id

    // validate arguments
    if (INVALID_CHANNEL_ID == channel_id)
    {
        printk("device_write(): no channel has been set on the fd\n");
        return -EINVAL;
    }

    // check if channel exists
    channel = find_channel(minors[minor], channel_id);
    if (NULL == channel)
    {
        printk("device_write(): channel does not exist: %ld\n", channel_id);
        return -EINVAL;
    }

    // validate the message length
    if (0 == channel->message_length)
    {
        printk("device_read(): no message to read\n");
        return -EWOULDBLOCK;
    }
    if (length < channel->message_length)
    {
        printk("device_read(): buffer is too small - %ld, message_length is %d\n", length, channel->message_length);
        return -ENOSPC;
    }

    // read the message from the channel to the user buffer
    for (i = 0; i < channel->message_length; i++)
    {
        if (put_user(channel->message[i], &buffer[i]))
        {
            printk("device_read(): put_user() failed\n");
            return -EFAULT;
        }
    }

    return channel->message_length;
}

static ssize_t device_write(struct file* file, const char __user* buffer, size_t length, loff_t* offset)
{
    int i = 0;
    unsigned int minor = -1;
    unsigned long channel_id = 0;
    PCHANNEL channel = NULL;
    
    printk("Called device_write(%p,%p,%ld)\n", file, buffer, length);

    minor = iminor(file_inode(file)); // get the minor number
    channel_id = (unsigned long)file->private_data; // get the channel id
    
    // validate arguments
    if (INVALID_CHANNEL_ID == channel_id)
    {
        printk("device_write(): no channel has been set on the fd\n");
        return -EINVAL;
    }
    if (0 == length || BUFFER_SIZE < length)
    {
        printk("device_write(): message length (%ld) is invalid\n", length);
        return -EMSGSIZE;
    }

    // check if channel exists
    channel = find_channel(minors[minor], channel_id);
    if (NULL == channel)
    {
        printk("device_write(): channel does not exist: %ld\n", channel_id);
        return -EINVAL;
    }

    // write the message from the user buffer to the channel
    for (i = 0; i < length; i++)
    {
        if (get_user(channel->message[i], &buffer[i]))
        {
            printk("device_write(): get_user() failed\n");
            return -EFAULT;
        }
    }
    channel->message_length = length;

    return length;
}

static long device_ioctl(struct file* file, unsigned int ioctl_command_id, unsigned long ioctl_param)
{
    int status = -1;
    unsigned int minor = -1;
    PCHANNEL channel = NULL;

    printk("Called device_ioctl(%p,%d,%ld)\n", file, ioctl_command_id, ioctl_param);

    // validate arguments
    if (MSG_SLOT_CHANNEL != ioctl_command_id)
    {
        printk("device_ioctl(): invalid ioctl_command_id: %d\n", ioctl_command_id);
        return -EINVAL;
    }
    if (INVALID_CHANNEL_ID >= ioctl_param)
    {
        printk("device_ioctl(): invalid ioctl_param (channel id): %ld\n", ioctl_param);
        return -EINVAL;
    }

    file->private_data = (void *)ioctl_param; // set the fd's channel id
    minor = iminor(file_inode(file)); // get the minor number

    // check if channel exists
    channel = find_channel(minors[minor], ioctl_param);
    if (NULL != channel)
    {
        printk("device_ioctl(): channel already exists: %ld\n", ioctl_param);
        return SUCCESS;
    }

    // we create a new channel and add it to the slot
    printk("device_ioctl(): creating new channel: %ld\n", ioctl_param);
    status = create_channel(&channel, ioctl_param);
    if (SUCCESS != status)
    {
        printk("device_ioctl(): create_channel() failed\n");
        goto lblCleanup;
    }

    // pass the address of the head of the slot in order to update it if needed
    status = add_channel(&minors[minor], channel);
    if (SUCCESS != status)
    {
        printk("device_ioctl(): add_channel() failed\n");
        goto lblCleanup;
    }

    // Transfer owernship (to slot) - don't free it
    channel = NULL;

    status = SUCCESS;

lblCleanup:
    (void)free_channel(channel);
    return status;
}

/* DEVICE SETUP */
struct file_operations fops = {
    .owner	        = THIS_MODULE, 
    .read           = device_read,
    .write          = device_write,
    .open           = device_open,
    .unlocked_ioctl = device_ioctl,
    .release        = device_release,
};
static int __init ms_init(void)
{
    int rc = -1;

    printk("Called ms_init()\n");
    // register driver capabilities
    rc = register_chrdev(MAJOR_NUM, DEVICE_RANGE_NAME, &fops);
    if (rc < 0)
    {
        printk(KERN_ALERT "%s registraion failed for %d\n", DEVICE_FILE_NAME, MAJOR_NUM);
        return rc;
    }

    printk("Registeration is successful.");
    printk("If you want to talk to the device driver you have to create a device file:\n");
    printk("mknod /dev/%s c %d <MINOR>\n", DEVICE_FILE_NAME, MAJOR_NUM);
    printk("Don't forget to rm the device file and rmmod when you're done\n");
    
    return SUCCESS;
}

static void __exit ms_cleanup(void)
{
    int i = 0;
    
    printk("Called ms_cleanup()\n");

    // when unloaded, free all memory
    for (i = 0; i < MAX_MINORS; i++)
    {
        (void)free_slot(minors[i]);
    }

    // unregister the device
    unregister_chrdev(MAJOR_NUM, DEVICE_RANGE_NAME);
}

module_init(ms_init);
module_exit(ms_cleanup);