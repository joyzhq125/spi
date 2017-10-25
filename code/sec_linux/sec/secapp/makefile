#=== makefile start ===
CC = gcc
INC = -I"../../inc" -I"../../impl/inc/sec" -I"../../impl/src/wep/inc" -I"../../impl/src/comm" \
				-I"../../impl/src/isec"  -I"../../impl/src/oem"
CFLAG = -Wall -O1
TARGET = test
LIB = /usr/lib/librt.so
LINK = 
AR = ar
VPATH = .:../../impl/src/isec :../../impl/src/comm :../../impl/src/oem

OBJ = test.o isecw.o isecb.o isigntext.o isec.o sec_edc.o sec_ecdr.o oem_seclog.o

All : $(TARGET)

$(TARGET): $(OBJ) $(LIB)
	$(CC) -o $@ $^
#	$(AR) -r $@ $^

%.o: %.c
	@echo ---------------------------
	@echo TARGET $(@F)
	$(CC) $(INC) $(CFLAG) -c -o $@ $^
	@echo ---------------------------

clean:
	rm -f *.o $(TARGET)
    	
#=== makefile end ===
