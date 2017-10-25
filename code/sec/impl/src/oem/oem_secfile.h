/*=====================================================================================
    FILE NAME :
        oem_secfile.h
    MODULE NAME :
        WEA
    GENERAL DESCRIPTION
        This file declare API of file.
    TECHFAITH Software Confidential Proprietary(c)
        Copyright 2006 by TECHFAITH Software. All Rights Reserved.
=======================================================================================
    Revision History
    Modification Tracking
    Date       Author         Number    Description of changes
    ---------- -------------- --------- --------------------------------------
    2006-06-01 Alan              none        Initialization
    
=====================================================================================*/
#ifndef __OEM_SECFILE_H__
#define __OEM_SECFILE_H__


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



/*******************************************************************************
    FUNCTION: 
            WeFile_ConvertPathWeToBrew
    DESCRIPTION:
        Add WE and BREW prefix string, and convert to lower case.
    RETURN VALUE:
        Zero terminated WE_CHAR string. 
        NULL, if error.        
    REMARK:
           
*******************************************************************************/
WE_CHAR*
WeFile_ConvertPathWeToBrew
(
    const WE_CHAR *pcFileName
);


/*====================================================================================
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
        
=====================================================================================*/
E_WE_ERROR  
WeFile_Open
(
    WE_HANDLE hWeHandle,
    WE_CHAR *pszFileName,
    E_WE_FILE_MODE eMode,
    WE_HANDLE *phWeFileHandle
);


/*====================================================================================
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
        
=====================================================================================*/
E_WE_ERROR 
WeFile_Close 
(
    WE_HANDLE hWeFileHandle
);


/*====================================================================================
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
        
=====================================================================================*/
E_WE_ERROR 
WeFile_Write 
(
    WE_HANDLE hWeFileHandle,
    void *pvDataBuf, 
    WE_LONG lBufSize,
    WE_LONG *plWriteNum
);


/*====================================================================================
    FUNCTION: 
        WeFile_Read
    DESCRIPTION:
        This function attempts to read size bytes from the buffer. 
    RETURN VALUE:
        Possible error codes are: 
        E_WE_OK
            successful
        E_WE_ERROR_BAD_HANDLE
            hWeFileHandle is a disable handle.
        E_WE_ERROR_INVALID_PARAM
            hWeFileHandle is not a valid handle, or name is not a valid path name for a file. 
    REMARK:
       
=====================================================================================*/
E_WE_ERROR 
WeFile_Read 
(
    WE_HANDLE hWeFileHandle,
    void *pvDataBuf, 
    WE_LONG lBufSize,
    WE_LONG *plReadNum
);



/*====================================================================================
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
        
=====================================================================================*/
E_WE_ERROR 
WeFile_Seek 
(
    WE_HANDLE hWeFileHandle, 
    WE_LONG lOffset, 
    E_WE_FILE_SEEK_MODE eSeekMode,
    WE_INT32 *piNewPos
);


/*====================================================================================
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
        
=====================================================================================*/
E_WE_ERROR 
WeFile_Remove 
(
    WE_HANDLE hWeHandle, 
    const WE_CHAR *pszFileName
);


/*=====================================================================================
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
        
=====================================================================================*/
E_WE_ERROR 
WeFile_Rename 
(
    WE_HANDLE hWeHandle,
    const WE_CHAR *pszSrcName,
    const WE_CHAR *pszDstName
);


/*====================================================================================
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
        
=====================================================================================*/
E_WE_ERROR 
WeFile_MakeDir 
(
    WE_HANDLE hWeHandle, 
    const WE_CHAR *pszDirName
);


/*====================================================================================
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
        
=====================================================================================*/
E_WE_ERROR 
WeFile_RemoveDir 
(
    WE_HANDLE hWeHandle, 
    const WE_CHAR *pszDirName
);


/*====================================================================================
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
        
=====================================================================================*/
E_WE_ERROR 
WeFile_GetDirSize 
(
    WE_HANDLE hWeHandle,
    const WE_CHAR *pszDirName,
    WE_INT32* piSize
);


/*====================================================================================
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
        
=====================================================================================*/
E_WE_ERROR 
WeFile_ReadDir 
(
    WE_HANDLE hWeHandle, 
    const WE_CHAR *pszDirName, 
    WE_INT32 iPos, 
    WE_CHAR *pszNameBuf, 
    WE_INT32 iNameBufLength, 
    E_WE_FILE_TYPE *eType, 
    WE_LONG *plSize
);


/*====================================================================================
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
        
=====================================================================================*/
E_WE_ERROR
WeFile_Select 
(
    WE_HANDLE hWeFileHandle, 
    E_WE_EVENT eEventType
);


/*====================================================================================
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
        
=====================================================================================*/
E_WE_ERROR
WeFile_SetSize 
(
    WE_HANDLE hWeFileHandle, 
    WE_LONG lSize
);


/*====================================================================================
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
=====================================================================================*/
E_WE_ERROR
WeFile_GetSize 
(
    WE_HANDLE hWeHandle, 
    const WE_CHAR *pszFileName,
    WE_LONG* plSize
);


/*====================================================================================
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
=====================================================================================*/
E_WE_ERROR
WeFile_CloseAll 
(
    WE_HANDLE hWeHandle
);

/*==================================================================================================
FUNCTION: 
    WeFile_CheckFileExist
DESCRIPTION: 
    This function check that the file is exist or not.
ARGUMENTS PASSED:
    WE_HANDLE hWeHandle: Pointer to the IFileMgr Interface object
    WE_CHAR *pszFileName: file name
RETURN VALUE:
    Possible error codes are: 
    E_WE_OK
        successful
    E_WE_ERROR_BAD_HANDLE
        hWeFileHandle is a disable handle.
    E_WE_ERROR_INVALID_PARAM
        hWeFileHandle is not a valid handle, or name is not a valid path name for a file. 
    E_WE_FILE_ERROR_PATH 
==================================================================================================*/

E_WE_ERROR
WeFile_CheckFileExist(WE_HANDLE hWeHandle, 
                                const WE_CHAR *pszFileName);

#endif 
