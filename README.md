# chan

Go-style channels in C.

This project respects Golang channels and rewrites it in C based on `C99` standard. The implementation is just quite simple, no fancy features included.

## Buffered/Unbuffered channels

The following C code creates an unbuffered channel (buffered if size is specified), then pushes a string `hello` to the channel:

```c
struct chan * ch = chan_make(sizeof(char *), 0);

// sender
char * str = strdup("hello");
chan_send(ch, &str);

// receiver
char * str;
chan_recv(ch, &str);
free(str);
```

Equivalent Go code:

```go
ch := make(chan string)

// sender
ch <- "hello"

// receiver
<- ch
```

## Synchronization (zero-byte transmission)

Let's say you don't want overhead on data transmission but a sychronization way:

```c
struct chan * ch = chan_make(0, 0);
chan_send(ch, NULL);
```

Equivalent Go code:

```go
ch := make(chan struct{})
ch <- struct{}{}
```
