#ifndef __CHAN_H__
#define __CHAN_H__

#include <stddef.h>

struct chan;

// Initialize a channel. If count is not zero, a buffered channel is created.
struct chan * chan_make(size_t size, size_t count);

// Finalize a channel.
void chan_term(struct chan * ch);

// Shutdown a channel. Note that sending and receiving will not be allowed
// on a closed channel.
void chan_close(struct chan * ch);

// Sending data to a channel.
size_t chan_send(struct chan * ch, const void * data);

// Try sending data to a channel. This call will not get blocked.
size_t chan_trysend(struct chan * ch, const void * data);

// Receiving data from a channel.
size_t chan_recv(struct chan * ch, void * data);

// Try receiving data to a channel. This call will not get blocked.
size_t chan_tryrecv(struct chan * ch, void * data);

// Return the number of elements (unread) in a channel.
size_t chan_len(const struct chan * ch);

#endif  // __CHAN_H__
