#include "chan.h"

#include <stdio.h>
#include <assert.h>
#include <unistd.h>

static size_t send(struct chan * ch, int val)
{
    return chan_send(ch, &val);
}

static void sender(struct chan * ch, size_t count)
{
    for (size_t i = 0; i < count; i++) {
        assert(send(ch, i) == sizeof(int));
        assert(chan_len(ch) == i + 1);
    }
}

static void receiver(struct chan * ch, size_t count)
{
    for (size_t i = 0; i < count; i++) {
        int val;

        assert(chan_len(ch) == count - i);
        assert(chan_recv(ch, &val) == sizeof(int));
        assert(val == i);
    }
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