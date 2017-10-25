CC = gcc
CFLAGS = -Wall
OBJS = Ecdr.o log.o MsgHdl.o MsgQueue.o UartHdl.o Wifunc.o DevInfo.o
LDFLAGS = -lpthread -lrt

all: wifunc

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $< $(LDFLAGS)

wifunc: $(OBJS)
	$(CC) -o $@ $(OBJS) $(LDFLAGS)

clean:
	rm -f demo1 *.o
