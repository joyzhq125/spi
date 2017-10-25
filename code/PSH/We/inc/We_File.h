/*
 * Copyright (C) Techfaith 2002-2005.
 * All rights reserved.
 *
 * This software is covered by the license agreement between
 * the end user and Techfaith, and may be 
 * used and copied only in accordance with the terms of the 
 * said agreement.
 *
 * Techfaith assumes no responsibility or 
 * liability for any errors or inaccuracies in this software, 
 * or any consequential, incidental or indirect damage arising
 * out of the use of the software.
 *
 */
/*
 * We_File.h
 *
 * Created by Ingmar Persson
 *
 * Revision  history:
 *   021014, IPN: Added WE_FILE_CLOSE_ALL
 *
 */

#ifndef _we_file_h
#define _we_file_h

#ifndef _we_int_h
#include "We_Int.h"
#endif


/****************************************
 * File
 ****************************************/

#define WE_FILE_OPEN                   TPIa_fileOpen

#define WE_FILE_CLOSE                  TPIa_fileClose

#define WE_FILE_WRITE                  TPIa_fileWrite

#define WE_FILE_READ                   TPIa_fileRead

#define WE_FILE_FLUSH                  TPIa_fileFlush

#define WE_FILE_SEEK                   TPIa_fileSeek

#define WE_FILE_REMOVE                 TPIa_fileRemove

#define WE_FILE_RENAME                 TPIa_fileRename

#define WE_FILE_MKDIR                  TPIa_fileMkDir

#define WE_FILE_RMDIR                  TPIa_fileRmDir

#define WE_FILE_GETSIZE_DIR            TPIa_fileGetSizeDir

#define WE_FILE_READ_DIR               TPIa_fileReadDir

#define WE_FILE_SETSIZE                TPIa_fileSetSize

#define WE_FILE_GETSIZE                TPIa_fileGetSize

#define WE_FILE_SELECT                 TPIa_fileSelect

#define WE_FILE_CLOSE_ALL              TPIa_fileCloseAll

#define WE_FILE_QUOTA_GET              TPIa_fileQuotaGet

#define WE_FILE_PATH_PROPERTY_GET      TPIa_filePathPropertyGet

#define WE_FILE_SET_RDONLY    TPI_FILE_SET_RDONLY

#define WE_FILE_SET_WRONLY    TPI_FILE_SET_WRONLY

#define WE_FILE_SET_RDWR      TPI_FILE_SET_RDWR

#define WE_FILE_SET_APPEND    TPI_FILE_SET_APPEND

#define WE_FILE_SET_CREATE    TPI_FILE_SET_CREATE

#define WE_FILE_SET_EXCL      TPI_FILE_SET_EXCL

#define WE_FILE_SET_BUFFERED  TPI_FILE_SET_BUFFERED


#define WE_FILE_SEEK_SET    TPI_FILE_SEEK_SET

#define WE_FILE_SEEK_CUR    TPI_FILE_SEEK_CUR

#define WE_FILE_SEEK_END    TPI_FILE_SEEK_END


#define WE_FILE_DIRTYPE     TPI_FILE_DIRTYPE 

#define WE_FILE_FILETYPE    TPI_FILE_FILETYPE


#define WE_FILE_EVENT_READ  TPI_FILE_EVENT_READ

#define WE_FILE_EVENT_WRITE TPI_FILE_EVENT_WRITE


#define WE_FILE_READ_READY  TPI_FILE_READ_READY

#define WE_FILE_WRITE_READY TPI_FILE_WRITE_READY


#define WE_FILE_OK             TPI_FILE_OK

#define WE_FILE_ERROR_ACCESS   TPI_FILE_ERROR_ACCESS

#define WE_FILE_ERROR_DELAYED  TPI_FILE_ERROR_DELAYED

#define WE_FILE_ERROR_PATH     TPI_FILE_ERROR_PATH

#define WE_FILE_ERROR_INVALID  TPI_FILE_ERROR_INVALID

#define WE_FILE_ERROR_SIZE     TPI_FILE_ERROR_SIZE

#define WE_FILE_ERROR_FULL     TPI_FILE_ERROR_FULL

#define WE_FILE_ERROR_EOF      TPI_FILE_ERROR_EOF

#define WE_FILE_ERROR_EXIST    TPI_FILE_ERROR_EXIST

#endif
