#Build SEC module,using algorithm static library
#By Bird 070322
#=== makefile start ===
CC = gcc
INC = -I"../../impl/src/wep/inc" -I"../../impl/inc/sec" -I"../../inc" -I"../../inc" -I"../../impl/src/oem"\
      -I"../../impl/src/secd" -I"../../impl/src/comm"
CFLAG = -Wall -O1 
TARGET = secd
LIB = ../alglib/libalg.a
LINK =
LINKFLAG = 
AR = ar
VPATH = .:../../impl/core/cert:../../impl/core/ue:../../impl/core/wim:../../impl/core/alg:../../impl/src/oem\
        :../../impl/core/main:../../impl/src/secd:../../impl/src/wep/src:../../impl/src/comm

OBJ = \
      sec_cd.o sec_cm.o sec_dp.o sec_evtdcvt.o sec_ibrs.o sec_iue.o sec_iwap.o sec_iwapim.o sec_iwim.o sec_lib.o \
      sec_main.o sec_msg.o sec_resp.o sec_str.o sec_time.o sec_tl.o sec_ue.o sec_ueresp.o sec_wim.o \
      sec_wimpkcresp.o sec_wimresp.o sec_wimsi.o sec_wimsp.o sec_wtlsparser.o we_scl.o we_x509.o \
      we_mbk2ostr.o we_mpa.o we_kpg.o we_mpm.o we_rsa.o we_rsalib.o we_csc.o\
      oem_secfile.o oem_seclog.o oem_secmgr.o oem_secx509parser.o oem_secpkc.o oem_seccsc.o\
      sec_ecdr.o sec_eds.o sec_sic.o sec_sicmsg.o secd.o isecw.o isecb.o isigntext.o

All : $(TARGET)

$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(LIB)
#	$(AR) -r $@ $^

%.o: %.c
	@echo ---------------------------
	@echo TARGET $(@F)
	$(CC) $(INC) $(CFLAG) -c -o $@ $^
	@echo ---------------------------

clean:
	rm -f *.o $(TARGET)
    	
#=== makefile end ===
