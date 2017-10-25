###############################################################################
#
#    FILE NAME : objects.mak
#
#    GENERAL DESCRIPTION
#        This file contains list of objects that must be compiled and linked to the final output files.
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
#    03/26/2003   lindawang        C000018     Set new pacific image for the first version.
#    06/14/2003   lindawang        P000474     Support play gif animation and fix some bugs.
#    07/26/2003   lindawang        P001074     Remove wbmp file.
#    05/24/2004   penghaibo       p005629       fix jpg decode bug 
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
#    List of objects
#==============================================================================

DS_OBJS = .\obj\ds.o \
    .\obj\ds_drawing.o \
    .\obj\ds_font.o \
    .\obj\ds_fontdata.o \
    .\obj\ds_primitive.o \
    .\obj\ds_util.o \
    .\obj\ds_lcd.o \
    .\obj\ds_sublcd.o \
    .\obj\ds_popup.o \
    .\obj\ds_lcd_driver.o \
    .\obj\hd66766.o \
    .\obj\s44000a01.o \
    .\obj\chinese_font12_decompress.o \
    .\obj\chinese_font16_decompress.o 
DS_SOURCE_DIR = .\src

DECODE_OBJS =.\obj\jpg_to_bmp.o \
    .\obj\gif_to_bmp.o \
    .\obj\jutils.o \
    .\obj\jmemmgr.o \
    .\obj\jmemansi.o \
    .\obj\jidctred.o \
    .\obj\jidctfst.o \
    .\obj\jerror.o \
    .\obj\jdtrans.o \
    .\obj\jdsample.o \
    .\obj\jdpostct.o \
    .\obj\jdphuff.o \
    .\obj\jdmerge.o \
    .\obj\jdmaster.o \
    .\obj\jdmarker.o \
    .\obj\jdmainct.o \
    .\obj\jdinput.o \
    .\obj\jdhuff.o \
    .\obj\jddctmgr.o \
    .\obj\jdcolor.o \
    .\obj\jdcoefct.o \
    .\obj\jdatasrc.o \
    .\obj\jdapistd.o \
    .\obj\jdapimin.o \
    .\obj\jcomapi.o \
    .\obj\wrbmp.o 
DECODE_DIR = .\decode

WIDGET_OBJS = .\obj\int_widget_common.o \
              .\obj\int_widget_custom.o \
              .\obj\int_widget_eventhandler.o \
              .\obj\int_widget_focus.o \
              .\obj\int_widget_general.o \
              .\obj\int_widget_highlevel.o \
              .\obj\int_widget_lowlevel.o \
              .\obj\int_widget_remove.o \
              .\obj\int_widget_show.o \
              .\obj\int_widget_txt_input.o \
              .\obj\int_widget_imm.o \
              .\obj\comm_util.o \
              .\obj\window_manager.o
WIDGET_DIR = .\widget
#==============================================================================
#    End of list
#==============================================================================    
