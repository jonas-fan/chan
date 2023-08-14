#include "chan.h"

#include <stdio.h>
#include <assert.h>
#include <unistd.h>

static size_t send(struct chan * ch, int val)
{
    return chan_send(ch, &val);
}

static size_t trysend(struct chan * ch, int val)
{
    return chan_trysend(ch, &val);
}

static void sender(struct chan * ch, size_t count)
{
    for (size_t i = 0; i < count; i++) {
        assert(send(ch, i) == sizeof(int));
        assert(chan_len(ch) == i + 1);
    }

    assert(chan_len(ch) == count);
    assert(trysend(ch, 10) == 0);
    assert(trysend(ch, 10) == 0);
    assert(trysend(ch, 10) == 0);
    assert(chan_len(ch) == count);
}

static void receiver(struct chan * ch, size_t count)
{
    int val;

    for (size_t i = 0; i < count; i++) {
        assert(chan_len(ch) == count - i);
        assert(chan_recv(ch, &val) == sizeof(int));
        assert(val == i);
    }

    assert(chan_len(ch) == 0);
    assert(chan_tryrecv(ch, &val) == 0);
    assert(chan_tryrecv(ch, &val) == 0);
    assert(chan_tryrecv(ch, &val) == 0);
    assert(chan_len(ch) == 0);
}

int main()
{
    struct chan * ch = chan_make(sizeof(int), 4);

    sender(ch, 4);
    receiver(ch, 4);

    chan_close(ch);
    chan_term(ch);

    printf("ok\n");

    return 0;
}