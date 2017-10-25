#ifndef __WE_FILE_H__
#define __WE_FILE_H__

/*==================================================================================================
    FILE NAME :
        we_file.h
    MODULE NAME :
        WEA
    GENERAL DESCRIPTION
        This file declare API of file.
    TECHFAITH Wireless Confidential Proprietary(c)
        Copyright 2006 by TECHFAITH Wireless. All Rights Reserved.
====================================================================================================
    Revision History
    Modification Tracking
    Date       Author         Number    Description of changes
    ---------- -------------- --------- --------------------------------------
    2006-06-01 Alan           none      Initialization
    
==================================================================================================*/

#include "we_def.h"
#include "oem_secerr.h"
#include "oem_secevent.h"

#define WE_FILE_OPEN                   WeFile_Open
#define WE_FILE_CLOSE                  WeFile_Close
#define WE_FILE_WRITE                  WeFile_Write
#define WE_FILE_READ                   WeFile_Read
#define WE_FILE_SEEK                   WeFile_Seek
#define WE_FILE_REMOVE                 WeFile_Remove
#define WE_FILE_RENAME                 WeFile_Rename
#define WE_FILE_MKDIR                  WeFile_MakeDir
#define WE_FILE_RMDIR                  WeFile_RemoveDir
#define WE_FILE_GETSIZE_DIR            WeFile_GetDirSize
#define WE_FILE_READ_DIR               WeFile_ReadDir
#define WE_FILE_SETSIZE                WeFile_SetSize
#define WE_FILE_GETSIZE                WeFile_GetSize
#define WE_FILE_SELECT                 WeFile_Select
#define WE_FILE_CLOSE_ALL              WeFile_CloseAll

#define WE_FILE_CHECK_EXIST      WeFile_CheckFileExist

typedef enum{
    WE_FILE_SET_RDONLY,
    WE_FILE_SET_WRONLY,
    WE_FILE_SET_RDWR,
    WE_FILE_SET_APPEND,
    WE_FILE_SET_CREATE
}E_WE_FILE_MODE;

typedef enum{
    WE_FILE_SEEK_SET,
    WE_FILE_SEEK_CUR,
    WE_FILE_SEEK_END
}E_WE_FILE_SEEK_MODE;

typedef enum{
    WE_FILE_DIRTYPE,
    WE_FILE_FILETYPE
}E_WE_FILE_TYPE;



/*==================================================================================================
    FUNCTION: 
            WeFile_ConvertPathWeToBrew
    DESCRIPTION:
        Add WE and BREW prefix string, and convert to lower case.
    RETURN VALUE:
        Zero terminated WE_CHAR string. 
        NULL, if error.     
    REMARK:
        The parameter pcFileName should be like "file", "/file", "dir/file", "/dir/file", etc.
        The string returned must be freed by user.
==================================================================================================*/
WE_CHAR*
WeFile_ConvertPathWeToBrew
(
    const WE_CHAR *pcFileName
);


/*==================================================================================================
    FUNCTION: 
        WeFile_Open
    DESCRIPTION:
        Creates and/or opens a file with the name pszFileName. 
    RETURN VALUE:
        Possible error codes are: 
        E_WE_OK
            successful
        E_WE_ERROR_BAD_HANDLE
            hWeHandle is a disable handle.
        E_WE_ERROR_INVALID_PARAM
            hWeHandle is not a valid handle, or name is not a valid path name for a file. 
            or Errror in parameters e.g. mode specifies both WE_FILE_SET_RDONLY 
            and WE_FILE_SET_WRONLY. 
        E_WE_FILE_ERROR_PATH
            The named file does not exist. For example, the path is null or a component 
            of path does not exist, or the file itself does not exist and WE_FILE_SET_CREATE 
            is not set. 
        E_WE_FILE_ERROR_SIZE 
        E_WE_FILE_ERROR_FULL 
        E_WE_FILE_ERROR_EXIST 
            E_WE_FILE_SET_EXCL and E_WE_FILE_SET_CREATE are set and the named file exists. 
    REMARK:
        The open mode must be one E_WE_FILE_MODE type value.
        The file be opened or created is in the WE's directory, its' name should be like
        "file", "/file", "dir/file", "/dir/file", etc, and the name will be converted to lower case.
==================================================================================================*/
E_WE_ERROR  
WeFile_Open
(
    WE_HANDLE hWeHandle,
    WE_CHAR *pszFileName,
    E_WE_FILE_MODE eMode,
    WE_HANDLE *phWeFileHandle
);


/*==================================================================================================
    FUNCTION: 
        WeFile_Close
    DESCRIPTION:
        This function closes the file. 
    RETURN VALUE:
        Possible error codes are: 
        E_WE_OK
            successful
        E_WE_ERROR_BAD_HANDLE
            hWeFileHandle is a disable handle.
        E_WE_ERROR_INVALID_PARAM
            hWeFileHandle is not a valid handle, or name is not a valid path name for a file. 
    REMARK:
==================================================================================================*/
E_WE_ERROR 
WeFile_Close 
(
    WE_HANDLE hWeFileHandle
);


/*==================================================================================================
    FUNCTION: 
        WeFile_Write
    DESCRIPTION:
        This function attempts to write size bytes from the buffer. 
    RETURN VALUE:
        Possible error codes are: 
        E_WE_OK
            successful
        E_WE_ERROR_BAD_HANDLE
            hWeFileHandle is a disable handle.
        E_WE_ERROR_INVALID_PARAM
            hWeFileHandle is not a valid handle, or name is not a valid path name for a file. 
        E_WE_FILE_ERROR_ACCESS 
            E.g. attempt to write in a file opened as read only. 
        E_WE_ FILE_ERROR_SIZE 
        E_WE_ FILE_ERROR_FULL 
    REMARK:
        This function writes the specified number of bytes to an open file, the actual writen size
        is return by piWriteNum.
==================================================================================================*/
E_WE_ERROR 
WeFile_Write 
(
    WE_HANDLE hWeFileHandle,
    void *pvDataBuf, 
    WE_INT32 iBufSize,
    WE_INT32 *piWriteNum
);


/*==================================================================================================
    FUNCTION: 
        WeFile_Read
    DESCRIPTION:
        This function attempts to read a specified number of bytes from an open file.
    RETURN VALUE:
        Possible error codes are: 
        E_WE_OK
            successful
        E_WE_ERROR_BAD_HANDLE
            hWeFileHandle is a disable handle.
        E_WE_ERROR_INVALID_PARAM
            hWeFileHandle is not a valid handle, or name is not a valid path name for a file. 
    REMARK:
        The actual read size is return by piReadNum.

==================================================================================================*/
E_WE_ERROR 
WeFile_Read 
(
    WE_HANDLE hWeFileHandle,
    void *pvDataBuf, 
    WE_INT32 iBufSize,
    WE_INT32 *piReadNum
);




/*==================================================================================================
    FUNCTION: 
        WeFile_Seek
    DESCRIPTION:
        Moves the file pointer of an open file, allowing random access read and write operations.  
    RETURN VALUE:
        Possible error codes are: 
        E_WE_OK
            successful
        E_WE_ERROR_BAD_HANDLE
            hWeFileHandle is a disable handle.
        E_WE_ERROR_INVALID_PARAM
            hWeFileHandle is not a valid handle, or name is not a valid path name for a file. 
    REMARK:
        Moves the file pointer of an open file, allowing random access read and write operations. 
        If the seek operation exceeds the limits of the file an error is returned. Offset may be negative. 
==================================================================================================*/
E_WE_ERROR 
WeFile_Seek 
(
    WE_HANDLE hWeFileHandle, 
    WE_INT32 iOffset, 
    E_WE_FILE_SEEK_MODE eSeekMode,
    WE_INT32 *piNewPos
);


/*==================================================================================================
    FUNCTION: 
        WeFile_Remove
    DESCRIPTION:
        This function removes the file identified by fileName.   
    RETURN VALUE:
        Possible error codes are: 
        E_WE_OK
            successful
        E_WE_ERROR_BAD_HANDLE
            hWeFileHandle is a disable handle.
        E_WE_ERROR_INVALID_PARAM
            hWeFileHandle is not a valid handle, or name is not a valid path name for a file. 
    REMARK:
        This function removes the file identified by fileName. 
==================================================================================================*/
E_WE_ERROR 
WeFile_Remove 
(
    WE_HANDLE hWeHandle, 
    const WE_CHAR *pszFileName
);


/*==================================================================================================
    FUNCTION: 
        WeFile_Rename
    DESCRIPTION:
        This function renames the file identified by fileName.   
    RETURN VALUE:
        Possible error codes are: 
        E_WE_OK
            successful
        E_WE_ERROR_BAD_HANDLE
            hWeFileHandle is a disable handle.
        E_WE_ERROR_INVALID_PARAM
            hWeFileHandle is not a valid handle, or name is not a valid path name for a file. 
    REMARK:
        Renames the file or directory specified by srcName to the name specified by dstName. 
==================================================================================================*/
E_WE_ERROR 
WeFile_Rename 
(
    WE_HANDLE hWeHandle,
    const WE_CHAR *pszSrcName,
    const WE_CHAR *pszDstName
);


/*==================================================================================================
    FUNCTION: 
        WeFile_MakeDir
    DESCRIPTION:
        This function create a directory name by dirName.   
    RETURN VALUE:
        Possible error codes are: 
        E_WE_OK
            successful
        E_WE_ERROR_BAD_HANDLE
            hWeFileHandle is a disable handle.
        E_WE_ERROR_INVALID_PARAM
            hWeFileHandle is not a valid handle, or name is not a valid path name for a file. 
        E_WE_FILE_ERROR_FULL 
        E_WE_FILE_ERROR_PATH 
        E_WE_FILE_ERROR_EXIST

    REMARK:
        This function creates a directory named by dirName. 
==================================================================================================*/
E_WE_ERROR 
WeFile_MakeDir 
(
    WE_HANDLE hWeHandle, 
    const WE_CHAR *pszDirName
);


/*==================================================================================================
    FUNCTION: 
        WeFile_RemoveDir
    DESCRIPTION:
        This function removes a directory named by pathName.   
    RETURN VALUE:
        Possible error codes are: 
        E_WE_OK
            successful
        E_WE_ERROR_BAD_HANDLE
            hWeFileHandle is a disable handle.
        E_WE_ERROR_INVALID_PARAM
            hWeFileHandle is not a valid handle, or name is not a valid path name for a file. 
        E_WE_ FILE_ERROR_ACCESS 
        E_WE_FILE_ERROR_PATH 
    REMARK:
        This function removes a directory named by pathName.
==================================================================================================*/
E_WE_ERROR 
WeFile_RemoveDir 
(
    WE_HANDLE hWeHandle, 
    const WE_CHAR *pszDirName
);


/*==================================================================================================
    FUNCTION: 
        WeFile_GetDirSize
    DESCRIPTION:
        This function get the entry count in the directory.
    RETURN VALUE:
        Possible error codes are: 
        E_WE_OK
            successful
        E_WE_ERROR_BAD_HANDLE
            hWeFileHandle is a disable handle.
        E_WE_ERROR_INVALID_PARAM
            hWeFileHandle is not a valid handle, or name is not a valid path name for a file. 
        E_WE_FILE_ERROR_PATH 
    REMARK:
        This function should return the size, i.e the number of entries, of a directory. 
        Entry in this case is either a file or another directory. 
===================================================================================================*/
E_WE_ERROR 
WeFile_GetDirSize 
(
    WE_HANDLE hWeHandle,
    const WE_CHAR *pszDirName,
    WE_INT32* piSize
);


/*==================================================================================================
    FUNCTION: 
        WeFile_ReadDir
    DESCRIPTION:
        This function reads from a directory.   
    RETURN VALUE:
        Possible error codes are: 
        E_WE_OK
            successful
        E_WE_ERROR_BAD_HANDLE
            hWeFileHandle is a disable handle.
        E_WE_ERROR_INVALID_PARAM
            hWeFileHandle is not a valid handle, or name is not a valid path name for a file. 
        E_WE_FILE_ERROR_PATH 
    REMARK:
        This function reads from a directory and extracts information about an entry, 
        which could be a file or a directory. This function is not recursive, it doesn't read 
        the subdirectory of input directory.
        The "iPos" parameter value starts from 0.
==================================================================================================*/
E_WE_ERROR 
WeFile_ReadDir 
(
    WE_HANDLE hWeHandle, 
    const WE_CHAR *pszDirName, 
    WE_INT32 iPos, 
    WE_CHAR *pszNameBuf, 
    WE_INT32 iNameBufLength, 
    E_WE_FILE_TYPE *eType, 
    WE_INT32 *piSize
);


/*==================================================================================================
    FUNCTION: 
        WeFile_Select 
    DESCRIPTION:
        This function is used for registering a event  notify when the file is readable.
    RETURN VALUE:
        Possible error codes are: 
        E_WE_OK
            successful
        E_WE_ERROR_BAD_HANDLE
            hWeFileHandle is a disable handle.
        E_WE_ERROR_INVALID_PARAM
            hWeFileHandle is not a valid handle, or name is not a valid path name for a file. 
        E_WE_FILE_ERROR_PATH 
    REMARK:
        When the file is readable, WeFile will call the callback function register by WeMgr_RegMsgProcFunc().
==================================================================================================*/
E_WE_ERROR
WeFile_Select 
(
    WE_HANDLE hWeFileHandle, 
    E_WE_EVENT eEventType
);




/*==================================================================================================
    FUNCTION: 
        WeFile_SetSize 
    DESCRIPTION:
        This function sets the file size.
    RETURN VALUE:
        Possible error codes are: 
        E_WE_OK
            successful
        E_WE_ERROR_BAD_HANDLE
            hWeFileHandle is a disable handle.
        E_WE_ERROR_INVALID_PARAM
            hWeFileHandle is not a valid handle, or name is not a valid path name for a file. 
        E_WE_FILE_ERROR_PATH 
    REMARK:
        This function sets the size of the file associated with hWeFileHandle to "size", it can increase 
        or decrease the file size.
==================================================================================================*/
E_WE_ERROR
WeFile_SetSize 
(
    WE_HANDLE hWeFileHandle, 
    WE_INT32 iSize
);


/*==================================================================================================
    FUNCTION: 
        WeFile_GetSize
    DESCRIPTION:
        This function gets the file size.
    RETURN VALUE:
        Possible error codes are: 
        E_WE_OK
            successful
        E_WE_ERROR_BAD_HANDLE
            hWeFileHandle is a disable handle.
        E_WE_ERROR_INVALID_PARAM
            hWeFileHandle is not a valid handle, or name is not a valid path name for a file. 
        E_WE_FILE_ERROR_PATH 
    REMARK:
==================================================================================================*/
E_WE_ERROR
WeFile_GetSize 
(
    WE_HANDLE hWeHandle, 
    const WE_CHAR *pszFileName,
    WE_INT32* piSize
);


/*==================================================================================================
    FUNCTION: 
        WeFile_CloseAll
    DESCRIPTION:
        This function closes all files associated with hWeHandle.
    RETURN VALUE:
        Possible error codes are: 
        E_WE_OK
            successful
        E_WE_ERROR_BAD_HANDLE
            hWeFileHandle is a disable handle.
        E_WE_ERROR_INVALID_PARAM
            hWeFileHandle is not a valid handle, or name is not a valid path name for a file. 
        E_WE_FILE_ERROR_PATH 
    REMARK:
==================================================================================================*/
E_WE_ERROR
WeFile_CloseAll 
(
    WE_HANDLE hWeHandle
);


#endif 
