#============================================================================
#  Name:
#    $(TARGET).MAK
#
#  Description:
#    Makefile to build the $(TARGET) downloadable module.
#
#   The following nmake targets are available in this makefile:
#
#     all           - make .elf and .mod image files (default)
#     clean         - delete object directory and image files
#     filename.o    - make object file
#
#   The above targets can be made with the following command:
#
#     nmake /f $(TARGET).mak [target]
#
#  Assumptions:
#    1. The environment variable ADSHOME is set to the root directory of the
#       arm tools.
#    2. The version of ADS is 1.2 or above.
#
#  Notes:
#    None.
#
#
#        Copyright ?2000-2003 QUALCOMM Incorporated.
#               All Rights Reserved.
#            QUALCOMM Proprietary/GTDR
#
#----------------------------------------------------------------------------
#============================================================================
TARGET         =sec
LOCAL_OBJS     =isecb.o isecw.o isigntext.o oem_seccsc.o oem_secfile.o oem_seclog.o oem_secmgr.o oem_secpkc.o \
oem_secx509parser.o sec.o sec_cd.o sec_cm.o sec_dp.o sec_evtdcvt.o sec_ibrs.o sec_iue.o sec_iwap.o sec_iwapim.o sec_iwim.o sec_lib.o \
sec_main.o sec_mod.o sec_msg.o sec_resp.o sec_str.o sec_time.o sec_tl.o sec_ue.o sec_ueresp.o sec_wim.o sec_wimpkcresp.o \
sec_wimresp.o sec_wimsi.o sec_wimsp.o sec_wtlsparser.o we_libalg.o we_scl.o 

APP_INCLUDES   = -I"$(BREWDIR)\inc" -I"..\..\..\..\..\wep\inc" -I"..\..\..\impl\inc\sec" -I"..\..\..\inc" -I"..\..\..\inc" -I"..\..\..\impl\src\oem"


#------------------------------------------------------------------------------
# specified AEEModGen and AEEAppGen file 
#------------------------------------------------------------------------------
AEEGENPATH= ..\..\..\impl\src\aee
AEEMODGEN = sec_mod
AEEAPPGEN = sec
#------------------------------------------------------------------------------
#Add path of bin
#------------------------------------------------------------------------------
BIN= ..\..\..\..\..\bin
#----------------------------------------------------------------------------
#Lint config file
#---------------------------------------------------------------------------------
LINTF= $(BIN)\tslint.lnt
#-----------------------------------------------------------------------------
#Add all user compile flags here
#-----------------------------------------------------------------------------
USER=

#------------------------------------------------------------------------------
# Common make file
#------------------------------------------------------------------------------
!INCLUDE "$(BIN)\common.min"

#----------------------------------------------------------------------------
# Applet Specific Rules
#----------------------------------------------------------------------------
RULE1 = ..\..\..\impl\core\main
{$(RULE1)}.c.o:
	@echo ---------------------------------------------------------------
	@echo OBJECT $(@F)
	$(CC) $(CFLAGS) $(INC) $(OBJ_FILE)  $(RULE1)\$(SRC_FILE)
	@echo ---------------------------------------------------------------


RULE2 = ..\..\..\impl\core\cert
{$(RULE2)}.c.o:
	@echo ---------------------------------------------------------------
	@echo OBJECT $(@F)
	$(CC) $(CFLAGS) $(INC) $(OBJ_FILE)  $(RULE2)\$(SRC_FILE)
	@echo ---------------------------------------------------------------


RULE3 = ..\..\..\..\..\wep\comm
{$(RULE3)}.c.o:
	@echo ---------------------------------------------------------------
	@echo OBJECT $(@F)
	$(CC) $(CFLAGS) $(INC) $(OBJ_FILE)  $(RULE3)\$(SRC_FILE)
	@echo ---------------------------------------------------------------


RULE4 = ..\..\..\impl\core\ue
{$(RULE4)}.c.o:
	@echo ---------------------------------------------------------------
	@echo OBJECT $(@F)
	$(CC) $(CFLAGS) $(INC) $(OBJ_FILE)  $(RULE4)\$(SRC_FILE)
	@echo ---------------------------------------------------------------


RULE5 = ..\..\..\impl\core\wim
{$(RULE5)}.c.o:
	@echo ---------------------------------------------------------------
	@echo OBJECT $(@F)
	$(CC) $(CFLAGS) $(INC) $(OBJ_FILE)  $(RULE5)\$(SRC_FILE)
	@echo ---------------------------------------------------------------


RULE6 = ..\..\..\impl\core\alg
{$(RULE6)}.c.o:
	@echo ---------------------------------------------------------------
	@echo OBJECT $(@F)
	$(CC) $(CFLAGS) $(INC) $(OBJ_FILE)  $(RULE6)\$(SRC_FILE)
	@echo ---------------------------------------------------------------

RULE7 = ..\..\..\impl\src\aee
{$(RULE7)}.c.o:
	@echo ---------------------------------------------------------------
	@echo OBJECT $(@F)
	$(CC) $(CFLAGS) $(INC) $(OBJ_FILE)  $(RULE7)\$(SRC_FILE)
	@echo ---------------------------------------------------------------


RULE8 = ..\..\..\impl\src\oem
{$(RULE8)}.c.o:
	@echo ---------------------------------------------------------------
	@echo OBJECT $(@F)
	$(CC) $(CFLAGS) $(INC) $(OBJ_FILE)  $(RULE8)\$(SRC_FILE)
	@echo ---------------------------------------------------------------

#----------------------------------------------------------------------------
# Applet Specific Rules
#----------------------------------------------------------------------------
{$(RULE1)}.c.lint:
	@echo ---------------------------------------------------------------
	@echo OBJECT $(@F)
	$(XLINT) -u $(LINT_FLAGS) $(LINTF) -i"$(ARMINC)" $(LINTINC) -os($@) $<
	@echo ---------------------------------------------------------------

{$(RULE2)}.c.lint:
	@echo ---------------------------------------------------------------
	@echo OBJECT $(@F)
	$(XLINT) -u $(LINT_FLAGS) $(LINTF) -i"$(ARMINC)" $(LINTINC) -os($@) $<
	@echo ---------------------------------------------------------------

{$(RULE3)}.c.lint:
	@echo ---------------------------------------------------------------
	@echo OBJECT $(@F)
	$(XLINT) -u $(LINT_FLAGS) $(LINTF) -i"$(ARMINC)" $(LINTINC) -os($@) $<
	@echo ---------------------------------------------------------------

{$(RULE4)}.c.lint:
	@echo ---------------------------------------------------------------
	@echo OBJECT $(@F)
	$(XLINT) -u $(LINT_FLAGS) $(LINTF) -i"$(ARMINC)" $(LINTINC) -os($@) $<
	@echo ---------------------------------------------------------------

{$(RULE5)}.c.lint:
	@echo ---------------------------------------------------------------
	@echo OBJECT $(@F)
	$(XLINT) -u $(LINT_FLAGS) $(LINTF) -i"$(ARMINC)" $(LINTINC) -os($@) $<
	@echo ---------------------------------------------------------------

{$(RULE6)}.c.lint:
	@echo ---------------------------------------------------------------
	@echo OBJECT $(@F)
	$(XLINT) -u $(LINT_FLAGS) $(LINTF) -i"$(ARMINC)" $(LINTINC) -os($@) $<
	@echo ---------------------------------------------------------------

{$(RULE7)}.c.lint:
	@echo ---------------------------------------------------------------
	@echo OBJECT $(@F)
	$(XLINT) -u $(LINT_FLAGS) $(LINTF) -i"$(ARMINC)" $(LINTINC) -os($@) $<
	@echo ---------------------------------------------------------------

{$(RULE8)}.c.lint:
	@echo ---------------------------------------------------------------
	@echo OBJECT $(@F)
	$(XLINT) -u $(LINT_FLAGS) $(LINTF) -i"$(ARMINC)" $(LINTINC) -os($@) $<
	@echo ---------------------------------------------------------------