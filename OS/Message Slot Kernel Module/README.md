# Introduction
An assignment given as part of the Operating Systems (fall 2024) course - implementing a kernel module.

The goal is to gain experience with kernel programming, inter-process communication (IPC), kernel
modules, and drivers. We implement a kernel module that provides a new IPC mechanism,
called a message slot. A message slot is a character device file through which processes communicate.
A message slot device has multiple message channels active concurrently, which can be used by
multiple processes. After opening a message slot device file, a process uses ioctl() to specify the
id of the message channel it wants to use. It subsequently uses read()/write() to receive/send
messages on the channel. In contrast to pipes, **a message channel preserves a message until it
is overwritten, so the same message can be read multiple times**.

# Message slot specification
A message slot appears in the system as a character device file. This device file is managed by the
message slot device driver, which you will implement. A message slot is a *pseudo* device: it doesn’t
correspond to a physical hardware device, and therefore all its functionality is provided by the
device driver.

**Message slot device files**: A device file has a major number and a minor number. The major
number tells the kernel which driver is associated with the device file. The minor number is used
internally by the driver for its own purposes.

In our case: There can be several message slot files, which correspond to different message slots.
All of these files are managed by your driver, i.e., **they all have the same major number, which
is hard-coded to 235**. However, different message slot files will have different minor numbers,
allowing your driver to distinguish between them.

Device files are created with the mknod command, which takes as arguments the file’s major and
minor numbers. For example:
```mknod /dev/slot0 c 235 0```\
The above command creates a character device file /dev/slot0 with major number 235 (i.e., a
message slot) and minor number 0. Then, a subsequent command creates another message slot file:
```mknod /dev/slot1 c 235 1```

The message slot driver has special semantics for the ioctl, write, and read file operations.

## ioctl
A message slot supports a single ioctl command, named MSG_SLOT_CHANNEL. This command takes
a single unsigned int parameter that specifies a non-zero channel id. Invoking the ioctl() sets
the file descriptor’s channel id. Subsequent reads/writes on this file descriptor will receive/send
messages on the specified channel.

## write
Writes a non-empty message of up to 128 bytes from the user’s buffer to the channel. Returns the
number of bytes written, unless an error occurs. (Note that the message can contain any sequence
of bytes, it is not necessarily a C string.) 

## read
Reads the last message written on the channel into the user’s buffer. Returns the number of bytes
read, unless an error occurs.

# Documentation
We implement the following:
1. message_slot: A kernel module implementing the message slot IPC mechanism.
2. message_sender: A user space program to send a message.
3. message_reader: A user space program to read a message.

## Message slot kernel module (device driver)
We implement the module in files named message_slot.c and message_slot.h:
1. The module uses the hard-coded major number 235.
2. The module implements the file operations needed to provide the message slot interface:
device_open, device_ioctl, device_read, and device_write.
3. In the module’s struct file_operations, we include the initialization
```.owner = THIS_MODULE,```\
This will prevent the module from being unloaded while it is being used.
4. We allocate memory using kmalloc() with GFP_KERNEL flag.

## Message sender
We implement the program in a file named message_sender.c.
Command line arguments:
- argv[1]: message slot file path.
- argv[2]: the target message channel id. Assume a non-negative integer.
- argv[3]: the message to pass.

The flow:
1. Open the specified message slot device file.
2. Set the channel id to the id specified on the command line.
3. Write the specified message to the message slot file. Don’t include the terminating null character
of the C string as part of the message.
4. Close the device.
5. Exit the program with exit value 0.
If an error occurs in any of the above steps, print an appropriate error message and exit the program with exit value 1.

## Message reader
We implement the program in a file named message_reader.c.
Command line arguments:
- argv[1]: message slot file path.
- argv[2]: the target message channel id. Assume a non-negative integer.

The flow:
1. Open the specified message slot device file.
2. Set the channel id to the id specified on the command line.
3. Read a message from the message slot file to a buffer.
4. Close the device.
5. Print the message to standard output (using the write() system call). Print only the message,
without any additional text.
6. Exit the program with exit value 0.
If an error occurs in any of the above steps, print an appropriate error message and exit the program with exit value 1.

# Usage
```
gcc -O3 -Wall -std=c11 message_sender.c (or message_reader.c)
```
Example session:
1. As root (e.g., with sudo): Load (insmod) the message_slot.ko module.
2. As root: Create a message slot file using mknod.
3. As root: Change the message slot file’s permissions to make it readable and writable by your user.
4. Invoke message_sender to send a message on some channel.
5. Invoke message_reader to read the message on the same channel.
6. Execute steps #4 and #5 several times, for different channels, in different sequences.

The message slot driver:
```
make
insmod message_slot.ko
lsmod | grep message

mknod /dev/msgslot0 c 235 0
mknod /dev/msgslot1 c 235 1
chmod o+rw /dev/msgslot0

echo "hello" > /dev/msgslot0
echo "hello" > /dev/msgslot1

rm /dev/msgslot0
rm /dev/msgslot1
rmmod message_slot # --force
```

The reader and sender:
```
gcc -O3 -Wall -std=c11 message_sender.c -o ms
gcc -O3 -Wall -std=c11 message_reader.c -o mr

./ms /dev/msgslot0 3 hello
./mr /dev/msgslot0 3
./ms /dev/msgslot0 5 hello_channel_5
./mr /dev/msgslot0 5

./ms /dev/msgslot1 3 hello
./mr /dev/msgslot1 3
```