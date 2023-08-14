#include "chan.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

struct park
{
    bool waiting;
    void * data;
    pthread_mutex_t lock;
    pthread_cond_t cond;
};

static int park_init(struct park * park)
{
    memset(park, 0, sizeof(*park));

    if (pthread_mutex_init(&park->lock, NULL)) {
        return -1;
    } else if (pthread_cond_init(&park->cond, NULL)) {
        pthread_mutex_destroy(&park->lock);
        return -1;
    }

    return 0;
}

static void park_destroy(struct park * park)
{
    pthread_cond_destroy(&park->cond);
    pthread_mutex_destroy(&park->lock);
}

struct chan
{
    size_t size;
    size_t count;

    size_t items;
    size_t recvx;
    size_t sendx;
    bool done;

    struct park recv;
    struct park send;
    pthread_mutex_t lock;
};

struct chan * chan_make(size_t size, size_t count)
{
    struct chan * ch = (struct chan *)malloc(sizeof(struct chan) + (size * count));

    if (!ch) {
        return NULL;
    }

    ch->size = size;
    ch->count = count;
    ch->items = 0;
    ch->recvx = 0;
    ch->sendx = 0;
    ch->done = false;

    if (park_init(&ch->recv)) {
        free(ch);
        return NULL;
    }

    if (park_init(&ch->send)) {
        park_destroy(&ch->recv);
        free(ch);
        return NULL;
    }

    if (pthread_mutex_init(&ch->lock, NULL)) {
        park_destroy(&ch->send);
        park_destroy(&ch->recv);
        free(ch);
        return NULL;
    }

    return ch;
}

void chan_term(struct chan * ch)
{
    free(ch);
}

void chan_close(struct chan * ch)
{
    pthread_mutex_lock(&ch->lock);

    ch->done = true;

    pthread_cond_signal(&ch->send.cond);
    pthread_cond_signal(&ch->recv.cond);
    pthread_mutex_unlock(&ch->lock);
}

static inline bool chan_empty(const struct chan * ch)
{
    if (ch->count) {
        return ch->items == 0;
    }

    return !ch->send.waiting;
}

static inline bool chan_full(const struct chan * ch)
{
    if (ch->count) {
        return ch->items == ch->count;
    }

    return !ch->recv.waiting;
}

static inline size_t chan_index(const struct chan * ch, size_t index)
{
    return (index < ch->count) ? index : 0;
}

static inline void * chan_buffer(struct chan * ch, size_t index)
{
    return (char *)(ch + 1) + (chan_index(ch, index) * ch->size);
}

static size_t dosend(struct chan * ch, const void * data, bool block)
{
    if (ch->done) {
        return 0;
    }

    if (!block && chan_full(ch)) {
        return 0;
    }

    size_t bytes = ch->size;

    pthread_mutex_lock(&ch->send.lock);
    pthread_mutex_lock(&ch->lock);

    if (ch->done) {
        bytes = 0;
    } else if (ch->recv.waiting) {
        // There is a waiting receiver. Bypass the buffer and
        // send the data directly.
        memmove(ch->recv.data, data, ch->size);

        ch->recv.waiting = false;

        pthread_cond_signal(&ch->recv.cond);
    } else if (ch->items < ch->count) {
        // Receivers seem a bit slow, copy the data into the buffer.
        memmove(chan_buffer(ch, ch->sendx), data, ch->size);

        ch->sendx = chan_index(ch, ch->sendx + 1);
        ch->items++;
    } else {
        // No receivers and available buffer so parking on the channel.
        ch->send.data = (void *)data;
        ch->send.waiting = true;

        // Wait until the data is delivered. It is important to put inside
        // a loop in case spurious wakeups.
        while (!ch->done && ch->send.waiting) {
            pthread_cond_wait(&ch->send.cond, &ch->lock);
        }

        if (ch->send.waiting) {
            bytes = 0;
        }
    }

    pthread_mutex_unlock(&ch->lock);
    pthread_mutex_unlock(&ch->send.lock);

    return bytes;
}

size_t chan_send(struct chan * ch, const void * data)
{
    return dosend(ch, data, true);
}

size_t chan_trysend(struct chan * ch, const void * data)
{
    return dosend(ch, data, false);
}

static size_t dorecv(struct chan * ch, void * data, bool block)
{
    if (ch->done) {
        return 0;
    }

    if (!block && chan_empty(ch)) {
        return 0;
    }

    size_t bytes = ch->size;

    pthread_mutex_lock(&ch->recv.lock);
    pthread_mutex_lock(&ch->lock);

    if (ch->done) {
        bytes = 0;
    } else if (ch->send.waiting) {
        // There is a waiting sender. Pop data from the buffer if any, then
        // push the data own by the sender to the buffer. Otherwise, bypass
        // the buffer and send the data directly.
        if (ch->items) {
            memmove(data, chan_buffer(ch, ch->recvx), ch->size);
            memmove(chan_buffer(ch, ch->recvx), ch->send.data, ch->size);

            ch->recvx = chan_index(ch, ch->recvx + 1);
            ch->sendx = ch->recvx;
        } else {
            memmove(data, ch->send.data, ch->size);
        }

        ch->send.waiting = false;

        pthread_cond_signal(&ch->send.cond);
    } else if (ch->items) {
        // Found available data from the buffer.
        memmove(data, chan_buffer(ch, ch->recvx), ch->size);

        ch->recvx = chan_index(ch, ch->recvx + 1);
        ch->items--;
    } else {
        // No available data so parking on the channel.
        ch->recv.data = data;
        ch->recv.waiting = true;

        // Wait until data is received. It is important to put inside
        // a loop in case spurious wakeups.
        while (!ch->done && ch->recv.waiting) {
            pthread_cond_wait(&ch->recv.cond, &ch->lock);
        }

        if (ch->recv.waiting) {
            bytes = 0;
        }
    }

    pthread_mutex_unlock(&ch->lock);
    pthread_mutex_unlock(&ch->recv.lock);

    return bytes;
}

size_t chan_recv(struct chan * ch, void * data)
{
    return dorecv(ch, data, true);
}

size_t chan_tryrecv(struct chan * ch, void * data)
{
    return dorecv(ch, data, false);
}

size_t chan_len(const struct chan * ch)
{
    return ch->items;
}
