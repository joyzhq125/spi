CC = gcc
CFLAGS = -Wall
OBJS = co_leds_server.o

all: co_leds_server

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $< $(LDFLAGS)

fbtest: $(OBJS)
	$(CC) -o $@ $(OBJS) $(LDFLAGS)

clean:
	rm -f rbcfg *.o
