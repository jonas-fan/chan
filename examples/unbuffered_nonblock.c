#include "chan.h"

#include <stdio.h>
#include <assert.h>

int main()
{
    struct chan * ch = chan_make(sizeof(int), 0);
    int val;

    assert(chan_len(ch) == 0);
    assert(chan_trysend(ch, &val) == 0);
    assert(chan_trysend(ch, &val) == 0);
    assert(chan_trysend(ch, &val) == 0);
    assert(chan_len(ch) == 0);
    assert(chan_tryrecv(ch, &val) == 0);
    assert(chan_tryrecv(ch, &val) == 0);
    assert(chan_tryrecv(ch, &val) == 0);
    assert(chan_len(ch) == 0);

    chan_close(ch);
    chan_term(ch);

    printf("ok\n");

    return 0;
}
