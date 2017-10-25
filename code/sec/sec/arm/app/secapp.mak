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
#        Copyright © 2000-2003 QUALCOMM Incorporated.
#               All Rights Reserved.
#            QUALCOMM Proprietary/GTDR
#
#----------------------------------------------------------------------------
#============================================================================
TARGET         =secapp

LOCAL_OBJS           =AEEAppGen.o AEEModGen.o secapp.o 
#APP_INCLUDES   = -I"$(BREWDIR)\inc" -I"..\..\..\sec\win32\wapapp\on_ffa" -I"..\..\..\..\..\wep\inc" -I"..\..\..\inc" -I"..\..\..\impl\inc" -I"..\..\..\impl\src\oem"
APP_INCLUDES   = -I"$(BREWDIR)\inc" -I"..\..\..\..\..\wep\inc" -I"..\..\..\impl\inc\sec" -I"..\..\..\inc" -I"..\..\..\inc" -I"..\..\..\impl\src\oem"
#------------------------------------------------------------------------------
# specified AEEModGen and AEEAppGen file 
#------------------------------------------------------------------------------
AEEGENPATH= ..\..\..\sec\win32\secapp
AEEMODGEN = AEEModGen
AEEAPPGEN = AEEAppGen
#------------------------------------------------------------------------------
#Add path of bin
#------------------------------------------------------------------------------
BIN= ..\..\..\..\..\bin

#----------------------------------------------------------------------------
#Lint config file
#-----------------------------------------------------------------------------
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

RULE1 = ..\..\win32\secapp 
{$(RULE1)}.c.o:
	@echo ---------------------------------------------------------------
	@echo OBJECT $(@F)
	$(CC) $(CFLAGS) $(INC) $(OBJ_FILE)  $(RULE1)\$(SRC_FILE)
	@echo ---------------------------------------------------------------

#----------------------------------------------------------------------
#pc-lint rules
#----------------------------------------------------------------------

#----------------------------------------------------------------------------
# Applet Specific Rules
#----------------------------------------------------------------------------
{$(RULE1)}.c.lint:
	@echo ---------------------------------------------------------------
	@echo OBJECT $(@F)
	$(XLINT) -u $(LINT_FLAGS) $(LINTF) -i"$(ARMINC)" $(LINTINC) -os($@) $<
	@echo ---------------------------------------------------------------


