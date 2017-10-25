###############################################################################
#
#    FILE NAME : Makefile
#
#    GENERAL DESCRIPTION
#        Top level make file of OPUS Display System.
#
#    TechFaith Telecom Confidential Proprietary
#    (c) Copyright 2002 by TechFaith Telecom Corp. All Rights Reserved.
#    Reverse engineering is prohibited.
#    The copyright notice does not imply publication.
#
#
#    Revision History
#
#    Modification                  Tracking
#    Date         Author           Number      Description of changes
#    ----------   --------------   ---------   --------------------------------
#    07/10/2002   Sean Kim         crxxxxx     Initial Creation
#    07/23/2002   Stanley Park     crxxxxx     Change path, engine build
#    04/21/2004   Tree Zhan        p005055     Add a swith to use different CFLAGS0 option
#                                              in release version and debug version.
#
#    Self-documenting Code
#    Describe/explain low-level design, especially things in header files,
#    of this module and/or group of funtions and/or specific funtion
#    that are hard to understand by reading code
#    and thus requires detail description.
#
#    PARAMETERS
#
#    Describe and show examples of command line parameters
#
###############################################################################

#==============================================================================
# Porting Source to VOB
#
# New Variables for splitted makefile
# Current folder is /opus/ds
#==============================================================================

ENGINE_VOB 	    = ..\..\cnxt
ENGINE_APIINC   = $(ENGINE_VOB)\independent\apiinc
ENGINE_SYSINC   = $(ENGINE_VOB)\independent\sysinc
ENGINE_REFINC   = $(ENGINE_VOB)\independent\refinc
ENGINE_BUILDTOOL= $(ENGINE_VOB)\tools\buildtool

MAIN_VOB	    = ..\..\main
MAIN_INC        = $(MAIN_VOB)\inc

OPUS_VOB 	    = ..\..\opus
OPUS_AM_INC     = $(OPUS_VOB)\am\inc
OPUS_DS_INC     = $(OPUS_VOB)\ds\inc
OPUS_RM_INC     = $(OPUS_VOB)\rm\inc
OPUS_SP_INC     = $(OPUS_VOB)\sp\inc
OPUS_HWI_INC    = $(OPUS_VOB)\hwi\inc
OPUS_APPS_INC   = $(OPUS_VOB)\apps\inc
OPUS_WAP_INC    = $(OPUS_VOB)\WAP\inc

PRODUCT_ROOT	= $(MAIN_VOB)\$(PRODUCT_NAME)

THDP_VOB  	= ..\..\thirdparty
THDP_EZI_INC	=$(THDP_VOB)\ezi\inc
THDP_PPP_INC	=$(THDP_VOB)\ppp\inc

#==============================================================================
# Target file name and type definitions
#==============================================================================

TARGET     = $(PRODUCT_NAME)
TARGETDIR  = $(TARGET)             # Name for object directory
MAKEFILE   = Makefile              # Name of this make file (without extension)
SRCDIR     = src
INCDIR	   = inc
OBJDIR	   = obj
LIBDIR	   = lib

SCRIPT_DIR = $(PRODUCT_ROOT)\script

#==============================================================================
# Archiver File List and Lib Name
#==============================================================================
OPUS_LIB   = opus.a
AM_LIB 	   = am.a
APPS_LIB   = apps.a
DS_LIB	   = ds.a
RM_LIB	   = rm.a
SP_LIB	   = sp.a
HWI_LIB	   = hwi.a
MAIN_LIB   = main.a
MA_LIB     = ma.a
WAP_LIB    = wap.a

THDP_LIB   = thirdparty.a
EZI_LIB    = ezi.a
PPP_LIB    = ppp.a

#==============================================================================
# Build process output type
#==============================================================================

EXETYPE    = elf                # Target image file format
CC         = tcc                # ARM SDT Thumb 16-bit inst. set ANSI C compiler
LD         = armlink            # ARM SDT linker
AR	       = armar		        # ARM SDT librian
BINTOOL	   = fromelf
OBJ_CMD    = -o                 # Command line option to specify output filename
ERRLOG     = errorlog.exe       # Tool extracting only warning, error, and
SCRIPT_DIR = $(PRODUCT_ROOT)\script
SCLFILE	   = $(SCRIPT_DIR)\poseidon.scl

#==============================================================================
# Compiler Options
#==============================================================================

INC        = -I. -I.\inc
INC_MAIN   = -I$(MAIN_INC)
INC_OPUS   = -I$(OPUS_AM_INC) -I$(OPUS_DS_INC) -I$(OPUS_RM_INC) -I$(OPUS_SP_INC) \
              -I$(OPUS_HWI_INC) -I$(OPUS_APPS_INC) -I$(OPUS_WAP_INC)   
INC_ENGINE = -I$(ENGINE_APIINC) -I$(ENGINE_SYSINC) -I$(ENGINE_REFINC)
INC_THDP   = -I$(THDP_EZI_INC) -I$(THDP_PPP_INC)

!INCLUDE $(SCRIPT_DIR)\make_define.opt
!IF $(NEED_DEBUG)
CFLAGS0 = -c -g+ -O0 -Wa
!ELSE
CFLAGS0 = -c -O2 -Otime -Wa
!ENDIF

APCS 	= -apcs /interwork
CVIA 	= -via $(SCRIPT_DIR)\conexant.opt -via $(SCRIPT_DIR)\armcomp.opt

CFLAGS  =  $(CFLAGS0) $(INC) $(INC_ENGINE) $(INC_MAIN) $(INC_OPUS) $(INC_THDP) $(APCS) $(CVIA)

#==============================================================================
# Linker options
#==============================================================================

SYM  	   = -Symbols  	                      # Direct symbol table to file
MAP  	   = -map                             # Memory map of all object file areas
INFO 	   = -Info Sizes			          # Info on object size
LIST 	   = -list $(TARGET).map              # Direct map and info output to file
LELF	   = -o $(TARGET).elf

LOADER 	   = -scatter $(SCLFILE)              # Use scatter load description file

VIA_SAMPPLE = -via $(SCRIPT_DIR)\sample_obj.opt

FELF	= -c $(TARGET).elf
FBIN	= -bin -output $(TARGET).bin

LFLAGS 	 = $(SYM) $(MAP) $(INFO) $(LIST) $(LELF) $(LOADER) $(VIA_SAMPPLE)
FLFLAGS  = $(FELF) $(FBIN)

#==============================================================================
#  Archiver options
#==============================================================================
ARFLAGS = -create

#==============================================================================
# Tools target definitions (used to echo tool information)
#==============================================================================

ARM_VERSION_BANNER = ADS 1.2 [Build 805]
VER_FLAG = -vsn


!INCLUDE .\objects.mak
!INCLUDE .\rules.mak
