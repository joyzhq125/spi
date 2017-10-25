#For build algorithm static library,when use it ,pls include we_libalg.h and link libalg.a to your program
#By Bird 070322
#=== makefile start ===
CC = gcc
INC = -I"../../impl/src/wep/inc" -I"../../impl/inc/sec" -I"../../inc" -I"../../inc" -I"../../impl/src/oem"\
      -I"../../impl/src/secd" -I"../../impl/src/comm"
CFLAG = -Wall -O1 
TARGET = libalg.a
LIBPATH = 
LINK = 
LINKFALG =
AR = ar
VPATH = .:../../impl/core/cert:../../impl/core/ue:../../impl/core/wim:../../impl/core/alg:../../impl/src/oem\
        :../../impl/core/main:../../impl/src/secd:../../impl/src/wep/src:../../impl/src/comm

OBJ = \
      we_libalg.o we_tripdes.o we_sha.o we_aes.o we_des.o we_hmac.o we_idea.o \
      we_md2.o we_md5.o we_rc2.o we_rc4.o we_rc5.o we_rnd.o 
 

All : $(TARGET)

$(TARGET): $(OBJ)
	$(AR) -r $@ $^

%.o: %.c
	@echo ---------------------------
	@echo TARGET $(@F)
	$(CC) $(INC) $(CFLAG) -c -o $@ $^
	@echo ---------------------------

clean:
	rm -f *.o $(TARGET)
    	
#=== makefile end ===
