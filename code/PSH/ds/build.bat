@echo off
rem ####################################################################################################
rem #
rem #    FILE NAME : build.bat
rem #
rem #    GENERAL DESCRIPTION
rem #        This file is the main build batch file for Poseidon display system build.
rem #
rem #    TECHFAITH Telecom Confidential Proprietary
rem #    (c) Copyright 2002 by TECHFAITH Telecom Corp. All Rights Reserved.
rem #    Reverse engineering is prohibited.
rem #    The copyright notice does not imply publication.
rem #
rem #
rem #    Revision History
rem #
rem #    Modification                  Tracking
rem #    Date         Author           Number      Description of changes
rem #    ----------   --------------   ---------   --------------------------------
rem #    07/10/2002   Sean Kim         crxxxxx     Initial Creation
rem #    07/23/2002   Stanley Park     crxxxxx     Change path, engine build
rem #
rem #
rem #    Self-documenting Code
rem #    Describe/explain low-level design, especially things in header files,
rem #    of this module and/or group of funtions and/or specific funtion
rem #    that are hard to understand by reading code
rem #    and thus requires detail description.
rem #
rem #    PARAMETERS
rem #
rem #    Describe and show examples of command line parameters
rem #
rem ###############################################################################

set PRODUCT_NAME=poseidon
set VOB_MAIN=main
set VOB_CNXT=cnxt
set VOB_OPUS=opus
set VOB_BUILD=poseidon
set BUILD_LOG=compile_out.log

del compile_out.log

@echo on
@echo #####################################################################
@echo #   Component Build
@echo #   Component Sub-Dir : Opus - Application Manager
@echo #####################################################################
@echo off

nmake2 -x compile_out.log

notepad compile_out.log

rem END OF BUILD FILE....
