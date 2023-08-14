#include "chan.h"

#include <stdio.h>
#include <assert.h>
#include <pthread.h>

static size_t send(struct chan * ch, int val)
{
    return chan_send(ch, &val);
}

static void * sender(void * data)
{
    struct chan * ch = (struct chan *)data;

    assert(send(ch, 0) == sizeof(int));
    assert(chan_len(ch) == 0);

    assert(send(ch, 1) == sizeof(int));
    assert(chan_len(ch) == 0);

    assert(send(ch, 2) == sizeof(int));
    assert(chan_len(ch) == 0);

    assert(send(ch, 3) == sizeof(int));
    assert(chan_len(ch) == 0);

    pthread_exit(NULL);
}

static void * receiver(void * data)
{
    struct chan * ch = (struct chan *)data;
    int val;

    assert(chan_len(ch) == 0);
    assert(chan_recv(ch, &val) == sizeof(int));
    assert(val == 0);

    assert(chan_len(ch) == 0);
    assert(chan_recv(ch, &val) == sizeof(int));
    assert(val == 1);

    assert(chan_len(ch) == 0);
    assert(chan_recv(ch, &val) == sizeof(int));
    assert(val == 2);

    assert(chan_len(ch) == 0);
    assert(chan_recv(ch, &val) == sizeof(int));
    assert(val == 3);

    pthread_exit(NULL);
}

int main()
{
    struct chan * ch = chan_make(sizeof(int), 0);
    pthread_t t1;
    pthread_t t2;

    pthread_create(&t1, NULL, sender, ch);
    pthread_create(&t2, NULL, receiver, ch);
    pthread_join(t2, NULL);
    pthread_join(t1, NULL);

    chan_close(ch);
    chan_term(ch);

    printf("ok\n");

    return 0;
}
