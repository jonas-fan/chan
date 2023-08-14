# chan

Go-style channels in C.

This project respects Golang channels and rewrites it in C based on `C99` standard. The implementation is just quite simple, no fancy features included.

## Unbuffered channels

```c
#include "chan.h"

int main()
{
    const int val = 1234;

    // Create an unbuffered channel
    struct chan * ch = chan_make(sizeof(int), 0);

    // Blocked until a receiver performs a read
    chan_send(ch, &val);

    // Close the channel
    chan_close(ch);

    // Destroy the channel
    chan_term(ch);

    return 0;
}
```

## Buffered channels

```c
#include "chan.h"

int main()
{
    // Create a buffered channel
    struct chan * ch = chan_make(sizeof(int), 1);

    // Send
    {
        const int val = 1234;
        chan_send(ch, &val);
    }

    // Receive
    {
        int val;
        chan_recv(ch, &val);
    }

    // Close the channel
    chan_close(ch);

    // Destroy the channel
    chan_term(ch);

    return 0;
}
```

## Synchronization (zero-byte transmission)

```c
#include "chan.h"

int main()
{
    // Create an unbuffered channel
    struct chan * ch = chan_make(0, 0);

    // Blocked until a receiver performs a read
    // or chan_recv(ch, NULL); to wait a writer
    chan_send(ch, NULL);

    // Close the channel
    chan_close(ch);

    // Destroy the channel
    chan_term(ch);

    return 0;
}
```
