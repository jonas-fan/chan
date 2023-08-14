#include "chan.h"

#include <stdio.h>
#include <assert.h>

static size_t send(struct chan * ch, int val)
{
    return chan_send(ch, &val);
}

static int recv(struct chan * ch)
{
    int val;

    return chan_recv(ch, &val) ? val : 0;
}

int main()
{
    struct chan * ch = chan_make(sizeof(int), 0);

    chan_close(ch);

    assert(send(ch, 0) == 0);
    assert(send(ch, 1) == 0);
    assert(send(ch, 2) == 0);
    assert(recv(ch) == 0);
    assert(recv(ch) == 0);
    assert(recv(ch) == 0);

    chan_term(ch);

    printf("ok\n");

    return 0;
}