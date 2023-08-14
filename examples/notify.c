#include "chan.h"

#include <stdio.h>
#include <assert.h>
#include <pthread.h>

static void * sender(void * data)
{
    struct chan * ch = (struct chan *)data;

    assert(chan_send(ch, NULL) == 0);
    assert(chan_send(ch, NULL) == 0);
    assert(chan_send(ch, NULL) == 0);

    pthread_exit(NULL);
}

static void * receiver(void * data)
{
    struct chan * ch = (struct chan *)data;

    assert(chan_recv(ch, NULL) == 0);
    assert(chan_recv(ch, NULL) == 0);
    assert(chan_recv(ch, NULL) == 0);

    pthread_exit(NULL);
}

int main()
{
    struct chan * ch = chan_make(0, 0);
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
