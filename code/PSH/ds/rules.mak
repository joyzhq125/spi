###############################################################################
#
#    FILE NAME : rules.mak
#
#    GENERAL DESCRIPTION
#        This file contains build rules for target.
#
#    TECHFAITH Telecom Confidential Proprietary
#    (c) Copyright 2002 by TECHFAITH Telecom Corp. All Rights Reserved.
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
#    05/24/2004   penghaibo       p005629       fix jpg decode bug
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
#    Object directory target
#==============================================================================

all : $(DS_LIB)

SRC_FILE = $(@F:.o=.c)          # Input source file specification
OBJ_FILE = $(OBJ_CMD) .\$(OBJDIR)\$(@F)   # Output object file specification


#==============================================================================
#    C code inference rules
#==============================================================================
        
$(DS_OBJS) : $(DS_SOURCE_DIR)\$(SRC_FILE)
        @echo ------------------------------------------------------------------
        @echo OBJECT $(@F)
        $(CC) $(CFLAGS) $(OBJ_FILE) $(DS_SOURCE_DIR)\$(SRC_FILE)
        @echo ------------------------------------------------------------------
$(DECODE_OBJS) : $(DECODE_DIR)\$(SRC_FILE)
        @echo ---$(DECODE_DIR)\$(SRC_FILE)---------------------------------------------------------------
        @echo OBJECT $(@F)
        $(CC) $(CFLAGS) $(OBJ_FILE) $(DECODE_DIR)\$(SRC_FILE)
        @echo ------------------------------------------------------------------              
$(WIDGET_OBJS) : $(WIDGET_DIR)\$(SRC_FILE)
        @echo ---$(WIDGET_DIR)\$(SRC_FILE)---------------------------------------------------------------
        @echo OBJECT $(@F)
        $(CC) $(CFLAGS) $(OBJ_FILE) $(WIDGET_DIR)\$(SRC_FILE)
        @echo ------------------------------------------------------------------              
#==============================================================================
#    Archiver rules
#==============================================================================

$(DS_LIB) : $(DS_OBJS) $(DECODE_OBJS) $(WIDGET_OBJS)
        @echo ------------------------------------------------------------------
        @echo OBJECT $(@F)
        $(AR) $(ARFLAGS) .\$(OBJDIR)\$(DS_LIB) $(DS_OBJS) $(DECODE_OBJS) $(WIDGET_OBJS)
        @echo ------------------------------------------------------------------

#==============================================================================
#    End of list
#==============================================================================        
