@echo off
rem ###############################################################################
rem #
rem #    FILE NAME : pclint.bat
rem #
rem #    GENERAL DESCRIPTION
rem #        This file build *.c in ds folder,and link all the *.lob in ds
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
rem #    06/18/2004   Jianghaibin      C006298     Initial Creation
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
rem #    it will create pclint.log in each folder
rem #
rem ###############################################################################

cd ..\..\tool\PCLint\
@echo on

del ..\..\opus\ds\decode\pclint.log
lint-nt -u -ilnt +v std.lnt ..\..\opus\ds\decode\*.c >> ..\..\opus\ds\decode\pclint.log

del ..\..\opus\ds\widget\pclint.log
lint-nt -u -ilnt +v std.lnt ..\..\opus\ds\widget\*.c >> ..\..\opus\ds\widget\pclint.log

del ..\..\opus\ds\pclint.log
lint-nt -u -ilnt +v std.lnt ..\..\opus\ds\src\*.c >> ..\..\opus\ds\pclint.log
